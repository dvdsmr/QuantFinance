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
	namespace OrnsteinUhlenbeck
	{
		auto simulate(double state, double time, double drift, double mean, double diffusion) -> double
		{
			double variance{ diffusion * diffusion * 0.5 / drift * (1.0 - std::exp(-2.0 * drift * time)) };
			return state * std::exp(-drift * time) + mean * (1.0 - std::exp(-drift * time)) + Random::normal(0.0,variance);
			//return state + stepSize * drift * state + std::sqrt(stepSize) * diffusion * Random::normal(0.0,1.0);
		}
	}

	namespace BSM
	{
		auto simulate(double initialState, double time, double drift, double volatility) -> double
		{
			return initialState * std::exp((drift - volatility * volatility / 2) * time + volatility * std::sqrt(time) * Random::normal(0.0, 1.0));
		}
		auto path(double initialState, double terminalTime, std::size_t timePoints, double drift, double volatility) -> XYVals
		{
			XYVals spath{ timePoints };
			spath.m_yVals[static_cast<std::size_t>(0)] = initialState;
			spath.m_xVals[static_cast<std::size_t>(0)] = 0.0;
			double time = terminalTime / (timePoints - 1);
			for (std::size_t i{ 1 }; i <= timePoints - 1; i++)
			{
				spath.m_xVals[i] = static_cast<double>(i) * time;
				spath.m_yVals[i] = simulate(spath.m_yVals[i - 1], time, drift, volatility);
			}
			return spath;
		}
		auto monteCarlo(double initialState, double terminalTime, std::size_t samples, double drift, double volatility) -> XYVals
		{
			XYVals mcSamples{ samples };
			for (std::size_t i{ 0 }; i < samples; i++)
			{
				mcSamples.m_xVals[i] = static_cast<double>(i);
				mcSamples.m_yVals[i] = simulate(initialState, terminalTime, drift, volatility);
			}
			return mcSamples;
		}
	}

	namespace Bachelier
	{
		auto simulate(double initialState, double time, double drift, double volatility) -> double
		{
			return initialState * std::exp(drift*time) + volatility*std::sqrt(1./2./drift*(std::exp(2*drift*time)-1.)) * Random::normal(0.0, 1.0);
		}


		auto path(double initialState, double terminalTime, std::size_t timePoints, double drift, double volatility) -> XYVals
		{
			XYVals spath{ timePoints };
			spath.m_yVals[static_cast<std::size_t>(0)] = initialState;
			spath.m_xVals[static_cast<std::size_t>(0)] = 0.0;
			double time = terminalTime / (timePoints - 1);
			for (std::size_t i{ 1 }; i <= timePoints - 1; i++)
			{
				spath.m_xVals[i] = static_cast<double>(i) * time;
				spath.m_yVals[i] = simulate(spath.m_yVals[i - 1], time, drift, volatility);
			}
			return spath;
		}

		auto monteCarlo(double initialState, double terminalTime, std::size_t samples, double drift, double volatility) -> XYVals
		{
			XYVals mcSamples{ samples };
			for (std::size_t i{ 0 }; i < samples; i++)
			{
				mcSamples.m_xVals[i] = static_cast<double>(i);
				mcSamples.m_yVals[i] = simulate(initialState, terminalTime, drift, volatility);
			}
			return mcSamples;
		}

	}

	namespace CEV
	{
		auto step(double state, double time, double drift, double diffusion, double exponent) -> double
		{
			return state + time * drift * state + std::sqrt(time) * diffusion * std::pow(state, exponent) * Random::normal(0.0, 1.0);
		}
	}

	namespace MertonJump
	{
		auto simulate(double initialState, double time, double drift, double volatility, double meanJumpSize, double stdJumpSize, double expectedJumpsPerYear) -> double
		{
			return initialState * std::exp(
				(drift - volatility*volatility / 2. - expectedJumpsPerYear*(meanJumpSize-stdJumpSize*stdJumpSize/2.)) * time 
					+ volatility * std::sqrt(time) * Random::normal(0.0, 1.0)
					+ Random::normal(meanJumpSize,stdJumpSize) * Random::poisson(expectedJumpsPerYear*time)
			);
		}
		auto path(double initialState, double terminalTime, std::size_t timePoints, double drift, double volatility, double meanJumpSize, double stdJumpSize, double expectedJumpsPerYear) -> XYVals
		{
			XYVals spath{ timePoints };
			spath.m_yVals[static_cast<std::size_t>(0)] = initialState;
			spath.m_xVals[static_cast<std::size_t>(0)] = 0.0;
			double time = terminalTime / (timePoints - 1);
			for (std::size_t i{ 1 }; i <= timePoints - 1; i++)
			{
				spath.m_xVals[i] = static_cast<double>(i) * time;
				spath.m_yVals[i] = simulate(spath.m_yVals[i - 1], time, drift, volatility, meanJumpSize, stdJumpSize, expectedJumpsPerYear);
			}
			return spath;
		}
		auto monteCarlo(double initialState, double terminalTime, std::size_t samples, double drift, double volatility, double meanJumpSize, double stdJumpSize, double expectedJumpsPerYear) -> XYVals
		{
			XYVals mcSamples{ samples };
			for (std::size_t i{ 0 }; i < samples; i++)
			{
				mcSamples.m_xVals[i] = static_cast<double>(i);
				mcSamples.m_yVals[i] = simulate(initialState, terminalTime, drift, volatility, meanJumpSize, stdJumpSize, expectedJumpsPerYear);
			}
			return mcSamples;
		}
	}

	namespace Heston
	{
		// TODO: This can be made exact like the price step, since this is an OU process
		auto varianceStep(double initialVariance, double stepSize, double longVariance, double correlatedNormal, double reversionRate, double volVol) -> double
		{
			return std::abs(initialVariance + stepSize * reversionRate * (longVariance - initialVariance) * stepSize + std::sqrt(stepSize * initialVariance) * volVol * correlatedNormal);
		}

		auto priceStep(double initialState, double stepSize, double drift, double variance, double correlatedNormal) -> double
		{
			// return initialState + stepSize * drift * initialState + std::sqrt(stepSize * variance) * correlatedNormal;
			return initialState * std::exp((drift - variance / 2) * stepSize + std::sqrt(variance) * std::sqrt(stepSize) * correlatedNormal);
		}
		// careful: the step is only accurate for very small time steps
		auto path(double initialState, double terminalTime, std::size_t timePoints, double drift, double initialVariance, double longVariance, double correlation, double reversionRate, double volVol) -> XYVals
		{
			// assert the Feller condition, which ensures that the process is positive
			if (2 * reversionRate * longVariance <= volVol * volVol)
			{
				std::cout << "Warning: Feller condition of Heston model not satisfied, variance can become zero.\n";
			}
			// assert(2 * reversionRate * longVariance > volVol * volVol);

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
				spath.m_yVals[i] = priceStep(spath.m_yVals[i-1], time, drift, var, increment1);

				// make step with variance process for next step
				var = varianceStep(var, time, longVariance, increment2, reversionRate, volVol);
			}
			return spath;
		}

		auto monteCarlo(double initialState, double terminalTime, std::size_t samples, std::size_t timePoints, double drift, double initialVariance, double longVariance, double correlation, double reversionRate, double volVol) -> XYVals
		{
			XYVals mcSamples{ samples };

			// initialize samples with initial state
			mcSamples.m_yVals = std::vector<double>(samples, initialState);

			for (std::size_t i{ 0 }; i < samples; i++)
			{
				mcSamples.m_xVals[i] = static_cast<double>(i);
				XYVals spath{ path(initialState, terminalTime, timePoints, drift, initialVariance, longVariance, correlation, reversionRate, volVol) };
				mcSamples.m_yVals[i] = spath.m_yVals.back();
			}
			return mcSamples;
		}
	}

	namespace VarianceGamma
	{
		auto step(double initialState, double stepSize, double drift, double variance, double vol) -> double
		{
			double gammaIncrement{ Random::gamma(stepSize / variance, variance) };
			double normalIncrement{ Random::normal(0.,1.) };
			double VGIncrement{ drift * gammaIncrement + vol * std::sqrt(gammaIncrement) * normalIncrement };
			return initialState * std::exp(VGIncrement);
		}
		auto path(double initialState, double terminalTime, std::size_t timePoints, double drift, double variance, double vol) -> XYVals
		{

			XYVals spath{ timePoints };
			spath.m_yVals[static_cast<std::size_t>(0)] = initialState;
			spath.m_xVals[static_cast<std::size_t>(0)] = 0.0;
			double time = terminalTime / (timePoints - 1);
			for (std::size_t i{ 1 }; i <= timePoints - 1; i++)
			{
				// update values
				spath.m_xVals[i] = static_cast<double>(i) * time;
				spath.m_yVals[i] = step(spath.m_yVals[i - 1], time, drift, variance, vol);
			}
			return spath;
		}
		auto monteCarlo(double initialState, double terminalTime, std::size_t samples, std::size_t timePoints, double drift, double variance, double vol) -> XYVals
		{
			XYVals mcSamples{ samples };

			// initialize samples with initial state
			mcSamples.m_yVals = std::vector<double>(samples, initialState);

			for (std::size_t i{ 0 }; i < samples; i++)
			{
				mcSamples.m_xVals[i] = static_cast<double>(i);
				XYVals spath{ path(initialState, terminalTime, timePoints, drift, variance, vol) };
				mcSamples.m_yVals[i] = spath.m_yVals.back();
			}
			return mcSamples;
		}

	}








	// NOTE: the argument in the CFs is the log of the stock price (commonly denoted as log S_t)
	namespace CharacteristicFunctions
	{
		auto BSM(std::complex<double> argument, double riskFreeReturn, double vol, double maturity, double spot, double dividendYield) -> std::complex<double>
		{
			return std::exp(IMNUM * (std::log(spot) + (riskFreeReturn - dividendYield - vol * vol / 2.0) * maturity) * argument
				- vol * vol * argument * argument * maturity / 2.0);
		}

		auto MertonJump(std::complex<double> argument, double riskFreeReturn, double vol, double maturity, double spot, double dividendYield, double meanJumpSize, double stdJumpSize, double expectedJumpsPerYear) -> std::complex<double>
		{
			double omega{riskFreeReturn-dividendYield-vol*vol/2. - expectedJumpsPerYear*(std::exp(meanJumpSize+stdJumpSize/2.)-1.)};
			std::complex<double> term1{ IMNUM*argument*std::log(spot) + IMNUM*argument*omega*maturity - 0.5*argument*argument*vol*vol*maturity};
			std::complex<double> term2{expectedJumpsPerYear*maturity*(std::exp(IMNUM*argument*meanJumpSize-argument*argument*stdJumpSize*stdJumpSize/2.)-1.)};
			return std::exp(term1 + term2);
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

		auto VarianceGamma(std::complex<double> argument, double riskFreeReturn, double maturity, double spot, double dividendYield, double volatility, double drift, double variance) -> std::complex<double>
		{
			double omega{ std::log(1-drift*variance - volatility*volatility*variance*0.5) / variance};
			std::complex<double> factor1{ std::exp(IMNUM * argument * (std::log(spot)+(riskFreeReturn-dividendYield+omega)*maturity)) };
			std::complex<double> factor2{ std::pow(1.0-IMNUM*argument*drift*variance + volatility*volatility*argument*argument*variance*0.5,-maturity/variance) };
			return factor1 * factor2;

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
		auto generalCF(std::complex<double> argument, const MertonJumpParams& modelParams, const MarketParams& marketParams) -> std::complex<double>
		{
			return MertonJump(argument, marketParams.riskFreeReturn, modelParams.vol, marketParams.maturity,
				marketParams.spot, marketParams.dividendYield, modelParams.meanJumpSize, modelParams.stdJumpSize, modelParams.expectedJumpsPerYear);
		}
		auto generalCF(std::complex<double> argument, const VarianceGammaParams& modelParams, const MarketParams& marketParams) -> std::complex<double>
		{
			return VarianceGamma(argument, marketParams.riskFreeReturn, marketParams.maturity,
				marketParams.spot, marketParams.dividendYield, modelParams.vol, modelParams.drift, modelParams.variance);
		}


	}

	namespace Testing
	{

		auto saveMCsamples() -> void
		{
			double initialState{ 100. };
			double terminalTime{ 0.2 };
			std::size_t samples{ 50000 };
			double drift{ 0.05 };
			double volatility{ 0.4 };
			XYVals mcSamplesBSM{ BSM::monteCarlo(initialState, terminalTime, samples, drift, volatility) };

			double meanJumpSize{ 0.0 };
			double stdJumpSize{ 0.2 };
			double expectedJumpsPerYear{ 2. };
			XYVals mcSamplesMertonJump{ MertonJump::monteCarlo(initialState, terminalTime, samples, drift, volatility, meanJumpSize, stdJumpSize, expectedJumpsPerYear) };

			double bachelierVol{ volatility * initialState }; // otherwise Bachelier has comparably tiny vol
			XYVals mcSamplesBachelier{ Bachelier::monteCarlo(initialState, terminalTime, samples, drift, bachelierVol) };

			std::size_t timePoints{ 1000 };
			double initialVariance{ volatility*volatility };
			double longVariance{ volatility*volatility };
			double hestonDrift{ drift };
			double correlation{ -0.9 };
			double reversionRate{ 1.5 };
			double volVol{ 0.6 };
			XYVals mcSamplesHeston{ Heston::monteCarlo(initialState, terminalTime, samples, timePoints, hestonDrift, initialVariance, longVariance, correlation, reversionRate, volVol) };

			double variance{ 0.15 };
			XYVals mcSamplesVarianceGamma{ VarianceGamma::monteCarlo(initialState, terminalTime, samples, timePoints, drift, variance, volatility) };

			Saving::write_xyvals_to_csv("Data/mcSamplesBSM.csv", mcSamplesBSM);
			Saving::write_xyvals_to_csv("Data/mcSamplesMertonJump.csv", mcSamplesMertonJump);
			Saving::write_xyvals_to_csv("Data/mcSamplesBachelier.csv", mcSamplesBachelier);
			Saving::write_xyvals_to_csv("Data/mcSamplesHeston.csv", mcSamplesHeston);
			Saving::write_xyvals_to_csv("Data/mcSamplesVarianceGamma.csv", mcSamplesVarianceGamma);
		}

		auto saveHestonPaths() -> void
		{
			
			XYVals spath2{ Heston::path(100.0, 1.0, 1000, 0.09, 8., 15.,0.2,0.3,0.2) };
			Saving::write_xyvals_to_csv("Data/HstockPath1.csv", spath2);
			XYVals spath3{ Heston::path(100.0, 1.0, 1000, 0.09, 8., 15.,0.2,0.3,0.2) };
			Saving::write_xyvals_to_csv("Data/HstockPath2.csv", spath3);
			XYVals spath4{ Heston::path(100.0, 1.0, 1000, 0.09, 8., 15.,0.2,0.3,0.2) };
			Saving::write_xyvals_to_csv("Data/HstockPath3.csv", spath4);
		

			//HestonPath(initialState,terminalTime,timePoints,drift,initialVariance,longVariance,correlation,reversionRate,volVol)

			/*
			XYVals spath4{ SDE::HestonPath(100.0, 0.2, 1000, 0.045-0.007, 0.62, 0.62,-0.15,0.2,6.64) };
			Saving::write_xyvals_to_csv("Data/stockPath4.csv", spath4);
			XYVals spath5{ SDE::HestonPath(100.0, 0.2, 1000, 0.045 - 0.007, 0.62, 0.62,-0.15,0.2,6.64) };
			Saving::write_xyvals_to_csv("Data/stockPath5.csv", spath5);
			XYVals spath6{ SDE::HestonPath(100.0, 0.2, 1000, 0.045 - 0.007, 0.62, 0.62,-0.15,0.2,6.64) };
			Saving::write_xyvals_to_csv("Data/stockPath6.csv", spath6);

			XYVals spath7{ geometricBrownianMotionPath(100.0, 0.2, 1000, 0.045-0.007, 0.62) };
			Saving::write_xyvals_to_csv("Data/stockPath7.csv", spath7);
			XYVals spath8{ geometricBrownianMotionPath(100.0, 0.2, 1000, 0.045 - 0.007, 0.62) };
			Saving::write_xyvals_to_csv("Data/stockPath8.csv", spath8);
			XYVals spath9{ geometricBrownianMotionPath(100.0, 0.2, 1000, 0.045 - 0.007, 0.62) };
			Saving::write_xyvals_to_csv("Data/stockPath9.csv", spath9);
			*/

		}

		auto saveVarianceGammaPaths() -> void
		{

			XYVals spath2{ VarianceGamma::path(100.0, 0.2, 1000, 0.05, 0.002, 0.4) };
			Saving::write_xyvals_to_csv("Data/VGstockPath1.csv", spath2);
			XYVals spath3{ VarianceGamma::path(100.0, 0.2, 1000, 0.05, 0.002, 0.4) };
			Saving::write_xyvals_to_csv("Data/VGstockPath2.csv", spath3);
			XYVals spath4{ VarianceGamma::path(100.0, 0.2, 1000, 0.05, 0.002, 0.4) };
			Saving::write_xyvals_to_csv("Data/VGstockPath3.csv", spath4);
		}

		auto saveMertonJumpPaths() -> void
		{

			XYVals spath2{ MertonJump::path(100.0, .5, 1000, 0.05, 0.3,0.0,0.3,2.) };
			Saving::write_xyvals_to_csv("Data/MJstockPath1.csv", spath2);
			XYVals spath3{ MertonJump::path(100.0, .5, 1000, 0.05, 0.3,0.0,0.3,2.) };
			Saving::write_xyvals_to_csv("Data/MJstockPath2.csv", spath3);
			XYVals spath4{ MertonJump::path(100.0, .5, 1000, 0.05, 0.3,0.0,0.3,2.) };
			Saving::write_xyvals_to_csv("Data/MJstockPath3.csv", spath4);
		}

	}
}