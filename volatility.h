#ifndef VOLATILITY_H
#define VOLATILITY_H
#include "xyvals.h"
#include <string_view>

namespace Volatility
{
	namespace Surface
	{
		auto call(const LabeledTable& priceSurface, double riskFreeReturn, double spot, double dividendYield, std::string_view optimizer = "adam") -> LabeledTable;
		LabeledTable testCalibration();
	}
}

#endif
