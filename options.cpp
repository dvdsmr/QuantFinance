#include "options.h"
#include <algorithm>
#include <iostream>


// ToDo: Exception handling for invalid prices!


namespace Options
{
	auto callPayoff(double strikePrice, double spotPrice) -> double
	{
		return std::max(spotPrice - strikePrice, 0.);
	}

	auto putPayoff(double strikePrice, double spotPrice) -> double
	{
		return std::max(strikePrice - spotPrice, 0.);
	}

	auto straddlePayoff(double strikePrice, double spotPrice) -> double
	{
		return callPayoff(strikePrice, spotPrice) + putPayoff(strikePrice, spotPrice);
	}

	auto stranglePayoff(double strikeCall, double strikePut, double spotPrice) -> double
	{
		return callPayoff(strikeCall, spotPrice) + putPayoff(strikePut, spotPrice);
	}

	auto putDebitSpreadPayoff(double lower, double higher, double spotPrice) -> double
	{
		return putPayoff(higher, spotPrice) - putPayoff(lower, spotPrice);
	}

	auto putCreditSpreadPayoff(double lower, double higher, double spotPrice) -> double
	{
		return putPayoff(lower, spotPrice) - putPayoff(higher, spotPrice);
	}

	auto callDebitSpreadPayoff(double lower, double higher, double spotPrice) -> double
	{
		return callPayoff(lower, spotPrice) - callPayoff(higher, spotPrice);
	}

	auto callCreditSpreadPayoff(double lower, double higher, double spotPrice) -> double
	{
		return callPayoff(higher, spotPrice) - callPayoff(lower, spotPrice);
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

		std::cout << "The payoff of your strangle is " << Options::stranglePayoff(strikeCall, strikePut, spot) << ".\n";
	}

	void callCreditSpreadUnitTest()
	{
		std::cout << "You hold a call credit spread (bought a call and sold a call at lower strike) for AAPL.\n" << "Please enter the strike price of the call you are long: ";
		double strikeCall{};
		std::cin >> strikeCall;

		std::cout << "Please enter the strike price of the call you are short: ";
		double strikeShortCall{};
		std::cin >> strikeShortCall;

		std::cout << "Please enter the spot price: ";
		double spot{};
		std::cin >> spot;

		std::cout << "The payoff of your spread " << Options::callCreditSpreadPayoff(strikeShortCall, strikeCall, spot) << ".\n";
	}

}
