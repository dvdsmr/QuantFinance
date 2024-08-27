#ifndef VOLATILITY_H
#define VOLATILITY_H
#include "xyvals.h"

namespace Volatility
{
	namespace Surface
	{
		auto call(const LabeledTable& priceSurface, double riskFreeReturn, double spot, double dividendYield) -> LabeledTable;
	}
}

#endif
