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

	namespace CDFs
	{
		auto standardNormal(double x) -> double
		{
			// taken from https://www.johndcook.com/blog/cpp_phi/
			// constants
			double a1 = 0.254829592;
			double a2 = -0.284496736;
			double a3 = 1.421413741;
			double a4 = -1.453152027;
			double a5 = 1.061405429;
			double p = 0.3275911;

			// Save the sign of x
			int sign = 1;
			if (x < 0)
				sign = -1;
			x = fabs(x) / sqrt(2.0);

			// A&S formula 7.1.26
			double t = 1.0 / (1.0 + p * x);
			double y = 1.0 - (((((a5 * t + a4) * t) + a3) * t + a2) * t + a1) * t * exp(-x * x);

			return 0.5 * (1.0 + sign * y);
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