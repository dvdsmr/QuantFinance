#include <iostream>
#include "options.h"
#include "compounding.h"
#include "Random.h"
#include "distributions.h"
#include "securities.h"
#include "sdes.h"
#include "saving.h"
#include "xyvals.h"
#include <iostream>
#include <vector>
#include <string>

auto main() -> int
{
	//Options::callCreditSpreadUnitTest();

	//Compounding::discreteUnitTest();

	//std::cout << Distributions::MomentGeneratingFunctions::normal(1.0, 0.0, 1.0);

	//Options::binomialPricingUnitTest();

	double vol{ 0.2 };
	double maturity{ 1. };
	double riskFreeReturn{ 0.05 };
	double strike{ 120. };
	double spot{ 100. };

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
	XYVals spath2{ SDE::HestonPath(100.0, 1.0, 1000, 0.09, 8., 15.,0.2,0.3,0.2) };
	Saving::write_xyvals_to_csv("Data/stockPath1.csv", spath2);
	XYVals spath3{ SDE::HestonPath(100.0, 1.0, 1000, 0.09, 8., 15.,0.2,0.3,0.2) };
	Saving::write_xyvals_to_csv("Data/stockPath2.csv", spath3);
	XYVals spath4{ SDE::HestonPath(100.0, 1.0, 1000, 0.09, 8., 15.,0.2,0.3,0.2) };
	Saving::write_xyvals_to_csv("Data/stockPath3.csv", spath4);

	return 0;
}