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

struct BachelierParams
{
	double vol{ 0.1 };
};

struct CEVParams
{
	double vol{ 0.5 };
	double exponent{ 0.5 };
};

struct MertonJumpParams
{
	double vol{ 0.1 };
	double meanJumpSize{ 0.0 };
	double stdJumpSize{ 0.2 };
	double expectedJumpsPerYear{ 1. };
};


struct HestonParams
{
	double reversionRate{ 0.3 };
	double longVariance{ 15. };
	double volVol{ 0.2 };
	double correlation{ 0.2 };
	double initialVariance{ 8. };
};

struct VarianceGammaParams
{
	double vol{ 0.1 };
	double drift{ 0.5 };
	double variance{ 0.2 };
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
	namespace OrnsteinUhlenbeck
	{
		auto simulate(double state, double time, double drift, double mean, double diffusion) -> double;
	}
	namespace BSM
	{
		auto simulate(double initialState, double time, double drift, double volatility) -> double;
		auto path(double initialState, double terminalTime, std::size_t timePoints, double drift, double volatility) -> XYVals;
		auto monteCarlo(double initialState, double terminalTime, std::size_t samples, double drift, double volatility) -> XYVals;
		auto monteCarloPaths(double initialState, double terminalTime, std::size_t samples, std::size_t timePoints, double drift, double volatility) -> DataTable;
	
		// overloads with param structs
		auto simulate(double initialState, double time, double drift, BSMParams params) -> double;
		auto path(double initialState, double terminalTime, std::size_t timePoints, double drift, BSMParams params) -> XYVals;
		auto monteCarlo(double initialState, double terminalTime, std::size_t samples, double drift, BSMParams params) -> XYVals;

	}
	namespace Bachelier
	{
		auto simulate(double initialState, double time, double drift, double volatility) -> double;
		auto path(double initialState, double terminalTime, std::size_t timePoints, double drift, double volatility) -> XYVals;
		auto monteCarlo(double initialState, double terminalTime, std::size_t samples, double drift, double volatility) -> XYVals;
	
		// Overloads with Param structs
		auto simulate(double initialState, double time, double drift, BachelierParams params) -> double;
		auto path(double initialState, double terminalTime, std::size_t timePoints, double drift, BachelierParams params) -> XYVals;
		auto monteCarlo(double initialState, double terminalTime, std::size_t samples, double drift, BachelierParams params) -> XYVals;
	}
	namespace CEV
	{
		auto step(double state, double time, double drift, double volatility, double exponent) -> double;
		auto path(double initialState, double terminalTime, std::size_t timePoints, double drift, double volatility, double exponent) -> XYVals;
		auto monteCarlo(double initialState, double terminalTime, std::size_t samples, std::size_t timePoints, double drift, double volatility, double exponent) -> XYVals;
	
		// overloads for param structs
		auto path(double initialState, double terminalTime, std::size_t timePoints, double drift, CEVParams params) -> XYVals;
		auto monteCarlo(double initialState, double terminalTime, std::size_t samples, std::size_t timePoints, double drift, CEVParams params) -> XYVals;
	}
	namespace MertonJump
	{
		auto simulate(double initialState, double time, double drift, double volatility, double meanJumpSize, double stdJumpSize, double expectedJumpsPerYear) -> double;
		auto path(double initialState, double terminalTime, std::size_t timePoints, double drift, double volatility, double meanJumpSize, double stdJumpSize, double expectedJumpsPerYear) -> XYVals;
		auto monteCarlo(double initialState, double terminalTime, std::size_t samples, double drift, double volatility, double meanJumpSize, double stdJumpSize, double expectedJumpsPerYear) -> XYVals;
	
		// Overloads with Param structs
		auto simulate(double initialState, double time, double drift, MertonJumpParams params) -> double;
		auto path(double initialState, double terminalTime, std::size_t timePoints, double drift, MertonJumpParams params) -> XYVals;
		auto monteCarlo(double initialState, double terminalTime, std::size_t samples, double drift, MertonJumpParams params) -> XYVals;
	}
	namespace Heston
	{
		auto varianceStep(double initialVariance, double stepSize, double longVariance, double correlatedNormal, double reversionRate, double volVol) -> double;
		auto priceStep(double initialState, double stepSize, double drift, double variance, double correlatedNormal) -> double;
		auto path(double initialState, double terminalTime, std::size_t timePoints, double drift, double initialVariance, double longVariance, double correlation, double reversionRate, double volVol) -> XYVals;
		auto monteCarlo(double initialState, double terminalTime, std::size_t samples, std::size_t timePoints, double drift, double initialVariance, double longVariance, double correlation, double reversionRate, double volVol) -> XYVals;
	
		// overloads for param structs
		auto path(double initialState, double terminalTime, std::size_t timePoints, double drift, HestonParams params) -> XYVals;
		auto monteCarlo(double initialState, double terminalTime, std::size_t samples, std::size_t timePoints, double drift, HestonParams params) -> XYVals;
	}
	namespace VarianceGamma
	{
		auto step(double initialState, double stepSize, double drift, double gammaDrift, double variance, double vol) -> double;
		auto path(double initialState, double terminalTime, std::size_t timePoints, double drift, double gammaDrift, double variance, double vol) -> XYVals;
		auto monteCarlo(double initialState, double terminalTime, std::size_t samples, std::size_t timePoints, double drift, double gammaDrift, double variance, double vol) -> XYVals;
	
		// overloads for param structs
		auto path(double initialState, double terminalTime, std::size_t timePoints, double drift, VarianceGammaParams params) -> XYVals;
		auto monteCarlo(double initialState, double terminalTime, std::size_t samples, std::size_t timePoints, double drift, VarianceGammaParams params) -> XYVals;
	}


	namespace CharacteristicFunctions
	{
		auto BSM(std::complex<double> argument, double riskFreeReturn, double vol, double maturity, double spot, double dividendYield) -> std::complex<double>;
		auto Heston(std::complex<double> argument, double riskFreeReturn, double initialVariance, double longVariance, double correlation, double reversionRate, double volVol, double maturity, double spot, double dividendYield) -> std::complex<double>;
		auto MertonJump(std::complex<double> argument, double riskFreeReturn, double vol, double maturity, double spot, double dividendYield, double meanJumpSize, double stdJumpSize, double expectedJumpsPerYear) -> std::complex<double>;
		auto VarianceGamma(std::complex<double> argument, double riskFreeReturn, double maturity, double spot, double dividendYield, double volatility, double drift, double variance) -> std::complex<double>;
		auto generalCF(std::complex<double> argument, const HestonParams& modelParams, const MarketParams& marketParams) -> std::complex<double>;
		auto generalCF(std::complex<double> argument, const BSMParams& modelParams, const MarketParams& marketParams) -> std::complex<double>;
		auto generalCF(std::complex<double> argument, const MertonJumpParams& modelParams, const MarketParams& marketParams) -> std::complex<double>;
		auto generalCF(std::complex<double> argument, const VarianceGammaParams& modelParams, const MarketParams& marketParams) -> std::complex<double>;
	}

	namespace Testing
	{
		auto saveMCsamples() -> void;
		auto saveHestonPaths() -> void;
		auto saveVarianceGammaPaths() -> void;
		auto saveMertonJumpPaths() -> void;
	}

	// general overload for includsion into ModelStock class
	auto path(double initialState, double terminalTime, std::size_t timePoints, double drift, BSMParams params) -> XYVals;
	auto path(double initialState, double terminalTime, std::size_t timePoints, double drift, BachelierParams params) -> XYVals;
	auto path(double initialState, double terminalTime, std::size_t timePoints, double drift, CEVParams params) -> XYVals;
	auto path(double initialState, double terminalTime, std::size_t timePoints, double drift, MertonJumpParams params) -> XYVals;
	auto path(double initialState, double terminalTime, std::size_t timePoints, double drift, HestonParams params) -> XYVals;
	auto path(double initialState, double terminalTime, std::size_t timePoints, double drift, VarianceGammaParams params) -> XYVals;

	auto monteCarlo(double initialState, double terminalTime, std::size_t samples, double drift, BSMParams params) -> XYVals;
	auto monteCarlo(double initialState, double terminalTime, std::size_t samples, double drift, BachelierParams params) -> XYVals;
	auto monteCarlo(double initialState, double terminalTime, std::size_t samples, double drift, CEVParams params) -> XYVals;
	auto monteCarlo(double initialState, double terminalTime, std::size_t samples, double drift, MertonJumpParams params) -> XYVals;
	auto monteCarlo(double initialState, double terminalTime, std::size_t samples, double drift, HestonParams params) -> XYVals;
	auto monteCarlo(double initialState, double terminalTime, std::size_t samples, double drift, VarianceGammaParams params) -> XYVals;

}

#endif
