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
		auto standardNormal(double x) -> double;
	}
	namespace PDFs
	{
		auto standardNormal(double x) -> double;
	}
	namespace Utils
	{
		template <typename T>
		auto binomialCoefficient(T n, T k) -> T;

		template <typename T>
		auto factorial(T n) -> T;
	}
}

#endif
