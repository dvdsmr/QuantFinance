#include "interestModels.h"

namespace ShortRateModels
{
	namespace HullWhite
	{
		auto step(const std::function<double(double)>& drift, double time, double meanReversion, double vol) -> double
		{
			// TODO
			return drift(time) + meanReversion + vol;
		}
	}
}