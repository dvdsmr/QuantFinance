#include "options.h"
#include "compounding.h"
#include "Random.h"
#include "distributions.h"
#include "securities.h"
#include "sdes.h"
#include "saving.h"
#include "xyvals.h"
#include "out.h"
#include "adam.h"
#include "numpy.h"
#include "copola.h"
#include "volatility.h"
#include "fft.h"
#include "calibrate.h"
#include "pso.h"
#include "reading.h"
#include <iostream>
#include <iostream>
#include <vector>
#include <string>
#include <cassert>

auto main() -> int
{

	//LabeledTable volSurface{ Volatility::Surface::testCalibration() };
	//Saving::write_labeledTable_to_csv("Data/ArtificalVolSurface.csv", volSurface);

	//FFT::UnitTests::separateModes();
	//FFT::UnitTests::dft();
	//FFT::UnitTests::pricingfft();

	//Calibrate::testHeston();

	//Calibrate::testBSM();

	//Calibrate::testVarianceGamma();

	//testPSO();

	//SDE::Testing::saveMCsamples();

	//SDE::Testing::saveHestonPaths();

	//SDE::Testing::saveVarianceGammaPaths();

	// We read in options data from yahoo finance
	std::string filename = "Data/yFinance/AAPL_callPriceSurface.csv"; // Replace with your file name
	std::vector<std::vector<std::string>> csvData = Reading::readCSV(filename);

	// Output the CSV data
	for (const auto& row : csvData) {
		for (const auto& cell : row) {
			std::cout << cell << " ";
		}
		std::cout << std::endl;
	}

	// we try to estimate the implied Vol with the BSM model. Market params are (for the moment) set to
	[[maybe_unused]] double riskFreeReturn{ 0.02 };
	[[maybe_unused]] double dividendYield{ 0.0 };
	[[maybe_unused]] double spot{ 225.12 };
	
	// calibrate implied vol
	double volGuess{ 0.4 };
	Adam adam{};
	for (std::size_t row{1}; row < std::size(csvData); ++row)
	{
		// index for mat is 1
		// index for price is 5
		// index for strike is 2
		double truePrice{ std::stod(csvData[row][4]) };
		double mat{ std::stod(csvData[row][1]) };
		double strike{ std::stod(csvData[row][2]) };
		// define adam target function and derivative
		auto func
		{
			[&](double vol) {
				double price{ Options::Pricing::BSM::call(riskFreeReturn, vol, mat, strike, spot, dividendYield) };
				return (price - truePrice) * (price - truePrice);
			}
		};
		auto deriv
		{
			[&](double vol) {
				double price{ Options::Pricing::BSM::call(riskFreeReturn, vol, mat, strike, spot, dividendYield) };
				return 2 * (price - truePrice) * Options::Pricing::BSM::callVega(riskFreeReturn, vol, mat, strike, spot, dividendYield);
			}
		};

		adam.set_state(volGuess);
		[[maybe_unused]] double calVol{ adam.optimize(func, deriv, false) };
		std::cout << "Yahoo implied vol is " << csvData[row][6] << ". " << "BSM calibrated implied vol is " << calVol << ".\n";
	}

	return 0;
}