#ifndef INTEREST_MODELS
#define INTETEST_MODELS
#include <functional>

namespace ShortRateModels
{
	namespace HullWhite
	{
		auto step(double time, double state, double timeStep, const std::function<double(double)>& drift, double meanReversion, double vol) -> double;
		auto simulate(double time, double state, double constDrift, double meanReversion, double vol) -> double;
		auto simulate(double time, double state, const std::function<double(double)>& drift, double meanReversion, double vol) -> double;
	}

	namespace Utils
	{
		auto integrateDriftTrapezoidal(double time, double meanReversion, const std::function<double(double)>& drift) -> double;
	}

	namespace Testing
	{
		void hullWhite();
	}
}

#endif
