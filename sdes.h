#ifndef SDES_H
#define SDES_H
#include "stockpaths.h"
#include <vector>

namespace SDE
{
	auto OrnsteinUhlenbeckStep(double state, double stepSize, double drift, double diffusion) -> double;
	auto geometricBrownianMotion(double initialState, double time, double drift, double volatility) -> double;
	auto OrnsteinUhlenbeckSimulate(double state, double stepSize, double drift, double diffusion, double time) -> double;
	auto geometricBrownianMotionPath(double initialState, double terminalTime, std::size_t timePoints, double drift, double volatility) -> StockPath;
	auto HestonVarianceStep(double initialVariance, double stepSize, double longVariance, double correlatedNormal, double reversionRate, double volVol) -> double;
	auto HestonPriceStep(double initialState, double stepSize, double drift, double variance, double correlatedNormal) -> double;
	auto HestonPath(double initialState, double terminalTime, std::size_t timePoints, double drift, double initialVariance, double longVariance, double correlation, double reversionRate, double volVol) -> StockPath;
}

#endif
