#include "options.h"
#include <algorithm>
#include <iostream>

namespace Options
{
	double callIntrinsicValue(double strikePrice, double spotPrice)
	{
		return std::max(spotPrice - strikePrice, 0.);
	}

	double putIntrinsicValue(double strikePrice, double spotPrice)
	{
		return std::max(strikePrice - spotPrice, 0.);
	}

	double straddleIntrinsicValue(double strikePrice, double spotPrice)
	{
		return callIntrinsicValue(strikePrice, spotPrice) + putIntrinsicValue(strikePrice, spotPrice);
	}

	double strangleIntrinsicValue(double strikeCall, double strikePut, double spotPrice)
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
