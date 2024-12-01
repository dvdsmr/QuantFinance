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
		auto noncentralChiSquared(double x, int k, double lambda) -> double;
		auto standardNormal(double x) -> double;
	}
	namespace PDFs
	{
		auto chiSquared(double x, int k) -> double;
		auto noncentralChiSquared(double x, int k, double lambda) -> double;
		auto standardNormal(double x) -> double;
		auto standardNormal_dx(double x) -> double;
	}
	namespace Utils
	{

		template <typename T>
		auto binomialCoefficient(T n, T k) -> T;

		template <typename T>
		auto factorial(T n) -> T;

		auto lower_incomplete_gamma(double s, double x, int n = 1000) -> double;
		auto modifiedBessel(double alpha, double x, int maxTerms = 100, double tol = 1e-10) -> double;
		auto marcumQ(double nu, double a, double b, int n = 1000) -> double;
	}
}

#endif
