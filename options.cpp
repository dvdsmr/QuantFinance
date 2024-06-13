#include "options.h"
#include <algorithm>


namespace Options
{
	double callIntrinsicValue(double strikePrice, double spotPrice)
	{
		return std::max(spotPrice - strikePrice, 0.);
	}

	double putIntrinsicValue(double strikePrice, double spotPrice)
	{
		return std::max(strikePrice - spotPrice, 0.);
	}

	double straddleIntrinsicValue(double strikePrice, double spotPrice)
	{
		return callIntrinsicValue(strikePrice, spotPrice) + putIntrinsicValue(strikePrice, spotPrice);
	}

}
