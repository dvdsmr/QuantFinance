#ifndef OPTION_RETURNS_H
#define OPTION_RETURNS_H

#include "securities.h"

namespace Options
{
	enum class Payoff
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

	enum class Position
	{
		longPosition,
		shortPosition,
	};

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
		}
	}
	void strangleUnitTest();
	void callCreditSpreadUnitTest();
	void binomialPricingUnitTest();
}

// Template for option class

class Option
{
public:
	explicit Option() = default;

private:
	double strikePrice{ 0. };
	double maturity{ 10. };
	Options::Payoff m_payoff{ Options::Payoff::call };
	Options::Position m_position{ Options::Position::longPosition };
	SimpleStock m_underlying{ SimpleStock() };
};

#endif
