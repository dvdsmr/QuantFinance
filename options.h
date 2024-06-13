#ifndef OPTION_RETURNS_H
#define OPTION_RETURNS_H

namespace Options
{
	double callIntrinsicValue(double strikePrice, double spotPrice);
	double putIntrinsicValue(double strikePrice, double spotPrice);
}

#endif
