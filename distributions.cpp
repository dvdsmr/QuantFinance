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
		auto noncentralChiSquared(double x, double k, double lambda) -> double
		{
			return 1. - Utils::marcumQ(k / 2.,std::sqrt(lambda),std::sqrt(x));
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
		auto noncentralChiSquared(double x, double k, double lambda) -> double
		{
			return 0.5 * std::exp(-0.5 * (x + lambda)) * std::pow((x / lambda), k / 4. - 1. / 2.) * Utils::modifiedBessel(k / 2. - 1., std::sqrt(lambda * x));
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
			double h{ x / n };  // Step size
			double sum{ 0.0 };

			for (int i = 1; i <= n; ++i) 
			{
				double t = i * h;
				sum += std::pow(t, s - 1) * std::exp(-t);
			}
			sum *= h;

			return sum;
		}

		// Compute the modified Bessel function of the first kind, I_nu(x), using its series definition
		auto modifiedBessel(double alpha, double x, int maxTerms, double tol) -> double
		{

			if (x > 30.0) // Switch to asymptotic expansion for large x
			{
				double term{ 1.0 };
				double sum{ term };
				double factor{ 1.0 / (2 * x) };

				for (int k = 1; k < maxTerms; ++k)
				{
					term *= -factor * (4 * alpha * alpha - (2 * k - 1) * (2 * k - 1)) / (2 * k);
					if (std::abs(term) < tol)
						break;
					sum += term;
				}

				return std::exp(x) / std::sqrt(2 * PI * x) * sum;
			}
			else // Use the series expansion for small x
			{
				double sum{ 0.0 };
				double term{ 1.0 }; // Initial term
				double x2{ x / 2.0 };

				for (int k = 0; k < maxTerms && std::abs(term) > tol; ++k)
				{
					term = std::pow(x2, 2 * k + alpha) / (factorial(k) * std::tgamma(k + alpha + 1));
					sum += term;
				}

				return sum;
			}

		}

		// Compute the logarithm of the modified Bessel function of the first kind
		auto logModifiedBessel(double alpha, double x, int maxTerms, double tol) -> double 
		{
			// If x is small, use the series expansion
			if (x < 3.0) 
			{
				double sum = 0.0;
				double term = 1.0;  // Initial term
				double x2 = x / 2.0;

				for (int k = 0; k < maxTerms && std::abs(term) > tol; ++k) 
				{
					term = std::pow(x2, 2 * k + alpha) / (factorial(k) * std::tgamma(k + alpha + 1));
					sum += term;
				}
				return std::log(sum);
			}
			/*
			// If x is large, use the asymptotic expansion
			return x - 0.5 * std::log(2.0 * PI * x);
			*/
			// If x is large, use the asymptotic expansion
			else 
			{
				// Leading term and logarithmic correction
				double logI = x - 0.5 * std::log(2.0 * PI * x);

				// Add correction terms for higher accuracy
				double correction = 0.0;
				double factor = 1.0;  // Tracks (4v^2 - 1)(4v^2 - 9)... for each term
				double xPower = 1.0;   // Tracks powers of 8x

				for (int k = 1; k <= maxTerms; ++k) {
					factor *= (4 * alpha * alpha - (2 * k - 1) * (2 * k - 1));
					xPower *= 8.0 * x;  // Next power of 8x
					correction += factor / xPower;

					// If the correction term is small, break early
					if (std::abs(factor / xPower) < 1e-12) {
						break;
					}
				}

				logI += std::log1p(correction);  // Accumulate correction terms
				return logI;
			}
		}


		/*
		// Numerical integration of the marcum Q function(a,b) using the trapezoidal rule
		auto marcumQ(double nu, double a, double b, int n) -> double
		{
			double h{ b / n };  // Step size
			double sum{ 0.0 };

			for (int i = 1; i <= n; ++i)
			{
				double x = i * h;
				sum += std::pow(x, nu) * std::exp(-0.5*(x*x+a*a)) * modifiedBessel(nu-1,a*x);
			}
			sum *= h;

			double result{ 1. - 1. / std::pow(a,nu - 1) * sum };
			return result;
		}
		*/


		// Numerically integrate the Marcum Q function in logarithmic space
		auto marcumQ(double nu, double a, double b, int n) -> double 
		{
			double h = b / n;  // Step size
			double logSum = 0.0;

			for (int i = 1; i <= n; ++i) 
			{
				double t = i * h;
				double logTerm = nu * std::log(t) - 0.5 * (t * t + a * a)
					+ logModifiedBessel(nu - 1, a * t);

				// Convert back from log-space and accumulate
				logSum += std::exp(logTerm);
			}
			logSum *= h;

			// Final adjustment for the Marcum Q definition
			return 1.0 - (1.0 / std::pow(a, nu - 1)) * logSum;
		}

	}
}