#include "options.h"
#include "distributions.h"
#include "xyvals.h"
#include "numpy.h"
#include <algorithm>
#include <iostream>
#include <cmath>
#include <string>
#include <string_view>


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

			auto callGrid(double riskFreeRate, double upTick, double strike, double spot, int length, double dividendYield) -> PriceGrid
			{
				const double downTick = 1.0 / upTick;
				const double riskFreeUpTickProb{ (riskFreeRate - downTick - dividendYield) / (upTick - downTick) };

				PriceGrid priceGrid{ "BSM call price grid", "Periods to maturity", np::linspace<int>(length,0,length) };


				// build list of price vectors starting from the terminal prices
				priceGrid.m_gridVals.push_back(std::vector<double>{});
				for (int i{ 0 }; i < length; ++i)
				{
					priceGrid.m_gridVals[0].push_back(Options::Payoffs::call(strike,spot*std::pow(upTick,i)*std::pow(downTick,length-1-i)));
				}
				for (std::size_t time{ 1 }; time < length; ++time)
				{
					priceGrid.m_gridVals.push_back(std::vector<double>{});
					for (std::size_t state{ 0 }; state < length - time; ++state)
					{
						priceGrid.m_gridVals[time].push_back(1.0 / riskFreeRate * (riskFreeUpTickProb * priceGrid.m_gridVals[time - 1][state + 1] + (1 - riskFreeUpTickProb) * priceGrid.m_gridVals[time - 1][state]));
					}
				}

				return priceGrid;
			}
		}


		// todo: don't repeat d1, d2 definition in every function
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

			auto callDelta(double riskFreeReturn, double vol, double maturity, double strike, double spot, double dividendYield) -> double
			{
				double d1 = (std::log(spot / strike) + (riskFreeReturn - dividendYield + vol * vol / 2.) * maturity) / vol / std::sqrt(maturity);
				return std::exp(-dividendYield * maturity) * Distributions::CDFs::standardNormal(d1);
			}

			// calculate put delta using put-call parity
			auto putDelta(double riskFreeReturn, double vol, double maturity, double strike, double spot, double dividendYield) -> double
			{
				return callDelta(riskFreeReturn, vol, maturity, strike, spot, dividendYield) - std::exp(-dividendYield * maturity);
			}

			auto callGamma(double riskFreeReturn, double vol, double maturity, double strike, double spot, double dividendYield) -> double
			{
				double d1 = (std::log(spot / strike) + (riskFreeReturn - dividendYield + vol * vol / 2.) * maturity) / vol / std::sqrt(maturity);
				return std::exp(-dividendYield * maturity) * Distributions::PDFs::standardNormal(d1) / vol / spot / std::sqrt(maturity);
			}

			// by put call parity, Gamma is the same for puts and calls in the BSM model
			auto putGamma(double riskFreeReturn, double vol, double maturity, double strike, double spot, double dividendYield) -> double
			{
				return callGamma(riskFreeReturn,vol,maturity,strike,spot,dividendYield);
			}

			auto callVega(double riskFreeReturn, double vol, double maturity, double strike, double spot, double dividendYield) -> double
			{
				double d1 = (std::log(spot / strike) + (riskFreeReturn - dividendYield + vol * vol / 2.) * maturity) / vol / std::sqrt(maturity);
				return std::exp(-dividendYield * maturity) * Distributions::PDFs::standardNormal(d1) * spot * std::sqrt(maturity);
			}

			// put call parity
			auto putVega(double riskFreeReturn, double vol, double maturity, double strike, double spot, double dividendYield) -> double
			{
				return callVega(riskFreeReturn, vol, maturity, strike, spot, dividendYield);
			}

			auto callTheta(double riskFreeReturn, double vol, double maturity, double strike, double spot, double dividendYield) -> double
			{
				double d1 = (std::log(spot / strike) + (riskFreeReturn - dividendYield + vol * vol / 2.) * maturity) / vol / std::sqrt(maturity);
				double d2 = d1 - vol * std::sqrt(maturity);
				return -std::exp(-dividendYield * maturity) * spot * Distributions::PDFs::standardNormal(d1) * vol / 2.0 / std::sqrt(maturity)
					+ dividendYield * std::exp(-dividendYield * maturity) * spot * Distributions::CDFs::standardNormal(d1)
					- riskFreeReturn * strike * std::exp(-riskFreeReturn * maturity) * Distributions::CDFs::standardNormal(d2);
			}

			// using put call parity
			auto putTheta(double riskFreeReturn, double vol, double maturity, double strike, double spot, double dividendYield) -> double
			{
				return callTheta(riskFreeReturn, vol, maturity, strike, spot, dividendYield)
					+ riskFreeReturn * std::exp(-riskFreeReturn * maturity) * strike
					- dividendYield * std::exp(-dividendYield * maturity) * spot;
			}

			namespace DataGeneration
			{
				auto call(double riskFreeReturn, double vol, double maturity, double strike, double dividendYield) -> DataTable
				{
					// we initialize the data table consisting of the option prices for different spot-time pairs
					DataTable prices(static_cast<std::size_t>(101), static_cast<std::size_t>(101));

					// we define the time-spot grid on which we calculate the BSM option prices
					std::vector<double> spots{np::linspace<double>(strike - 10.0, strike + 10.0, 100)};
					std::vector<double> timesToMaturity{ np::linspace<double>(0., maturity, 100) };

					// we define the first column to be the time to maturity and the first row to be the spot
					for (std::size_t i{1}; i <= std::size(timesToMaturity); ++i)
					{
						prices.m_table[i][0] = timesToMaturity[i-1];
					}
					for (std::size_t j{ 1 }; j <= std::size(spots); ++j)
					{
						prices.m_table[0][j] = spots[j-1];
					}
					// arbitray initialization of corner element
					prices.m_table[0][0] = 0.0;

					// compute all prices and place them in the data table
					// ToDo: this should be optimized later (i, j are ugly)
					
					// define row and column indices
					std::size_t i{ 1 };
					std::size_t j{ 1 };
					for (const auto& time : timesToMaturity)
					{
						for (const auto& spot : spots)
						{
							prices.m_table[i][j] = Options::Pricing::BSM::call(riskFreeReturn, vol, time, strike, spot, dividendYield);
							++j;
						}
						++i;
						j = 1;
					}
					return prices;
				}

				auto callPriceSurface(double riskFreeReturn, double vol, double spot, double dividendYield) -> LabeledTable
				{
					// number of row and col vals
					int numVals{ 100 };

					// initialize the price table
					using namespace std::string_view_literals;
					LabeledTable priceSurface("BSM european call price surface"sv,
											  "Time to maturity"sv, 
											  static_cast<std::size_t>(numVals),
											  "Strike"sv,
											  static_cast<std::size_t>(numVals),
											  "BSM option price"sv
											 );

					// define strikes and maturities
					priceSurface.m_rowVals = np::linspace<double>(0., 1., numVals);
					priceSurface.m_colVals = np::linspace<double>(spot-10., spot+10., numVals);

					// get the table values by the BSM model
					for (std::size_t row{ 0 }; row < std::size(priceSurface.m_rowVals); ++row)
					{
						for (std::size_t col{ 0 }; col < std::size(priceSurface.m_colVals); ++col)
						{
							priceSurface.m_table[row][col] = Options::Pricing::BSM::call(riskFreeReturn, 
																						vol, 
																						priceSurface.m_rowVals[row],	
																						priceSurface.m_colVals[col], 
																						spot, 
																						dividendYield);
						}
					}

					return priceSurface;
				}
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

// currently only simple options with a single strike price considered
double Option::samplePayoff(double time)
{
	double spot = m_underlying.samplePrice(time);
	switch (m_type)
	{
	case call: return Options::Payoffs::call(m_strike, spot);
	case put: return Options::Payoffs::put(m_strike, spot);
	default: return Options::Payoffs::call(m_strike, spot);
	}
}

