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
		auto chiSquared(double x, int k) -> double
		{
			return Utils::lower_incomplete_gamma(k / 2., x / 2.) / std::tgamma(k / 2.);
		}
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

	namespace PDFs
	{
		auto chiSquared(double x, int k) -> double
		{
			return std::pow(x, k / 2. - 1.) * std::exp(-x / 2.) / (std::pow(2., k / 2.) * std::tgamma(k / 2.));
		}
		auto standardNormal(double x) -> double
		{
			double pi = 3.1415926535;
			return 1.0 / sqrt(2 * pi) * exp(-x * x / 2.0);
		}
		// derivative of pdf
		auto standardNormal_dx(double x) -> double
		{
			double pi = 3.1415926535;
			return - x / sqrt(2 * pi) * exp(-x * x / 2.0);
		}
	}

	namespace Utils
	{

		template <typename T>
		auto factorial(T n) -> T
		{
			return (n == 1 || n == 0) ? 1 : factorial(n - 1) * n;
		}

		template <typename T>
		auto binomialCoefficient(T n, T k) -> T
		{
			return factorial(n) / (factorial(k) * factorial(n - k));
		}

		// Numerical integration of gamma(a, x) using the trapezoidal rule
		auto lower_incomplete_gamma(double s, double x, int n) -> double 
		{
			double h = x / n;  // Step size
			double sum = 0.0;

			for (int i = 1; i < n; ++i) {
				double t = i * h;
				sum += std::pow(t, s - 1) * std::exp(-t);
			}

			// Add contributions from the endpoints
			sum += 0.5 * (std::pow(0, s - 1) * std::exp(0) + std::pow(x, s - 1) * std::exp(-x));
			sum *= h;

			return sum;
		}
	}
}