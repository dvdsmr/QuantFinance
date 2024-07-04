#include <iostream>
#include "options.h"
#include "compounding.h"
#include "Random.h"
#include "distributions.h"
#include <iostream>

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
	std::cout << "Price of the call option in the BSM model is " << price;

}