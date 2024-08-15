#include "Random.h"
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
	auto geometricBrownianMotionPath(double initialState, double terminalTime, int timePoints, double drift, double volatility) -> std::vector<double>
	{
		std::vector<double> path(static_cast<std::size_t>(timePoints));
		path[static_cast<std::size_t>(0)] = initialState;
		double time = terminalTime / (timePoints - 1);
		for (int i{1}; i <= timePoints - 1; i++)
		{
			path[static_cast<std::size_t>(i)] = path[static_cast<std::size_t>(i-1)] * std::exp((drift - std::pow(volatility, 2) / 2) * time + volatility * std::sqrt(time) * Random::normal(0.0, 1.0));
		}
		return path;
	}
}