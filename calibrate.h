#ifndef CALIBRATE_H
#define CALIBRATE_H

#include "sdes.h"
#include "numpy.h"
#include "fft.h"

namespace Calibrate
{
	auto interpolatePrices(const FFT::LogStrikePricePair& pair, const std::vector<double>& strikes) -> std::vector<double>;
	auto hestonCall(const LabeledTable& priceSurface, double riskFreeReturn, double spot, double dividendYield) -> HestonParams;
	auto bsmCall(const LabeledTable& priceSurface, double riskFreeReturn, double spot, double dividendYield, std::string_view pricing = "analytic") -> BSMParams;
	void testHeston();
	void testBSM();
}

#endif
