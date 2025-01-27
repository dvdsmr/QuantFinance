#ifndef INTEREST_MODELS
#define INTETEST_MODELS
#include <functional>

namespace ShortRateModels
{
	namespace HullWhite
	{
		auto step(double time, double state, double timeStep, const std::function<double(double)>& drift, double meanReversion, double vol) -> double;
	}
}

#endif
