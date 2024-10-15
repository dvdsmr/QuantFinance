#ifndef CALIBRATE_H
#define CALIBRATE_H

#include "sdes.h"
#include "numpy.h"
#include "fft.h"

namespace Calibrate
{
	auto interpolatePrices(const FFT::LogStrikePricePair& pair, const std::vector<double>& strikes) -> std::vector<double>;
	auto hestonTest(const LabeledTable& priceSurface, double riskFreeReturn, double spot, double dividendYield, std::string_view type = "call", std::string_view optimizer = "bruteForce") -> HestonParams;
}

#endif
