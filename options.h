#ifndef OPTION_RETURNS_H
#define OPTION_RETURNS_H

#include "securities.h"

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
		}
	}
	void strangleUnitTest();
	void callCreditSpreadUnitTest();
	void binomialPricingUnitTest();
}

// Template for option class
// todo: make payoffs etc friend functions
// todo: - rename payoff to type and consider only basic payoffs
//       - complex options shoukd be defined as linear combinations of basic options
class Option
{
public:
	enum Payoff
	{
		call,
		put,
		straddle,
		strangle,
		putDebitSpread,
		putCreditSpread,
		callDebitSpread,
		callCreditSpread,
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
	Payoff m_type{ call };
	Position m_position{ longPosition };
	SimpleStock m_underlying{ SimpleStock() };
};

#endif
