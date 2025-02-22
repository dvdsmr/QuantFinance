#include "compounding.h"
#include <iostream>
#include <cmath>

namespace Compounding
{
	auto discrete(double value, int timeUnits, double rate, int compoundingsPerTimeUnit = 1) -> double
	{
		return std::pow(1.0 + rate/compoundingsPerTimeUnit, timeUnits*compoundingsPerTimeUnit) * value;
	}
	auto continuous(double value, double time, double rate) -> double
	{
		return std::exp(rate*time) * value;
	}

	void continuousUnitTest()
	{
		std::cout << "Enter the value of your investment (in $) right now: ";
		double value{};
		std::cin >> value;

		std::cout << "Enter the risk free rate of return (per year and with continuous compounding): ";
		double rate{};
		std::cin >> rate;

		std::cout << "Enter the number of years you want to keep the investment: ";
		double time{};
		std::cin >> time;

		std::cout << "At the risk free rate of " << rate << " and over a time period of " << time << " years, your initial investment of "
			<< value << "$ will change to " << continuous(value, time, rate) << "$.";

	}

	void discreteUnitTest()
	{
		std::cout << "Enter the value of your investment (in $) right now: ";
		double value{};
		std::cin >> value;

		std::cout << "Enter the risk free rate of return (per year and with discrete compounding): ";
		double rate{};
		std::cin >> rate;

		std::cout << "Enter the number of years you want to keep the investment: ";
		int years{};
		std::cin >> years;

		std::cout << "At the risk free rate of " << rate << " and over a time period of " << years << " years, your initial investment of "
			<< value << "$ will change to " << discrete(value, years, rate) << "$.";

	}

}