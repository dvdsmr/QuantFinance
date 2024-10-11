#ifndef SDES_H
#define SDES_H
#include "xyvals.h"
#include "saving.h"
#include <vector>
#include <complex>
#include <iostream>
#include <type_traits>

// Helper template to check if a type T has a member named 'vol'
template<typename T, typename = void>
struct has_vol : std::false_type {};

template<typename T>
struct has_vol<T, std::void_t<decltype(std::declval<T>().vol)>> : std::true_type {};


struct BSMParams
{
	double vol{ 0.1 };
};

struct HestonParams
{
	double reversionRate{ 0.3 };
	double longVariance{ 15. };
	double volVol{ 0.2 };
	double correlation{ 0.2 };
	double initialVariance{ 8. };
	// should be deleted
	// double vol{ 0.8 };
};

struct MarketParams
{
	double maturity{ 1.0 };
	double spot{ 100.0 };
	double riskFreeReturn{ 0.095 };
	double dividendYield{ 0.005 };
};

namespace SDE
{
	auto OrnsteinUhlenbeckStep(double state, double stepSize, double drift, double diffusion) -> double;
	auto geometricBrownianMotion(double initialState, double time, double drift, double volatility) -> double;
	auto OrnsteinUhlenbeckSimulate(double state, double stepSize, double drift, double diffusion, double time) -> double;
	auto geometricBrownianMotionPath(double initialState, double terminalTime, std::size_t timePoints, double drift, double volatility) -> XYVals;
	auto HestonVarianceStep(double initialVariance, double stepSize, double longVariance, double correlatedNormal, double reversionRate, double volVol) -> double;
	auto HestonPriceStep(double initialState, double stepSize, double drift, double variance, double correlatedNormal) -> double;
	auto HestonPath(double initialState, double terminalTime, std::size_t timePoints, double drift, double initialVariance, double longVariance, double correlation, double reversionRate, double volVol) -> XYVals;

	namespace CharacteristicFunctions
	{
		auto BSM(std::complex<double> argument, double riskFreeReturn, double vol, double maturity, double spot, double dividendYield) -> std::complex<double>;
		auto Heston(std::complex<double> argument, double riskFreeReturn, double initialVariance, double longVariance, double correlation, double reversionRate, double volVol, double maturity, double spot, double dividendYield) -> std::complex<double>;
		auto generalCF(std::complex<double> argument, const HestonParams& modelParams, const MarketParams& marketParams) -> std::complex<double>;
		auto generalCF(std::complex<double> argument, const BSMParams& modelParams, const MarketParams& marketParams) -> std::complex<double>;
	}

	namespace Testing
	{
		auto saveHestonPaths() -> void;
	}
}

#endif
