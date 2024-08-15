#include <iostream>
#include "options.h"
#include "compounding.h"
#include "Random.h"
#include "distributions.h"
#include "securities.h"
#include "sdes.h"
#include <iostream>
#include <vector>

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

	std::vector<double> path{ SDE::geometricBrownianMotionPath(100.0, 10.0, 100, 1.0, 2.0) };
	for (int i{ 0 }; i <= 99; i++)
	{
		std::cout << path[static_cast<std::size_t>(i)] << " ";
	}
}