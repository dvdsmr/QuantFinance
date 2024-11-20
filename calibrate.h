#ifndef CALIBRATE_H
#define CALIBRATE_H

#include "sdes.h"
#include "numpy.h"
#include "fft.h"
#include "reading.h"
#include "adam.h"
#include "options.h"
#include "pso.h"
#include "saving.h"
#include <algorithm>
#include <cassert>

namespace Calibrate
{
	auto interpolatePrices(const FFT::LogStrikePricePair& pair, const std::vector<double>& strikes) -> std::vector<double>;
	auto computeFFTModelMRSE(const LabeledTable& priceSurface, MarketParams& marketParams, const auto& modelParams, const FFT::FFTParams& params, LabeledTable& modelPriceSurface, LabeledTable& errorSurface) -> double;
	auto computeBSM_MRSE(const LabeledTable& priceSurface, MarketParams& marketParams, const BSMParams& modelParams, LabeledTable& modelPriceSurface, LabeledTable& errorSurface) -> double;
	
	namespace BSM
	{
		auto Call(const LabeledTable& priceSurface, double riskFreeReturn, double spot, double dividendYield, std::string_view pricing = "analytic") -> BSMParams;
		auto CallPSO(const LabeledTable& priceSurface, double riskFreeReturn, double spot, double dividendYield) -> BSMParams;
		void test();
		void calibrateToRealData(std::string symbol, double spot);
		void saveLossShape(double riskFreeReturn, double dividendYield, double maturity, double strike, double spot, double truePrice);

	}
	
	namespace Heston
	{
		auto Call(const LabeledTable& priceSurface, double riskFreeReturn, double spot, double dividendYield) -> HestonParams;
		auto CallPSO(const LabeledTable& priceSurface, double riskFreeReturn, double spot, double dividendYield) -> HestonParams;
		void test();
	}

	namespace VarianceGamma
	{
		auto Call(const LabeledTable& priceSurface, double riskFreeReturn, double spot, double dividendYield) -> VarianceGammaParams;
		auto CallPSO(const LabeledTable& priceSurface, double riskFreeReturn, double spot, double dividendYield) -> VarianceGammaParams;
		void test();
	}

}

#endif
