#include <iostream>
#include "options.h"

int main()
{
	std::cout << "You hold a call option for AAPL. Please enter strike price: ";
	double strike{};
	std::cin >> strike;

	std::cout << "Please enter spot price: ";
	double spot{};
	std::cin >> spot;

	std::cout << "The intrinsic value of your call option is " << Options::callIntrinsicValue(strike, spot) << ".\n";

	return 0;
}