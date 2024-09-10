#ifndef OPTION_RETURNS_H
#define OPTION_RETURNS_H

#include "securities.h"
#include "xyvals.h"

namespace Options
{
	namespace Payoffs
	{
		auto call(double strikePrice, double spotPrice) -> double;
		auto put(double strikePrice, double spotPrice) -> double;
		auto straddle(double strikePrice, double spotPrice) -> double;
		auto strangle(double strikeCall, double strikePut, double spotPrice) -> double;
		auto putDebitSpread(double lower, double higher, double spotPrice) -> double;
		auto putCreditSpread(double lower, double higher, double spotPrice) -> double;
		auto callDebitSpread(double lower, double higher, double spotPrice) -> double;
		auto callCreditSpread(double lower, double higher, double spotPrice) -> double;
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


			namespace DataGeneration
			{
				auto call(double riskFreeReturn, double vol, double maturity, double strike, double dividendYield) -> DataTable;
				auto callPriceSurface(double riskFreeReturn, double vol, double spot, double dividendYield) -> LabeledTable;
			}
		}
	}
	void strangleUnitTest();
	void callCreditSpreadUnitTest();
	void binomialPricingUnitTest();
}

// Template for option class
// todo: make payoffs etc friend functions
// todo: - complex options should be defined as linear combinations of basic options
class Option
{
public:
	enum PayoffType
	{
		call,
		put,
	};

	enum ExerciseType
	{
		european,
		american,
		asian,
	};

	enum Position
	{
		longPosition,
		shortPosition,
	};
	explicit constexpr Option() = default;
	double samplePayoff(double time = 0.);

private:
	double m_strike{ 100. };
	double m_maturity{ 10. };
	PayoffType m_type{ call };
	ExerciseType m_etype{ european };
	Position m_position{ longPosition };
	SimpleStock m_underlying{ SimpleStock() };
};

#endif
