#ifndef FFT_H
#define FFT_H
#include "sdes.h"
#include <vector>
#include <complex>


namespace FFT
{
	struct fttParams
	{
		double decayParam{ 1.5 };
		double gridWidth{ 0.2 };
		int gridExponent{ 12 };
	};

	struct LogStrikePricePair
	{
		std::vector<double> logStrikes{};
		std::vector<double> prices{};
	};

	auto intPow(int base, int exponent) -> int;
	auto fft(std::vector<std::complex<double>> vec) -> std::vector<std::complex<double>>;
	auto pricingfft(std::string_view model, const auto& modelParams, const MarketParams& marketParams, const fttParams& params) -> LogStrikePricePair;
}

#endif
