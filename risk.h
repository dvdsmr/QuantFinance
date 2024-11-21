#ifndef RISK_H
#define RISK_H

#include "xyvals.h"
#include <cassert>
#include <algorithm>

namespace Risk
{
	auto sampleCVAR(XYVals samples, double level, double currentValue) -> double;
}

#endif