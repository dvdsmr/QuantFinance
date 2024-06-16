#include "options.h"
#include <algorithm>
#include <iostream>

namespace Options
{
	auto callIntrinsicValue(double strikePrice, double spotPrice) -> double
	{
		return std::max(spotPrice - strikePrice, 0.);
	}

	auto putIntrinsicValue(double strikePrice, double spotPrice) -> double
	{
		return std::max(strikePrice - spotPrice, 0.);
	}

	auto straddleIntrinsicValue(double strikePrice, double spotPrice) -> double
	{
		return callIntrinsicValue(strikePrice, spotPrice) + putIntrinsicValue(strikePrice, spotPrice);
	}

	auto strangleIntrinsicValue(double strikeCall, double strikePut, double spotPrice) -> double
	{
		return callIntrinsicValue(strikeCall, spotPrice) + putIntrinsicValue(strikePut, spotPrice);
	}


	// Starting from here, we test the option functions

	void strangleUnitTest()
	{
		std::cout << "You hold a strangle (a call and a put at different strikes) for AAPL.\n" << "Please enter the strike price of your call : ";
		double strikeCall{};
		std::cin >> strikeCall;

		std::cout << "Please enter the strike price of your put: ";
		double strikePut{};
		std::cin >> strikePut;

		std::cout << "Please enter the spot price: ";
		double spot{};
		std::cin >> spot;

		std::cout << "The intrinsic value of your strangle is " << Options::strangleIntrinsicValue(strikeCall, strikePut, spot) << ".\n";
	}


}
