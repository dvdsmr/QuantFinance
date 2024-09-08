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

	/*

	double vol{ 0.2 };
	double maturity{ 1. };
	double riskFreeReturn{ 0.05 };
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
	std::cout << callPriceGrid.m_gridVals[5][4];

	*/

	// advanced topics in derivative pricing

	double spot = 171.01;
	double strike = 180.0;
	double maturity = 1.0;
	double interest = 0.03;
	double dividendYield = 0.0;
	[[maybe_unused]] double drift = 0.05;
	double vol = 0.1;


	double price{ Options::Pricing::BSM::call(interest, vol, maturity, strike, spot, dividendYield) };
	double delta{ Options::Pricing::BSM::callDelta(interest, vol, maturity, strike, spot, dividendYield) };
	double gamma{ Options::Pricing::BSM::callGamma(interest, vol, maturity, strike, spot, dividendYield) };
	double vega{ Options::Pricing::BSM::callVega(interest, vol, maturity, strike, spot, dividendYield) };
	double theta{ Options::Pricing::BSM::callTheta(interest, vol, maturity, strike, spot, dividendYield) };

	std::cout << "price: " << price << "\n";
	std::cout << "delta: " << delta << "\n";
	std::cout << "gamma: " << gamma << "\n";
	std::cout << "vega: " << vega << "\n";
	std::cout << "theta: " << theta << "\n";

	// first delta hedge
	double pricePaid = delta * spot - price;
	std::cout << "Price paid for delta hedged portfolio: " << pricePaid << "\n";

	// second delta hedge
	double newSpot{ 180.2 };
	double newMaturity{ 1. / 12. * 11. };
	double newDelta{ Options::Pricing::BSM::callDelta(interest, vol, newMaturity, strike, newSpot, dividendYield) };
	std::cout << "The new delta after one month is " << newDelta << "\n";
	std::cout << "Hence, we have to buy an additional " << newDelta - delta << " shares.\n";
	std::cout << "This will cost " << (newDelta - delta) * newSpot << "$.\n";

	return 0;
}