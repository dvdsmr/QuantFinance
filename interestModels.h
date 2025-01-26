#ifndef INTEREST_MODELS
#define INTETEST_MODELS
#include <functional>

namespace ShortRateModels
{
	namespace HullWhite
	{
		auto step(const std::function<double(double)>& drift, double time, double meanReversion, double vol) -> double;
	}
}

#endif
