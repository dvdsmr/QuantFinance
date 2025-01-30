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
		auto simulate(double time, double state, const std::function<double(double)>& drift, double meanReversion, double vol) -> double
		{
			double rateMean{ std::exp(-meanReversion * time) * state + Utils::integrateDriftTrapezoidal(time, meanReversion, drift)};
			double rateVar{ vol * vol / 2. / meanReversion * (1 - std::exp(-2 * meanReversion * time)) };
			return Random::normal(rateMean, rateVar);
		}
	}

	namespace Utils
	{
		auto integrateDriftTrapezoidal(double time, double meanReversion, const std::function<double(double)>& drift) -> double
		{
			std::size_t numPoints{ 100 };
			double tDelta{ time / static_cast<double>(numPoints) };
			double integral{ 0.5 * tDelta * std::exp(meanReversion * (0. - time)) * drift(0.) };
			double currentTime{ tDelta };
			for (std::size_t i{ 1 }; i < numPoints; ++i)
			{
				integral += tDelta * std::exp(meanReversion * (currentTime - time)) * drift(currentTime);
				currentTime += tDelta;
			}
			integral += 0.5 * tDelta * std::exp(meanReversion * (currentTime - time)) * drift(currentTime);
			return integral;
		}
	}


	namespace Testing
	{
		void hullWhite()
		{
			// simulate with constant drift
			double constDrift{ 0.2 };
			double meanReversion{ 0.4 };
			double vol{ 0.3 };
			double time{ 1. };
			double state{ 0.05 };
			[[maybe_unused]]double shortRate{ HullWhite::simulate(time, state, constDrift, meanReversion, vol) };
		
			// with function drift
			auto drift{ [&](double time) -> double { return time; } };
			[[maybe_unused]]double newShortRate{ HullWhite::simulate(time, state, drift, meanReversion, vol) };
		}
	}

}