#include "options.h"
#include "distributions.h"
#include <algorithm>
#include <iostream>
#include <cmath>


// ToDo: Exception handling for invalid prices!
namespace Options
{

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

	namespace Pricing
	{
		namespace BinomialOneStep
		{
			auto call(double riskFreeRate, double upTick, double strike, double spot, double dividendYield) -> double
			{
				const double downTick = 1.0 / upTick;
				const double riskFreeUpTickProb{ (riskFreeRate - downTick - dividendYield) / (upTick - downTick) };
				return 1.0 / riskFreeRate * (riskFreeUpTickProb * Options::Payoffs::call(strike, upTick * spot) +
										   (1- riskFreeUpTickProb) * Options::Payoffs::call(strike, downTick * spot));
			}
			auto put(double riskFreeRate, double upTick, double strike, double spot, double dividendYield) -> double
			{
				const double downTick = 1.0 / upTick;
				const double riskFreeUpTickProb{ (riskFreeRate - downTick - dividendYield) / (upTick - downTick) };
				return 1.0 / riskFreeRate * (riskFreeUpTickProb * Options::Payoffs::put(strike, upTick * spot) +
											(1- riskFreeUpTickProb) * Options::Payoffs::put(strike, downTick * spot));
			}
		}


		namespace BSM
		{
			auto call(double riskFreeReturn, double vol, double maturity, double strike, double spot, double dividendYield) -> double
			{
				double d1 = (std::log(spot / strike) + (riskFreeReturn - dividendYield + vol * vol / 2.) * maturity) / vol / std::sqrt(maturity);
				double d2 = d1 - vol * std::sqrt(maturity);
				return spot * std::exp(-dividendYield * maturity) * Distributions::CDFs::standardNormal(d1)
					- strike * std::exp(-riskFreeReturn * maturity) * Distributions::CDFs::standardNormal(d2);
			}

			// given the call price, we use put-call parity to price puts
			auto put(double riskFreeReturn, double vol, double maturity, double strike, double spot, double dividendYield) -> double
			{
				double callPrice = Options::Pricing::BSM::call(riskFreeReturn, vol, maturity, strike, spot, dividendYield);
				return callPrice + strike * std::exp(-riskFreeReturn * maturity) - spot * std::exp(-dividendYield * maturity);
			}
		}
	}

	// Starting from here, we test the option functions
	void binomialPricingUnitTest()
	{
		const double riskFreeRate{ 1.05 };
		const double upTick{ 1.1 };
		const double strike{ 109.0 };
		const double spot{ 100.0 };

		std::cout << "The value of the call option is " << Options::Pricing::BinomialOneStep::call(riskFreeRate, upTick, strike, spot);
	}

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


