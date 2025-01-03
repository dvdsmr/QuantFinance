#ifndef FFT_H
#define FFT_H
#include "sdes.h"
#include <vector>
#include <complex>

constexpr std::complex<double> IMNUM(0.0, 1.0);
constexpr double PI = 3.14159265358979323846;

namespace FFT
{
	struct ModePair
	{
		std::vector<std::complex<double>> evens{};
		std::vector<std::complex<double>> odds{};

		ModePair(std::size_t length)
			: evens{ std::vector<std::complex<double>>((length + 1) / 2) }
			, odds{ std::vector<std::complex<double>>(length / 2) }
		{}

	};

	struct FFTParams
	{
		double decayParam{ 1.5 };
		double gridWidth{ 0.1 };
		int gridExponent{ 14 };
	};

	struct LogStrikePricePair
	{
		std::vector<double> logStrikes{};
		std::vector<double> prices{};
	};

	auto separateModes(const std::vector<std::complex<double>>& vec) -> ModePair;
	auto intPow(int base, int exponent) -> int;
	template <typename T>
	auto concatenate(std::vector<T>& vec1, const std::vector<T>& vec2) -> std::vector<T>;
	auto dft(const std::vector<std::complex<double>>& vec) -> std::vector<std::complex<double>>;
	auto fft(const std::vector<std::complex<double>>& vec) -> std::vector<std::complex<double>>;
	auto pricingfft(const auto& modelParams, const MarketParams& marketParams, const FFTParams& params, std::string_view type = "call") -> LogStrikePricePair;
	auto pricingfftHeston(const HestonParams& modelParams, const MarketParams& marketParams, const FFTParams& params, std::string_view type = "call") -> LogStrikePricePair;
	auto pricingfftBSM(const BSMParams& modelParams, const MarketParams& marketParams, const FFTParams& params, std::string_view type = "call") -> LogStrikePricePair;
	auto pricingfftMertonJump(const MertonJumpParams& modelParams, const MarketParams& marketParams, const FFTParams& params, std::string_view type = "call") -> LogStrikePricePair;
	auto pricingfftVarianceGamma(const VarianceGammaParams& modelParams, const MarketParams& marketParams, const FFTParams& params, std::string_view type = "call") -> LogStrikePricePair;


	namespace UnitTests
	{
		void separateModes();
		void dft();
		void pricingfft();
	}
}

#endif
