#include "risk.h"

namespace Risk
{
	// needs testing
	auto sampleVAR(XYVals samples, double level, double currentValue) -> double
	{
		// compute the value at risk based on the given samples
		assert(samples.m_length > 100); // ensure we have a sufficient number of samples
	
		// sort the samples so that we can easily get the cVar
		std::sort(std::begin(samples.m_yVals), std::end(samples.m_yVals));

		std::size_t index{ static_cast<std::size_t>(level * samples.m_length) };

		return currentValue-samples.m_yVals[index];
	}

	auto sampleCVAR(XYVals samples, double level, double currentValue) -> double
	{
		// compute the value at risk based on the given samples
		assert(samples.m_length > 100); // ensure we have a sufficient number of samples

		// sort the samples so that we can easily get the cVar
		std::sort(std::begin(samples.m_yVals), std::end(samples.m_yVals));

		std::size_t index{ static_cast<std::size_t>(level * samples.m_length) };

		// compute mean over losses exceeding var
		double mean{ 0. };
		for (std::size_t i{ 0 }; i <= index; ++i)
		{
			mean += currentValue - samples.m_yVals[i];
		}
		mean /= static_cast<double>(index + 1);

		return mean;
	}

	void testSampleRiskMeasures()
	{
		double initialState{ 100. };
		double terminalTime{ 0.2 };
		std::size_t samples{ 50000 };
		double drift{ 0.05 };
		double volatility{ 0.4 };
		XYVals mcSamplesBSM{ SDE::BSM::monteCarlo(initialState, terminalTime, samples, drift, volatility) };

		std::size_t timePoints{ 1000 };
		double initialVariance{ volatility * volatility };
		double longVariance{ volatility * volatility };
		double hestonDrift{ drift };
		double correlation{ -0.9 };
		double reversionRate{ 1.5 };
		double volVol{ 0.6 };
		XYVals mcSamplesHeston{ SDE::Heston::monteCarlo(initialState, terminalTime, samples, timePoints, hestonDrift, initialVariance, longVariance, correlation, reversionRate, volVol) };

		double variance{ 0.15 };
		XYVals mcSamplesVarianceGamma{ SDE::VarianceGamma::monteCarlo(initialState, terminalTime, samples, timePoints, drift, variance, volatility) };

		
		// compute var and cvar
		double level{ 0.05 };
		double bsmVar{ sampleVAR(mcSamplesBSM, level, initialState) };
		double hestonVar{ sampleVAR(mcSamplesHeston, level, initialState) };
		double vgVar{ sampleVAR(mcSamplesVarianceGamma, level, initialState) };

		double bsmCVAR{ sampleCVAR(mcSamplesBSM, level, initialState) };
		double hestonCVAR{ sampleCVAR(mcSamplesHeston, level, initialState) };
		double vgCVAR{ sampleCVAR(mcSamplesVarianceGamma, level, initialState) };

		std::cout << "BSM VAR at the " << level * 100. << "% level is $" << bsmVar << ". CVAR at the same level is $" << bsmCVAR << ".\n";
		std::cout << "Heston VAR at the " << level * 100. << "% level is $" << hestonVar << ". CVAR at the same level is $" << hestonCVAR << ".\n";
		std::cout << "Variance Gamma VAR at the " << level * 100. << "% level is $" << vgVar << ". CVAR at the same level is $" << vgCVAR << ".\n";

	}

}