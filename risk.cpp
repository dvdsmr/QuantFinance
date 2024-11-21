#include "risk.h"

namespace Risk
{
	auto sampleCVAR(XYVals samples, double level, double currentValue) -> double
	{
		// compute the value at risk based on the given samples
		assert(samples.m_length > 100); // ensure we have a sufficient number of samples
	
		// sort the samples so that we can easily get the cVar
		std::sort(std::begin(samples.m_yVals), std::end(samples.m_yVals));

		std::size_t index{ static_cast<std::size_t>(level * samples.m_length) };

		return currentValue-samples.m_yVals[index];
	}
}