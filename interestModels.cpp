#include "interestModels.h"
#include "Random.h"

namespace ShortRateModels
{
	namespace HullWhite
	{
		auto step(double time, double state, double timeStep, const std::function<double(double)>& drift, double meanReversion, double vol) -> double
		{
			return state + (drift(time) - meanReversion*state)*timeStep + vol*std::sqrt(timeStep) * Random::normal(0.0, 1.0);
		}
	}
}