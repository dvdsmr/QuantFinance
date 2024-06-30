#ifndef OPTION_RETURNS_H
#define OPTION_RETURNS_H

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
			auto call(double riskFreeRate, double upTick, double strike, double spot) -> double;
			auto put(double riskFreeRate, double upTick, double strike, double spot) -> double;
		}
	}
	void strangleUnitTest();
	void callCreditSpreadUnitTest();
	void binomialPricingUnitTest();
}

#endif
