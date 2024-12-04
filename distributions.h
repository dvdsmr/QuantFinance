#ifndef DISTRIBUTIONS_H
#define DISTRIBUTIONS_H

#include "fft.h"

namespace Distributions
{
	namespace MomentGeneratingFunctions
	{
		auto normal(double argument, double mean, double volatility) -> double;
	}
	namespace CDFs
	{
		auto chiSquared(double x, int k) -> double;
		auto noncentralChiSquared(double x, double k, double lambda) -> double;
		auto standardNormal(double x) -> double;
	}
	namespace PDFs
	{
		auto chiSquared(double x, int k) -> double;
		auto noncentralChiSquared(double x, double k, double lambda) -> double;
		auto standardNormal(double x) -> double;
		auto standardNormal_dx(double x) -> double;
	}
	namespace Utils
	{

		template <typename T>
		auto binomialCoefficient(T n, T k) -> T;

		template <typename T>
		auto factorial(T n) -> T;

		auto lower_incomplete_gamma(double s, double x, int n = 5000) -> double;
		auto modifiedBessel(double alpha, double x, int maxTerms = 500, double tol = 1e-10) -> double;
		auto logModifiedBessel(double alpha, double x, int maxTerms = 5, double tol = 1e-10) -> double;
		auto marcumQ(double nu, double a, double b, int n = 5000) -> double;
	}
}

#endif
