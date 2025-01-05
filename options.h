#ifndef OPTION_RETURNS_H
#define OPTION_RETURNS_H
#include "out.h"
#include "sdes.h"
#include "securities.h"
#include "saving.h"
#include "distributions.h"
#include "xyvals.h"
#include "numpy.h"
#include "fft.h"
#include <functional>


namespace Options
{
	namespace Payoffs
	{

		enum class Type
		{
			call,
			put,
			straddle,
			strangle,
			callDebitSpread,
			callCreditSpread,
			putDebitSpread,
			putCreditSpread,
		};

		auto call(double strikePrice, double spotPrice) -> double;
		auto put(double strikePrice, double spotPrice) -> double;
		auto straddle(double strikePrice, double spotPrice) -> double;
		auto strangle(double strikeCall, double strikePut, double spotPrice) -> double;
		auto callDebitSpread(double lower, double higher, double spotPrice) -> double;
		auto callCreditSpread(double lower, double higher, double spotPrice) -> double;
		auto putDebitSpread(double lower, double higher, double spotPrice) -> double;
		auto putCreditSpread(double lower, double higher, double spotPrice) -> double;
	}

	namespace Pricing
	{
		namespace BinomialOneStep
		{
			auto call(double riskFreeRate, double upTick, double strike, double spot, double dividendYield=0.) -> double;
			auto put(double riskFreeRate, double upTick, double strike, double spot, double dividenYield=0.) -> double;
			auto callGrid(double riskFreeRate, double upTick, double strike, double spot, int length, double dividendYield) -> PriceGrid;

		}
		namespace BSM
		{
			auto _d1(double riskFreeReturn, double vol, double maturity, double strike, double spot, double dividendYield) -> double;
			auto _d2(double riskFreeReturn, double vol, double maturity, double strike, double spot, double dividendYield) -> double;

			auto call(double riskFreeReturn, double vol, double maturity, double strike, double spot, double dividendYield=0.) -> double;
			auto put(double riskFreeReturn, double vol, double maturity, double strike, double spot, double dividendYield=0.) -> double;

			auto callDelta(double riskFreeReturn, double vol, double maturity, double strike, double spot, double dividendYield) -> double;
			auto putDelta(double riskFreeReturn, double vol, double maturity, double strike, double spot, double dividendYield) -> double;

			auto callGamma(double riskFreeReturn, double vol, double maturity, double strike, double spot, double dividendYield) -> double;
			auto putGamma(double riskFreeReturn, double vol, double maturity, double strike, double spot, double dividendYield) -> double;

			auto callVega(double riskFreeReturn, double vol, double maturity, double strike, double spot, double dividendYield) -> double;
			auto putVega(double riskFreeReturn, double vol, double maturity, double strike, double spot, double dividendYield) -> double;

			auto callTheta(double riskFreeReturn, double vol, double maturity, double strike, double spot, double dividendYield) -> double;
			auto putTheta(double riskFreeReturn, double vol, double maturity, double strike, double spot, double dividendYield) -> double;

			auto callStrikeDerivative(double riskFreeReturn, double vol, double maturity, double strike, double spot, double dividendYield) -> double;
			auto callStrikeDerivativeApprox(double riskFreeReturn, double vol, double maturity, double strike, double spot, double dividendYield) -> double;
			auto callStrikeSpotDerivativeApprox(double riskFreeReturn, double vol, double maturity, double strike, double spot, double dividendYield) -> double;

			auto monteCarlo(const std::function<double(double)>& payoff, double riskFreeReturn, double vol, double maturity, double spot, double dividendYield) -> double;
			void testMonteCarlo();

			namespace DataGeneration
			{
				auto call(double riskFreeReturn, double vol, double maturity, double strike, double dividendYield) -> DataTable;
				auto callPriceSurface(double riskFreeReturn, double vol, double spot, double dividendYield) -> LabeledTable;
			}
		}

		namespace Bachelier
		{
			auto _dplus(double riskFreeReturn, double vol, double maturity, double strike, double spot, double dividendYield) -> double;

			auto call(double riskFreeReturn, double vol, double maturity, double strike, double spot, double dividendYield = 0.) -> double;
			auto put(double riskFreeReturn, double vol, double maturity, double strike, double spot, double dividendYield = 0.) -> double;

			auto callVega(double riskFreeReturn, double vol, double maturity, double strike, double spot, double dividendYield) -> double;
			auto putVega(double riskFreeReturn, double vol, double maturity, double strike, double spot, double dividendYield) -> double;

			auto monteCarlo(const std::function<double(double)>& payoff, double riskFreeReturn, double vol, double maturity, double spot, double dividendYield) -> double;
			void testMonteCarlo();
		}

		namespace CEV
		{
			auto _nu(double exponent) -> double;
			// WARNING: CEV PRICING FORMULAS ARE HIGHLY UNSTABLE
			auto call(double riskFreeReturn, double vol, double maturity, double strike, double spot, double dividendYield, double exponent) -> double;
			auto put(double riskFreeReturn, double vol, double maturity, double strike, double spot, double dividendYield, double exponent) -> double;
			auto monteCarlo(const std::function<double(double)>& payoff, double riskFreeReturn, double vol, double maturity, double spot, double dividendYield, double exponent) -> double;
			void test();
			void testMonteCarlo();
		}

		namespace MertonJump
		{
			auto monteCarlo(const std::function<double(double)>& payoff, double riskFreeReturn, double maturity, double spot, double dividendYield, double volatility, double meanJumpSize, double stdJumpSize, double expectedJumpsPerYear) -> double;
			auto fft(double strike, double riskFreeReturn, double maturity, double spot, double dividendYield, double volatility, double meanJumpSize, double stdJumpSize, double expectedJumpsPerYear, std::string_view type = "call") -> double;
			void testPricing();
		}

		namespace Heston
		{
			auto monteCarlo(const std::function<double(double)>& payoff, double riskFreeReturn, double maturity, double spot, double dividendYield, double initialVariance, double longVariance, double correlation, double reversionRate, double volVol) -> double;
			auto fft(double strike, double riskFreeReturn, double maturity, double spot, double dividendYield, double initialVariance, double longVariance, double correlation, double reversionRate, double volVol, std::string_view type = "call") -> double;
			void testPricing();
		}

		namespace VarianceGamma
		{
			auto monteCarlo(const std::function<double(double)>& payoff, double riskFreeReturn, double maturity, double spot, double dividendYield,double gammaDrift, double variance, double vol) -> double;
			auto fft(double strike, double riskFreeReturn, double maturity, double spot, double dividendYield, double gammaDrift, double variance, double vol, std::string_view type = "call") -> double;
			void testPricing();
		}

		namespace Utils
		{
			auto _discountedExpectedPayoff(const std::function<double(double)>& payoff, std::vector<double> predictedSpots, double riskFreeReturn, double maturity) -> double;
		}


	}
	
	const void testCallGrid();
	const void savePriceSurface();
	const void testPricing();
	void strangleUnitTest();
	void callCreditSpreadUnitTest();
	void binomialPricingUnitTest();


	// Pricing method for asian options
	namespace Exotic
	{
		namespace Asian
		{
			template <typename Params>
			auto arithmeticAverage(std::size_t days, std::size_t numPaths, double riskFreeReturn, double maturity, double spot, double dividendYield, Params params) -> double
			{

				std::size_t timePoints{ static_cast<std::size_t>(maturity * 250) }; // one year has appr. 250 trading days
				DataTable paths{ SDE::monteCarloPaths(spot, maturity, numPaths, timePoints, riskFreeReturn - dividendYield, params) };

				double sampleAverage{ 0.0 };
				for (std::size_t num{ 0 }; num < numPaths; ++num)
				{
					auto subVec{ std::vector<double>(paths.m_table[num].rbegin(), paths.m_table[num].rbegin() + static_cast<int>(days)) }; // get last prices
					sampleAverage += np::mean(subVec);
				}
				return sampleAverage;
			}

			template <typename Params>
			auto call(std::size_t days, double riskFreeReturn, double maturity, double strike, double spot, double dividendYield, Params params) -> double
			{
				std::size_t numPaths{ 100000 };
				double sampleAverage{ arithmeticAverage(days, numPaths, riskFreeReturn, maturity, spot, dividendYield, params) };
				return std::max(sampleAverage / static_cast<double>(numPaths) - strike, 0.0);
			}

			template <typename Params>
			auto put(std::size_t days, double riskFreeReturn, double maturity, double strike, double spot, double dividendYield, Params params) -> double
			{
				std::size_t numPaths{ 100000 };
				double sampleAverage{ arithmeticAverage(days, numPaths, riskFreeReturn, maturity, spot, dividendYield, params) };
				return std::max(strike - sampleAverage / static_cast<double>(numPaths), 0.0);
			}
			
		}
	}

}




#endif
