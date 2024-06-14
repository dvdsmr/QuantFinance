#include <iostream>
#include "options.h"

int main()
{
	std::cout << "You hold a strangle (a call and a put at different strikes) for AAPL. Please enter the strike price of your call: ";
	double strikeCall{};
	std::cin >> strikeCall;

	std::cout << "Please enter the strike price of your put: ";
	double strikePut{};
	std::cin >> strikePut;

	std::cout << "Please enter the spot price: ";
	double spot{};
	std::cin >> spot;

	std::cout << "The intrinsic value of your strangle is " << Options::strangleIntrinsicValue(strikeCall, strikePut, spot) << ".\n";

	return 0;


}