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
	namespace Utils
	{
		auto binomialCoefficient(int n, int k) -> int;
		auto factorial(int n) -> int;
	}
}

#endif
