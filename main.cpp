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
#include <iostream>
#include <iostream>
#include <vector>
#include <string>
#include <cassert>

auto main() -> int
{
	//Options::callCreditSpreadUnitTest();

	//Compounding::discreteUnitTest();

	//std::cout << Distributions::MomentGeneratingFunctions::normal(1.0, 0.0, 1.0);

	//Options::binomialPricingUnitTest();

	/*

	double vol{ 0.2 };
	double maturity{ 1. };
	double riskFreeReturn{ 1.05 };
	double strike{ 120. };
	double spot{ 100. };
	double dividendYield{ 0.03 };
	int length{ 10 };
	double upTick{ 1.05 };

	double price = Options::Pricing::BSM::call(riskFreeReturn, vol, maturity, strike, spot);
	std::cout << "Price of the call option in the BSM model is " << price << "\n";


	SimpleStock stock1{};
	std::cout << "vol of the stock is " << stock1.getVol() << "\n";
	stock1.setVol(0.02);
	std::cout << "vol of the stock is " << stock1.getVol() << "\n";


	Option opt1{};
	std::cout << "sampling stock price " << stock1.samplePrice(2.,"bachelier") << ". \n";
	std::cout << "sampling option price " << opt1.samplePayoff(2.) << ". \n";

	//std::vector<double> path{ SDE::geometricBrownianMotionPath(100.0, 1.0, 100, 0.04, 0.05) };
	//Saving::write_vector_to_file(path, "Data/GBMpath");
	std::vector<double> path{ Saving::read_vector_from_file<double>("Data/GBMpath") };
	for (int i{ 0 }; i <= 99; i++)
	{
		std::cout << path[static_cast<std::size_t>(i)] << " ";
	}

	XYVals spath{ SDE::geometricBrownianMotionPath(100.0, 1.0, 100, 0.04, 0.05) };
	for (int i{ 0 }; i <= 99; i++)
	{
		std::cout << spath.m_yVals[static_cast<std::size_t>(i)] << " ";
	}

	//HestonPath(initialState, terminalTime, timePoints, drift, initialVariance, longVariance, correlation, reversionRate, volVol)
	//XYVals spath2{ SDE::HestonPath(100.0, 1.0, 1000, 0.09, 8., 15.,0.2,0.3,0.2) };
	//Saving::write_xyvals_to_csv("Data/stockPath1.csv", spath2);
	//XYVals spath3{ SDE::HestonPath(100.0, 1.0, 1000, 0.09, 8., 15.,0.2,0.3,0.2) };
	//Saving::write_xyvals_to_csv("Data/stockPath2.csv", spath3);
	//XYVals spath4{ SDE::HestonPath(100.0, 1.0, 1000, 0.09, 8., 15.,0.2,0.3,0.2) };
	//Saving::write_xyvals_to_csv("Data/stockPath3.csv", spath4);

	DataTable table(static_cast<std::size_t>(10), static_cast<std::size_t>(10));
	std::cout << table.m_table[0][0];

	// generate call option price data
	//DataTable optionPrices{ Options::Pricing::BSM::DataGeneration::call(0.05, 0.04, 1., 100., 0.01) };
	//Saving::write_table_to_csv("Data/callPrices.csv", optionPrices);

	std::cout << "\n";
	LabeledTable priceSurface{ Options::Pricing::BSM::DataGeneration::callPriceSurface(0.05, 0.1, 100., 0.02) };
	std::cout << priceSurface.m_rowLabel << " " << priceSurface.m_colLabel << " " << priceSurface.m_tableLabel << "\n";

	Saving::write_labeledTable_to_csv("Data/PriceSurface.csv", priceSurface);

	PriceGrid callPriceGrid{ Options::Pricing::BinomialOneStep::callGrid(riskFreeReturn,upTick,strike,spot,length,dividendYield) };
	std::cout << callPriceGrid.m_gridName << " " << callPriceGrid.m_xLabel << "\n";
	std::cout << callPriceGrid.m_gridVals[0][0];

	outputPriceGrid(callPriceGrid);
	*/

	//testAdam();

	// we test the vol calibrations for AAPL call option on 09/13/24 for expiry on 09/20/24
	std::vector<double> strikes{ np::linspace<double>(197.5,245.0,20) };
	std::vector<double> mids{25.05,22.75,20.35,17.83,15.45,12.90,10.55,8.35,6.28,4.48,2.98,1.84,1.05,0.55,0.28,0.14,0.08,0.05,0.04,0.03};
	assert(std::size(strikes) == std::size(mids));
	double maturity{ 0.028 };
	double interest{ 0.053 }; // short term US treasury yield on 09/13/2024
	double dividendYield{ 0.0045 };
	double spot{ 222.50 };
	for (std::size_t i{ 0 }; i < std::size(strikes); ++i)
	{
		auto func
		{
			[&](double vol) {
				double price{ Options::Pricing::BSM::call(interest, vol, maturity, strikes[i], spot, dividendYield)};
				return (price - mids[i]) * (price - mids[i]);
			}
		};
		auto deriv
		{
			[&](double vol) {
				double price{ Options::Pricing::BSM::call(interest, vol, maturity, strikes[i], spot, dividendYield)};
				return 2 * (price - mids[i]) * Options::Pricing::BSM::callVega(interest, vol, maturity, strikes[i], spot, dividendYield);
			}
		};
		Adam adam{ 0.7 };
		double optVol{ adam.optimize(func, deriv, false) };
		std::cout << "Adam found vol of " << optVol << " for the strike of " << strikes[i] <<  ".\n";
	}

	// test Gaussian copola default probs
	std::vector<double> defaultProbs{0.2, 0.2, 0.06, 0.3, 0.4, 0.65, 0.3, 0.23, 0.02, 0.12, 0.134, 0.21,
		0.08, 0.1, 0.1, 0.02, 0.3, 0.015, 0.2, 0.03};
	//std::vector<double> defaultProbs{ 0.5,0.5,0.5 };

	std::vector<double> probOfNDefaults{ Copola::Gaussian::probOfNDefaultsUncorrolated(defaultProbs) };
	
	double sum{0.0};
	for (std::size_t i{ 0 }; i < std::size(probOfNDefaults); ++i)
	{
		sum += probOfNDefaults[i];
		std::cout << "The probability of exactly " << i << " defaults is " << probOfNDefaults[i] << ".\n";
	}
	std::cout << "The sum of all probs is " << sum << ".\n";

	LabeledTable volSurface{ Volatility::Surface::testCalibration() };

	return 0;
}