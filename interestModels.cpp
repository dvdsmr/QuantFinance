#include "interestModels.h"
#include "Random.h"
#include "xyvals.h"
#include "saving.h"
#include <cassert>
#include <iostream>

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
		auto simulate(double startTime, double endTime, double state, const std::function<double(double)>& drift, double meanReversion, double vol) -> double
		{
			assert(endTime > startTime);

			double rateMean{ std::exp(-meanReversion * (endTime-startTime)) * state + Utils::integrateDriftTrapezoidal(startTime, endTime, meanReversion, drift)};
			double rateVar{ vol * vol / 2. / meanReversion * (1 - std::exp(-2 * meanReversion * (endTime-startTime))) };
			return Random::normal(rateMean, rateVar);
		}
		auto path(double initialState, double terminalTime, std::size_t timePoints, const std::function<double(double)>& drift, double meanReversion, double vol) -> XYVals
		{
			XYVals spath{ timePoints };
			spath.m_yVals[static_cast<std::size_t>(0)] = initialState;
			spath.m_xVals[static_cast<std::size_t>(0)] = 0.0;
			double time{ terminalTime / (timePoints - 1) };
			for (std::size_t i{ 1 }; i <= timePoints - 1; i++)
			{
				spath.m_xVals[i] = static_cast<double>(i) * time;
				spath.m_yVals[i] = simulate(static_cast<double>(i-1) * time, static_cast<double>(i) * time, spath.m_yVals[i - 1], drift, meanReversion, vol);
			}
			return spath;
		}

		auto drift(const std::function<double(double)>& instaForwardRate, std::function<double(double)> instaForwardRateDeriv, double meanReversion, double vol) -> std::function<double(double)>
		{
			auto driftFunc{ [=](double time) -> double { return instaForwardRateDeriv(time)
																+ meanReversion*instaForwardRate(time)
																+ vol*vol/2/meanReversion*(1-std::exp(-2*meanReversion*time)); } };

			return driftFunc;
		}
	}

	namespace Utils
	{
		auto integrateDriftTrapezoidal(double startTime, double endTime, double meanReversion, const std::function<double(double)>& drift) -> double
		{
			assert(endTime > startTime);

			std::size_t numPoints{ 100 };
			double tDelta{ (endTime-startTime) / static_cast<double>(numPoints) };
			double integral{ 0.5 * tDelta * std::exp(meanReversion * (startTime - endTime)) * drift(startTime) };
			double currentTime{ startTime + tDelta };
			for (std::size_t i{ 1 }; i < numPoints; ++i)
			{
				integral += tDelta * std::exp(meanReversion * (currentTime - endTime)) * drift(currentTime);
				currentTime += tDelta;
			}
			integral += 0.5 * tDelta * std::exp(meanReversion * (currentTime - endTime)) * drift(currentTime);
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
			double vol{ 0.6 };
			double time{ 1. };
			double state{ 0.05 };
			[[maybe_unused]]double shortRate{ HullWhite::simulate(time, state, constDrift, meanReversion, vol) };
		
			// with function drift
			auto drift{ [&](double time) -> double { return 0.3*time; } };
			[[maybe_unused]]double newShortRate{ HullWhite::simulate(0., time, state, drift, meanReversion, vol) };


			XYVals shortPath{ ShortRateModels::HullWhite::path(state, time, 100, drift, meanReversion, vol) };
			Saving::write_xyvals_to_csv("Data/hullWhitePath.csv", shortPath);


		}

		void getDrift()
		{
			double meanReversion{ 0.4 };
			double vol{ 0.6 };

			// test the continuous drift function as dependent on the instantaneous forward rate
			auto instantaneousForwardRate{ [&](double time) -> double { return 0.05 + std::sqrt(time + 1); } };
			auto instantaneousForwardRateDeriv{ [&]([[maybe_unused]] double time) -> double { return 0.5 / std::sqrt(time + 1); } };

			auto newDrift = ShortRateModels::HullWhite::drift(instantaneousForwardRate, instantaneousForwardRateDeriv, meanReversion, vol);

			for (int i{ 0 }; i <= 10; ++i)
			{
				std::cout << " " << newDrift(static_cast<double>(i));
			}
		}
	}

}