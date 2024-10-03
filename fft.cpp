#include "fft.h"
#include "sdes.h"
#include <vector>
#include <string_view>
#include <cmath>

constexpr double PI = 3.14159265358979323846;

namespace FFT
{

	auto intPow(int base, int exponent) -> int 
	{
		int result = 1;
		for (int i = 0; i < exponent; i++) {
			result *= base;
		}
		return result;
	}

	// TODO: replace with actual fft
	auto fft(std::vector<std::complex<double>> vec) -> std::vector<std::complex<double>>
	{
		std::vector<std::complex<double>> fourier(std::size(vec));
		return fourier;
	}

	auto pricingfft(std::string_view model, const auto& modelParams, const MarketParams& marketParams, const fttParams& params) -> LogStrikePricePair
	{

		// Parameters setting in fourier transform
		[[maybe_unused]] std::string_view modelName{ model };
		[[maybe_unused]] double decayParam{ params.decayParam };
		[[maybe_unused]] double gridWidth{ params.gridWidth };
		[[maybe_unused]] int gridExponent{ params.gridExponent };

		// get market parameters
		double maturity{ marketParams.maturity };
		double spot{ marketParams.spot };
		double riskFreeReturn{ marketParams.riskFreeReturn };
		double dividendYield{ marketParams.dividendYield };

		[[maybe_unused]] int gridNum{ intPow(2,gridExponent)};
		// step - size in log strike space
		[[maybe_unused]] double gridWidthLogStrikeSpace{ (2. * PI / gridNum) / gridWidth };
		// smallest value in log strike space
		[[maybe_unused]] double lowestLogStrike{ std::log(spot) - gridNum * gridWidthLogStrikeSpace / 2. };


		// Choice of beta
		//double lowestLogStrike{ std::log(S0) - gridNum * gridWidthLogStrikeSpace / 2 };

		// under construction
		std::vector<double> prices{};

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
		std::vector<std::complex<double>> yY{fft(xX)};
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