#include "Random.h"
#include <cmath>


namespace SDE
{
	// careful: the OU step is only accurate for very small time steps
	auto OrnsteinUhlenbeckStep(double state, double stepSize, double drift, double diffusion) -> double
	{
		return state + stepSize * drift * state + std::sqrt(stepSize) * diffusion * Random::normal(0.0,1.0);
	}
	auto geometricBrownianMotion(double initialState, double time, double drift, double volatility) -> double
	{
		return initialState * std::exp((drift - std::pow(volatility, 2) / 2) * time + volatility * std::sqrt(time) * Random::normal(0.0, 1.0));
	}
}