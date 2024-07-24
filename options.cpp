#include "options.h"
#include <algorithm>
#include <iostream>


// ToDo: Exception handling for invalid prices!
namespace Options
{
	enum class Payoff
	{
		call,
		put,
	};

	enum class Position
	{
		longPosition,
		shortPosition,
	};

	namespace Payoffs
	{
		auto call(double strikePrice, double spotPrice) -> double
		{
			return std::max(spotPrice - strikePrice, 0.);
		}

		auto put(double strikePrice, double spotPrice) -> double
		{
			return std::max(strikePrice - spotPrice, 0.);
		}

		auto straddle(double strikePrice, double spotPrice) -> double
		{
			return call(strikePrice, spotPrice) + put(strikePrice, spotPrice);
		}

		auto strangle(double strikeCall, double strikePut, double spotPrice) -> double
		{
			return call(strikeCall, spotPrice) + put(strikePut, spotPrice);
		}

		auto putDebitSpread(double lower, double higher, double spotPrice) -> double
		{
			return put(higher, spotPrice) - put(lower, spotPrice);
		}

		auto putCreditSpread(double lower, double higher, double spotPrice) -> double
		{
			return put(lower, spotPrice) - put(higher, spotPrice);
		}

		auto callDebitSpread(double lower, double higher, double spotPrice) -> double
		{
			return call(lower, spotPrice) - call(higher, spotPrice);
		}

		auto callCreditSpread(double lower, double higher, double spotPrice) -> double
		{
			return call(higher, spotPrice) - call(lower, spotPrice);
		}
	}

	// Starting from here, we test the payoff functions

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

		std::cout << "The payoff of your strangle is " << Options::Payoffs::strangle(strikeCall, strikePut, spot) << ".\n";
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

		std::cout << "The payoff of your spread " << Options::Payoffs::callCreditSpread(strikeShortCall, strikeCall, spot) << ".\n";
	}

}


// Template for option class

class Option
{
public:
	Option() = default;

private:
	double strikePrice{ 0. };
	double maturity{ 10. };
	Options::Payoff m_payoff{ Options::Payoff::call };
	Options::Position m_position{ Options::Position::longPosition };
};
