#ifndef OPTION_RETURNS_H
#define OPTION_RETURNS_H

namespace Options
{
	auto callIntrinsicValue(double strikePrice, double spotPrice) -> double;
	auto putIntrinsicValue(double strikePrice, double spotPrice) -> double;
	auto straddleIntrinsicValue(double strikePrice, double spotPrice) -> double;
	auto strangleIntrinsicValue(double strikeCall, double strikePut, double spotPrice) -> double;

	void strangleUnitTest();
}

#endif
