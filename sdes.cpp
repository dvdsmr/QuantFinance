#include "Random.h"
#include "stockpaths.h"
#include <cmath>
#include <vector>


namespace SDE
{
	// careful: the OU step is only accurate for very small time steps
	auto OrnsteinUhlenbeckStep(double state, double stepSize, double drift, double diffusion) -> double
	{
		return state + stepSize * drift * state + std::sqrt(stepSize) * diffusion * Random::normal(0.0,1.0);
	}
	auto OrnsteinUhlenbeckSimulate(double state, double stepSize, double drift, double diffusion, double time) -> double
	{
		double terminalState = state;
		double currentTime = 0.0;
		while (currentTime <= time-stepSize)
		{
			terminalState = OrnsteinUhlenbeckStep(terminalState,stepSize,drift,diffusion);
			currentTime += stepSize;
		}
		terminalState = OrnsteinUhlenbeckStep(terminalState, time-currentTime, drift, diffusion);
		return terminalState;
	}
	auto geometricBrownianMotion(double initialState, double time, double drift, double volatility) -> double
	{
		return initialState * std::exp((drift - std::pow(volatility, 2) / 2) * time + volatility * std::sqrt(time) * Random::normal(0.0, 1.0));
	}

	auto geometricBrownianMotionPath(double initialState, double terminalTime, std::size_t timePoints, double drift, double volatility) -> StockPath
	{
		StockPath spath{ timePoints };
		spath.m_stockVals[static_cast<std::size_t>(0)] = initialState;
		spath.m_timeVals[static_cast<std::size_t>(0)] = 0.0;
		double time = terminalTime / (timePoints - 1);
		for (std::size_t i{ 1 }; i <= timePoints - 1; i++)
		{
			spath.m_timeVals[i] = static_cast<double>(i) * time;
			spath.m_stockVals[i] = spath.m_stockVals[i - 1] * std::exp((drift - std::pow(volatility, 2) / 2) * time + volatility * std::sqrt(time) * Random::normal(0.0, 1.0));
		}
		return spath;
	}

	auto HestonVarianceStep(double initialVariance, double stepSize, double longVariance, double correlatedNormal, double reversionRate, double volVol) -> double
	{
		return initialVariance + stepSize * reversionRate * (longVariance - initialVariance) * stepSize + std::sqrt(stepSize * initialVariance) * volVol * correlatedNormal;
	}

	auto HestonPriceStep(double initialState, double stepSize, double drift, double variance, double correlatedNormal) -> double
	{
		return initialState + stepSize * drift * initialState + std::sqrt(stepSize) * variance * correlatedNormal;
	}

	// careful: the step is only accurate for very small time steps
	auto HestonPath(double initialState, double terminalTime, std::size_t timePoints, double drift, double initialVariance, double longVariance, double correlation, double reversionRate, double volVol) -> StockPath
	{
		StockPath spath{ timePoints };
		spath.m_stockVals[static_cast<std::size_t>(0)] = initialState;
		spath.m_timeVals[static_cast<std::size_t>(0)] = 0.0;
		double time = terminalTime / (timePoints - 1);
		for (std::size_t i{ 1 }; i <= timePoints - 1; i++)
		{
			spath.m_timeVals[i] = static_cast<double>(i) * time;

			// generate correlated standard normals
			double normal1{ Random::normal(0.0,1.0) };
			double normal2{ Random::normal(0.0,1.0) };
			double increment1{ std::sqrt((1 + correlation) / 2.0) * normal1 + std::sqrt((1 - correlation) / 2.0) * normal2 };
			double increment2{ std::sqrt((1 + correlation) / 2.0) * normal1 - std::sqrt((1 - correlation) / 2.0) * normal2 };

			// update values
			spath.m_stockVals[i] = HestonPriceStep(initialState, time, drift, initialVariance, increment1);

			// make step with variance process for next step
			initialVariance = HestonVarianceStep(initialVariance, time, longVariance, increment2, reversionRate, volVol);
		}
		return spath;

	}


}