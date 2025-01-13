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


// Base struct for parameter polymorphism
struct ParamsBase {};

struct BSMParams : public ParamsBase
{
	BSMParams(double vola = 0.1)
		: vol{ vola } 
	{}
	double vol{ 0.1 };
};

struct BachelierParams : public ParamsBase
{
	BachelierParams(double vola = 10.0)
		: vol{ vola }
	{}
	double vol{ 10.0 };
};

struct CEVParams : public ParamsBase
{
	CEVParams(double vola = 0.1, double exp = 0.5)
		: vol{ vola }
		, exponent{exp}
	{}
	double vol{ 0.5 };
	double exponent{ 0.5 };
};

struct MertonJumpParams : public ParamsBase
{
	MertonJumpParams(double vola = 0.1, double mean = 0.0, double std = 0.2, double exp = 1.)
		: vol{ vola }
		, meanJumpSize{ mean }
		, stdJumpSize{ std }
		, expectedJumpsPerYear{ exp }
	{}
	double vol{ 0.1 };
	double meanJumpSize{ 0.0 };
	double stdJumpSize{ 0.2 };
	double expectedJumpsPerYear{ 1. };
};


struct HestonParams : public ParamsBase
{
	HestonParams(double rr = 0.3, double lv = 15., double vv = 0.2, double cr = 0.2, double iv = 8.)
		: reversionRate{ rr }
		, longVariance{ lv }
		, volVol{ vv }
		, correlation{ cr }
		, initialVariance{ iv }
	{}
	double reversionRate{ 0.3 };
	double longVariance{ 15. };
	double volVol{ 0.2 };
	double correlation{ 0.2 };
	double initialVariance{ 8. };
};

struct VarianceGammaParams : public ParamsBase
{
	VarianceGammaParams(double vola = 0.1, double gdrift = 0.5, double var = 0.2)
		: vol{ vola }
		, drift{ gdrift }
		, variance{ var }
	{}
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
		auto monteCarloPaths(double initialState, double terminalTime, std::size_t samples, std::size_t timePoints, double drift, BSMParams params) -> DataTable;


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
		auto monteCarloPaths(double initialState, double terminalTime, std::size_t samples, std::size_t timePoints, double drift, double volatility, double exponent) -> DataTable;

		// overloads for param structs
		auto path(double initialState, double terminalTime, std::size_t timePoints, double drift, CEVParams params) -> XYVals;
		auto monteCarlo(double initialState, double terminalTime, std::size_t samples, std::size_t timePoints, double drift, CEVParams params) -> XYVals;
		auto monteCarloPaths(double initialState, double terminalTime, std::size_t samples, std::size_t timePoints, double drift, CEVParams params) -> DataTable;

	}
	namespace MertonJump
	{
		auto simulate(double initialState, double time, double drift, double volatility, double meanJumpSize, double stdJumpSize, double expectedJumpsPerYear) -> double;
		auto path(double initialState, double terminalTime, std::size_t timePoints, double drift, double volatility, double meanJumpSize, double stdJumpSize, double expectedJumpsPerYear) -> XYVals;
		auto monteCarlo(double initialState, double terminalTime, std::size_t samples, double drift, double volatility, double meanJumpSize, double stdJumpSize, double expectedJumpsPerYear) -> XYVals;
		auto monteCarloPaths(double initialState, double terminalTime, std::size_t samples, std::size_t timePoints, double drift, double volatility, double meanJumpSize, double stdJumpSize, double expectedJumpsPerYear) -> DataTable;

		// Overloads with Param structs
		auto simulate(double initialState, double time, double drift, MertonJumpParams params) -> double;
		auto path(double initialState, double terminalTime, std::size_t timePoints, double drift, MertonJumpParams params) -> XYVals;
		auto monteCarlo(double initialState, double terminalTime, std::size_t samples, double drift, MertonJumpParams params) -> XYVals;
		auto monteCarloPaths(double initialState, double terminalTime, std::size_t samples, std::size_t timePoints, double drift, MertonJumpParams params) -> DataTable;
	}
	namespace Heston
	{
		auto varianceStep(double initialVariance, double stepSize, double longVariance, double correlatedNormal, double reversionRate, double volVol) -> double;
		auto priceStep(double initialState, double stepSize, double drift, double variance, double correlatedNormal) -> double;
		auto path(double initialState, double terminalTime, std::size_t timePoints, double drift, double initialVariance, double longVariance, double correlation, double reversionRate, double volVol) -> XYVals;
		auto monteCarlo(double initialState, double terminalTime, std::size_t samples, std::size_t timePoints, double drift, double initialVariance, double longVariance, double correlation, double reversionRate, double volVol) -> XYVals;
		auto monteCarloPaths(double initialState, double terminalTime, std::size_t samples, std::size_t timePoints, double drift, double initialVariance, double longVariance, double correlation, double reversionRate, double volVol) -> DataTable;

		// overloads for param structs
		auto path(double initialState, double terminalTime, std::size_t timePoints, double drift, HestonParams params) -> XYVals;
		auto monteCarlo(double initialState, double terminalTime, std::size_t samples, std::size_t timePoints, double drift, HestonParams params) -> XYVals;
		auto monteCarloPaths(double initialState, double terminalTime, std::size_t samples, std::size_t timePoints, double drift, HestonParams params) -> DataTable;

	}
	namespace VarianceGamma
	{
		auto step(double initialState, double stepSize, double drift, double gammaDrift, double variance, double vol) -> double;
		auto path(double initialState, double terminalTime, std::size_t timePoints, double drift, double gammaDrift, double variance, double vol) -> XYVals;
		auto monteCarlo(double initialState, double terminalTime, std::size_t samples, std::size_t timePoints, double drift, double gammaDrift, double variance, double vol) -> XYVals;
		auto monteCarloPaths(double initialState, double terminalTime, std::size_t samples, std::size_t timePoints, double drift, double gammaDrift, double variance, double vol) -> DataTable;

		// overloads for param structs
		auto path(double initialState, double terminalTime, std::size_t timePoints, double drift, VarianceGammaParams params) -> XYVals;
		auto monteCarlo(double initialState, double terminalTime, std::size_t samples, std::size_t timePoints, double drift, VarianceGammaParams params) -> XYVals;
		auto monteCarloPaths(double initialState, double terminalTime, std::size_t samples, std::size_t timePoints, double drift, VarianceGammaParams params) -> DataTable;

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

	auto monteCarloPaths(double initialState, double terminalTime, std::size_t samples, std::size_t timePoints, double drift, BSMParams& params) -> DataTable;
	auto monteCarloPaths(double initialState, double terminalTime, std::size_t samples, std::size_t timePoints, double drift, BachelierParams& params) -> DataTable;
	auto monteCarloPaths(double initialState, double terminalTime, std::size_t samples, std::size_t timePoints, double drift, CEVParams& params) -> DataTable;
	auto monteCarloPaths(double initialState, double terminalTime, std::size_t samples, std::size_t timePoints, double drift, MertonJumpParams& params) -> DataTable;
	auto monteCarloPaths(double initialState, double terminalTime, std::size_t samples, std::size_t timePoints, double drift, HestonParams& params) -> DataTable;
	auto monteCarloPaths(double initialState, double terminalTime, std::size_t samples, std::size_t timePoints, double drift, VarianceGammaParams& params) -> DataTable;
	template <typename Params>
	auto monteCarloPaths(double initialState, double terminalTime, std::size_t samples, std::size_t timePoints, double drift, Params& params) -> DataTable
	{
		if constexpr (std::is_same_v<Params, BSMParams>) {
			return monteCarloPaths(initialState, terminalTime, samples, timePoints, drift, static_cast<BSMParams&>(params));
		}
		else if constexpr (std::is_same_v<Params, BachelierParams>) {
			return monteCarloPaths(initialState, terminalTime, samples, timePoints, drift, static_cast<BachelierParams&>(params));
		}
		else if constexpr (std::is_same_v<Params, MertonJumpParams>) {
			return monteCarloPaths(initialState, terminalTime, samples, timePoints, drift, static_cast<MertonJumpParams&>(params));
		}
		else if constexpr (std::is_same_v<Params, CEVParams>) {
			return monteCarloPaths(initialState, terminalTime, samples, timePoints, drift, static_cast<CEVParams&>(params));
		}
		else if constexpr (std::is_same_v<Params, HestonParams>) {
			return monteCarloPaths(initialState, terminalTime, samples, timePoints, drift, static_cast<HestonParams&>(params));
		}
		else if constexpr (std::is_same_v<Params, VarianceGammaParams>) {
			return monteCarloPaths(initialState, terminalTime, samples, timePoints, drift, static_cast<VarianceGammaParams&>(params));
		}
		else {
			return monteCarloPaths(initialState, terminalTime, samples, timePoints, drift, static_cast<BSMParams&>(params));
		}
	}
}

#endif
