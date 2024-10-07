#ifndef SDES_H
#define SDES_H
#include "xyvals.h"
#include "saving.h"
#include <vector>
#include <complex>

struct BSMParams
{
	double vol{ 0.1 };
};

struct HestonParams
{
	double reversionRate{ 0.1 };
	double longVariance{ 0.1 };
	double volVol{ 0.1 };
	double correlation{ 0.1 };
	double initialVariance{ 0.1 };
};

struct MarketParams
{
	double maturity{ 1.0 };
	double spot{ 100.0 };
	double riskFreeReturn{ 0.05 };
	double dividendYield{ 0.007 };
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
		auto BSM(double argument, double riskFreeReturn, double vol, double maturity, double spot, double dividendYield) -> std::complex<double>;
		auto Heston(double argument, double riskFreeReturn, double initialVariance, double longVariance, double correlation, double reversionRate, double volVol, double maturity, double spot, double dividendYield) -> std::complex<double>;
		auto generalCF(double argument, std::string_view model, const auto& modelParams, const MarketParams& marketParams) -> std::complex<double>;
	}

	namespace Testing
	{
		auto saveHestonPaths() -> void;
	}
}

#endif
