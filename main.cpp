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

	int n{ 10 };
	int k{ 5 };
	std::cout << Distributions::Utils::binomialCoefficient(n,k);

}