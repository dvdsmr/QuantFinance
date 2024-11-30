#ifndef DISTRIBUTIONS_H
#define DISTRIBUTIONS_H

namespace Distributions
{
	namespace MomentGeneratingFunctions
	{
		auto normal(double argument, double mean, double volatility) -> double;
	}
	namespace CDFs
	{
		auto chiSquared(double x, int k) -> double;
		auto standardNormal(double x) -> double;
	}
	namespace PDFs
	{
		auto chiSquared(double x, int k) -> double;
		auto standardNormal(double x) -> double;
		auto standardNormal_dx(double x) -> double;
	}
	namespace Utils
	{
		auto lower_incomplete_gamma(double s, double x, int n = 1000) -> double;

		template <typename T>
		auto binomialCoefficient(T n, T k) -> T;

		template <typename T>
		auto factorial(T n) -> T;
	}
}

#endif
