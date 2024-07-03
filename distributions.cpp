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

	namespace Utils
	{
		auto factorial(int n) -> int
		{
			return (n == 1 || n == 0) ? 1 : factorial(n - 1) * n;
		}

		auto binomialCoefficient(int n, int k) -> int
		{
			return factorial(n) / (factorial(k) * factorial(n - k));
		}
	}
}