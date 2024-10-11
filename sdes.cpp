#include "Random.h"
#include "xyvals.h"
#include "sdes.h"
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
		auto BSM(std::complex<double> argument, double riskFreeReturn, double vol, double maturity, double spot, double dividendYield) -> std::complex<double>
		{
			return std::exp(IMNUM * (std::log(spot) + (riskFreeReturn - dividendYield - vol * vol / 2.0) * maturity) * argument
				- vol * vol * argument * argument * maturity / 2.0);
		}

		auto Heston(std::complex<double> argument, double riskFreeReturn, double initialVariance, double longVariance, double correlation, double reversionRate, double volVol, double maturity, double spot, double dividendYield) -> std::complex<double>
		{

			std::complex<double> tmp{ (reversionRate - IMNUM * correlation * volVol * argument) };
			std::complex<double> g{ std::sqrt((volVol * volVol) * (argument * argument + IMNUM * argument) + tmp * tmp) };
			double pow1{ 2 * reversionRate * longVariance / (volVol * volVol) };
			std::complex<double> numer1{ (reversionRate * longVariance * maturity * tmp) / (volVol * volVol) + IMNUM * argument * maturity * (riskFreeReturn - dividendYield) + IMNUM * argument * std::log(spot) };
			std::complex<double> log_denum1{ pow1 * std::log(std::cosh(g * maturity / 2.0) + (tmp / g) * std::sinh(g * maturity / 2.0)) };
			std::complex<double> tmp2{ ((argument * argument + IMNUM * argument) * initialVariance) / (g / std::tanh(g * maturity / 2.0) + tmp) };
			std::complex<double> log_phi{ numer1 - log_denum1 - tmp2 };
			std::complex<double> phi{ std::exp(log_phi) };
			return phi;
		}

		auto generalCF(std::complex<double> argument, const HestonParams& modelParams, const MarketParams& marketParams) -> std::complex<double>
		{

			return Heston(argument, marketParams.riskFreeReturn, modelParams.initialVariance, modelParams.longVariance, 
							modelParams.correlation, modelParams.reversionRate, modelParams.volVol, 
							marketParams.maturity, marketParams.spot, marketParams.dividendYield);
		}

		auto generalCF(std::complex<double> argument, const BSMParams& modelParams, const MarketParams& marketParams) -> std::complex<double>
		{
			return BSM(argument, marketParams.riskFreeReturn, modelParams.vol, marketParams.maturity, 
						marketParams.spot, marketParams.dividendYield);
		}


	}

	namespace Testing
	{
		auto saveHestonPaths() -> void
		{
			XYVals spath2{ SDE::HestonPath(100.0, 1.0, 1000, 0.09, 8., 15.,0.2,0.3,0.2) };
			Saving::write_xyvals_to_csv("Data/stockPath1.csv", spath2);
			XYVals spath3{ SDE::HestonPath(100.0, 1.0, 1000, 0.09, 8., 15.,0.2,0.3,0.2) };
			Saving::write_xyvals_to_csv("Data/stockPath2.csv", spath3);
			XYVals spath4{ SDE::HestonPath(100.0, 1.0, 1000, 0.09, 8., 15.,0.2,0.3,0.2) };
			Saving::write_xyvals_to_csv("Data/stockPath3.csv", spath4);
		}
	}
}