#ifndef VOLATILITY_H
#define VOLATILITY_H

#include "xyvals.h"
#include "saving.h"
#include "reading.h"
#include "pso.h"
#include <string_view>

namespace Volatility
{
	namespace Surface
	{
		auto bsm(const LabeledTable& priceSurface, double riskFreeReturn, double spot, double dividendYield, std::string_view type = "call", std::string_view optimizer = "adam") -> LabeledTable;
		auto testCalibration() -> LabeledTable;
		auto sanityCheck() -> void;
		auto calibrateToRealData() -> void;
	}
}

#endif
