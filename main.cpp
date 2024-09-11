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


	// advanced topics in derivative pricing

	/*
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
	std::cout << "\n";

	// vol surface part
	double strikeSold{ 185. };
	double portfolioValue{ Options::Pricing::BSM::call(interest, vol, maturity, strikeSold, spot, dividendYield) - price };
	std::cout << "The value of the portfolio is " << portfolioValue << "\n";

	// with vol skew
	[[maybe_unused]]double vol180{ std::min(1.0,18.0 / 180.0) }; // not that this is equal to 0.1, which is the previous vol
	double vol185{ std::min(1.0,18.0 / 185.0) };
	double portfolioValueSkew{ Options::Pricing::BSM::call(interest, vol185, maturity, strikeSold, spot, dividendYield) - price };
	std::cout << "The value of the portfolio with skew is " << portfolioValueSkew << "\n";
	std::cout << "\n";

	// digital option pricing
	// note that without skew, the digital option price is fixed by the BSM model
	double digitalPrice{ -Options::Pricing::BSM::callStrikeDerivative(interest, vol, maturity, strike, spot, dividendYield) };
	std::cout << "The digital option price without skew is " << digitalPrice << "\n";

	// the derivative of the callStrikeDerivative is
	double digitalDelta{ -Options::Pricing::BSM::callStrikeSpotDerivativeApprox(interest, vol, maturity, strike, spot, dividendYield) };
	std::cout << "The delta of the digital is approximately " << digitalDelta << "\n";
	std::cout << "Hedging this will cost " << digitalDelta * spot << "$.\n";
	std::cout << "\n";

	// digital portfolio without skew
	double digitalPortfolioValue{ -Options::Pricing::BSM::callStrikeDerivative(interest, vol, maturity, strikeSold, spot, dividendYield) 
						 + Options::Pricing::BSM::callStrikeDerivative(interest, vol, maturity, strike, spot, dividendYield)
	};
	std::cout << "The value of the digital portfolio is " << digitalPortfolioValue << "\n";

	// digital portfolio with skew
	double vega185{ Options::Pricing::BSM::callVega(interest, vol185, maturity, strikeSold, spot, dividendYield) };
	double digitalPortfolioValueSkew{ -Options::Pricing::BSM::callStrikeDerivative(interest, vol185, maturity, strikeSold, spot, dividendYield) - vega185 * (-18.0/185.0/185.0)
						 + Options::Pricing::BSM::callStrikeDerivative(interest, vol, maturity, strike, spot, dividendYield) + vega * (-18.0/180.0/180.0)
	};
	std::cout << "The value of the digital portfolio with skew is " << digitalPortfolioValueSkew << "\n";

	std::cout << "\n";
	// Continuing assignment 
	spot = 100.;
	strike = 90.;
	interest = 0.05;
	maturity = 1.;
	dividendYield = 0.0;
	double volGuess{ 0.2 };
	std::cout << "The price with vol " << volGuess << " is " << Options::Pricing::BSM::call(interest, volGuess, maturity, strike, spot, dividendYield) << "\n";

	// with skew
	double vol90{ 0.3 * std::exp(-2.*(strike/100.-1.))};
	std::cout << "The strike of a vanilla Eurpoean put option is " << Options::Pricing::BSM::put(interest, vol90, maturity, strike, spot, dividendYield) << "\n";
	std::cout << "The delta of the put is " << Options::Pricing::BSM::putDelta(interest, vol90, maturity, strike, spot, dividendYield) << "\n";

	// we delta hedge the put by selling delta* the stock (so we're buying because delta is negative)
	double putPortfolioVal{ Options::Pricing::BSM::put(interest, vol90, maturity, strike, spot, dividendYield)
							- Options::Pricing::BSM::putDelta(interest, vol90, maturity, strike, spot, dividendYield)*spot };
	// now the price drops and vol surface shifts up
	double spotNew{ 95. };
	double volNew{ vol90 * 1.1 };
	double putportFolioValNew{ Options::Pricing::BSM::put(interest, volNew, maturity, strike, spotNew, dividendYield)
							- Options::Pricing::BSM::putDelta(interest, vol90, maturity, strike, spot, dividendYield) * spotNew };
	std::cout << "With the price drop and vol shift, we make " << putportFolioValNew - putPortfolioVal << " gains.\n";

	// comparing price changes in put option
	double priceFullChange{ Options::Pricing::BSM::put(interest, volNew, maturity, strike, spotNew, dividendYield)
							- Options::Pricing::BSM::put(interest, vol90, maturity, strike, spot, dividendYield) };
	double priceSpotChange{ Options::Pricing::BSM::put(interest, vol90, maturity, strike, spotNew, dividendYield)
							- Options::Pricing::BSM::put(interest, vol90, maturity, strike, spot, dividendYield) };
	double changeQuotient{ priceSpotChange / priceFullChange };
	std::cout << "The percentage of put price change due to the stock price drop is " << changeQuotient * 100. << "\n";

	*/

	return 0;
}