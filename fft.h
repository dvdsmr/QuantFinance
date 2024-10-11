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

	auto separateModes(const std::vector<std::complex<double>>& vec) -> ModePair;
	auto intPow(int base, int exponent) -> int;
	template <typename T>
	auto concatenate(std::vector<T>& vec1, const std::vector<T>& vec2) -> std::vector<T>;
	auto dft(const std::vector<std::complex<double>>& vec) -> std::vector<std::complex<double>>;
	auto fft(const std::vector<std::complex<double>>& vec) -> std::vector<std::complex<double>>;
	auto testPricingfft() -> LogStrikePricePair;
	
	// defined in header because of template argument deduction
	auto pricingfft(std::string_view model, const auto& modelParams, const MarketParams& marketParams, const fttParams& params) -> LogStrikePricePair
	{

		// Parameters setting in fourier transform
		double decayParam{ params.decayParam };
		double gridWidth{ params.gridWidth };
		int gridExponent{ params.gridExponent };

		// get market parameters
		double maturity{ marketParams.maturity };
		double spot{ marketParams.spot };
		double riskFreeReturn{ marketParams.riskFreeReturn };

		int gridNum{ intPow(2,gridExponent) };
		// step - size in log strike space
		double gridWidthLogStrikeSpace{ (2. * PI / gridNum) / gridWidth };
		// smallest value in log strike space
		double lowestLogStrike{ std::log(spot) - gridNum * gridWidthLogStrikeSpace / 2. };


		// forming vector x and strikes km for m = 1, ..., N
		std::vector<double> logStrikes{};
		std::vector<std::complex<double>> xX{};

		// discount factor
		double discount{ std::exp(-riskFreeReturn * maturity) };


		for (std::size_t j{ 0 }; j < gridNum; ++j)
		{
			double nuJ{ j * gridWidth };
			logStrikes.push_back(lowestLogStrike + j * gridWidthLogStrikeSpace);

			// generalCF(double argument, std::string_view model, const auto & modelParams, const MarketParams & marketParams)
			std::complex<double> psi_nuJ{ discount * SDE::CharacteristicFunctions::generalCF(nuJ - (decayParam + 1) * IMNUM, model, modelParams, marketParams) / ((decayParam + IMNUM * nuJ) * (decayParam + 1. + IMNUM * nuJ)) };
			double weight{};
			if (j == 0)
			{
				weight = (gridWidth / 2.0);
			}
			else
			{
				weight = gridWidth;
			}

			xX.push_back(std::exp(-IMNUM * lowestLogStrike * nuJ) * psi_nuJ * weight);
		}

		// compute fft TODO
		std::vector<std::complex<double>> yY{ fft(xX) };
		// ----------------

		std::vector<double> prices{};
		for (std::size_t j{ 0 }; j < gridNum; ++j)
		{
			double multiplier{ std::exp(-decayParam * logStrikes[j]) / PI };
			prices.push_back(multiplier * std::real(yY[j]));
		}

		LogStrikePricePair result{ logStrikes,prices };

		return result;
	}
}

#endif
