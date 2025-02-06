#ifndef INTEREST_MODELS
#define INTETEST_MODELS
#include <functional>
#include "xyvals.h"

namespace ShortRateModels
{
	namespace HullWhite
	{
		auto step(double time, double state, double timeStep, const std::function<double(double)>& drift, double meanReversion, double vol) -> double;
		auto simulate(double time, double state, double constDrift, double meanReversion, double vol) -> double;
		auto simulate(double startTime, double endTime, double state, const std::function<double(double)>& drift, double meanReversion, double vol) -> double;
		auto path(double initialState, double terminalTime, std::size_t timePoints, const std::function<double(double)>& drift, double meanReversion, double vol) -> XYVals;
		auto drift(const std::function<double(double)>& instaForwardRate, std::function<double(double)> instaForwardRateDeriv, double meanReversion, double vol) -> std::function<double(double)>;
	}

	namespace Utils
	{
		auto integrateDriftTrapezoidal(double startTime, double endTime, double meanReversion, const std::function<double(double)>& drift) -> double;
	}

	namespace Testing
	{
		void hullWhite();
	}
}

#endif
