#include "distributions.h"
#include <cmath>

namespace Distributions
{
	namespace MomentGeneratingFunctions
	{
		auto normal(double argument, double mean, double volatility) -> double
		{
			return std::exp(mean * argument + std::pow(volatility, 2) * std::pow(argument, 2) / 2);
		}
	}
}