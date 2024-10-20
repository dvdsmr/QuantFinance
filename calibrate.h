#ifndef CALIBRATE_H
#define CALIBRATE_H

#include "sdes.h"
#include "numpy.h"
#include "fft.h"

namespace Calibrate
{
	auto interpolatePrices(const FFT::LogStrikePricePair& pair, const std::vector<double>& strikes) -> std::vector<double>;
	auto computeFFTModelMRSE(const LabeledTable& priceSurface, MarketParams& marketParams, const auto& modelParams, const FFT::FFTParams& params, LabeledTable& modelPriceSurface, LabeledTable& errorSurface) -> double;
	auto computeBSM_MRSE(const LabeledTable& priceSurface, MarketParams& marketParams, const BSMParams& modelParams, LabeledTable& modelPriceSurface, LabeledTable& errorSurface) -> double;
	auto hestonCall(const LabeledTable& priceSurface, double riskFreeReturn, double spot, double dividendYield) -> HestonParams;
	auto bsmCall(const LabeledTable& priceSurface, double riskFreeReturn, double spot, double dividendYield, std::string_view pricing = "analytic") -> BSMParams;
	auto varianceGammaCall(const LabeledTable& priceSurface, double riskFreeReturn, double spot, double dividendYield) -> VarianceGammaParams;
	void testHeston();
	void testBSM();
	void testVarianceGamma();
}

#endif
