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
		auto simulate(double time, double state, double constDrift, double meanReversion, double vol) -> double
		{
			double rateMean{ std::exp(-meanReversion * time) * state + constDrift / meanReversion * (1 - std::exp(-meanReversion * time)) };
			double rateVar{ vol * vol / 2. / meanReversion * (1 - std::exp(-2 * meanReversion * time)) };
			return Random::normal(rateMean,rateVar);
		}
	}
}