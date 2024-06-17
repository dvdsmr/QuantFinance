#ifndef OPTION_RETURNS_H
#define OPTION_RETURNS_H

namespace Options
{
	auto callPayoff(double strikePrice, double spotPrice) -> double;
	auto putPayoff(double strikePrice, double spotPrice) -> double;
	auto straddlePayoff(double strikePrice, double spotPrice) -> double;
	auto stranglePayoff(double strikeCall, double strikePut, double spotPrice) -> double;
	auto putDebitSpreadPayoff(double lower, double higher, double spotPrice) -> double;
	auto putCreditSpreadPayoff(double lower, double higher, double spotPrice) -> double;
	auto callDebitSpreadPayoff(double lower, double higher, double spotPrice) -> double;
	auto callCreditSpreadPayoff(double lower, double higher, double spotPrice) -> double;
	void strangleUnitTest();
	void callCreditSpreadUnitTest();
}

#endif
