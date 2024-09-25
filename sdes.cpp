#include "Random.h"
#include "xyvals.h"
#include <cmath>
#include <vector>
#include <cassert>
#include <complex>


constexpr std::complex<double> IMNUM(0.0, 1.0);



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

	auto geometricBrownianMotionPath(double initialState, double terminalTime, std::size_t timePoints, double drift, double volatility) -> XYVals
	{
		XYVals spath{ timePoints };
		spath.m_yVals[static_cast<std::size_t>(0)] = initialState;
		spath.m_xVals[static_cast<std::size_t>(0)] = 0.0;
		double time = terminalTime / (timePoints - 1);
		for (std::size_t i{ 1 }; i <= timePoints - 1; i++)
		{
			spath.m_xVals[i] = static_cast<double>(i) * time;
			spath.m_yVals[i] = spath.m_yVals[i - 1] * std::exp((drift - std::pow(volatility, 2) / 2) * time + volatility * std::sqrt(time) * Random::normal(0.0, 1.0));
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
	auto HestonPath(double initialState, double terminalTime, std::size_t timePoints, double drift, double initialVariance, double longVariance, double correlation, double reversionRate, double volVol) -> XYVals
	{
		// assert the Feller condition, which ensures that the process is positive
		assert(2 * reversionRate * longVariance > volVol * volVol);

		XYVals spath{ timePoints };
		spath.m_yVals[static_cast<std::size_t>(0)] = initialState;
		spath.m_xVals[static_cast<std::size_t>(0)] = 0.0;
		double time = terminalTime / (timePoints - 1);
		double var {initialVariance};
		for (std::size_t i{ 1 }; i <= timePoints - 1; i++)
		{
			spath.m_xVals[i] = static_cast<double>(i) * time;

			// generate correlated standard normals
			double normal1{ Random::normal(0.0,1.0) };
			double normal2{ Random::normal(0.0,1.0) };
			double increment1{ std::sqrt((1 + correlation) / 2.0) * normal1 + std::sqrt((1 - correlation) / 2.0) * normal2 };
			double increment2{ std::sqrt((1 + correlation) / 2.0) * normal1 - std::sqrt((1 - correlation) / 2.0) * normal2 };

			// update values
			spath.m_yVals[i] = HestonPriceStep(spath.m_yVals[i-1], time, drift, var, increment1);

			// make step with variance process for next step
			var = HestonVarianceStep(var, time, longVariance, increment2, reversionRate, volVol);
		}
		return spath;

	}


	namespace CharacteristicFunctions
	{
		auto BSM(double argument, double riskFreeReturn, double vol, double maturity, double spot, double dividendYield) -> std::complex<double>
		{
			return std::exp(IMNUM * (std::log(spot) + (riskFreeReturn - dividendYield - vol * vol / 2.0) * maturity) * argument
				- vol * vol * argument * argument * maturity / 2.0);
		}
	}

}