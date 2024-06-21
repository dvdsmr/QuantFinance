#include <iostream>
#include "options.h"
#include "compounding.h"
#include "Random.h"
#include <iostream>

auto main() -> int
{
	//Options::callCreditSpreadUnitTest();

	//Compounding::discreteUnitTest();

	for (int k{1}; k <= 10; k++)
	{
		std::cout << Random::normal(0.0, 1.0) << "\n";
	}

	return 0;
}