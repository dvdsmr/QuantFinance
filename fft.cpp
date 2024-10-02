#include "fft.h"
#include <vector>
#include <string_view>
#include <cmath>

constexpr double PI = 3.14159265358979323846;

namespace FFT
{

	struct fttParams
	{
		double decayParam{ 1.5 };
		double gridWidth{ 0.2 };
		int gridExponent { 12 };
	};

	auto intPow(int base, int exponent) -> int 
	{
		int result = 1;
		for (int i = 0; i < exponent; i++) {
			result *= base;
		}
		return result;
	}

	auto fft(std::string_view model, std::vector<double> modelParams, std::vector<double> marketParams, const fttParams& params) -> std::vector<double>
	{

		// Parameters setting in fourier transform
		[[maybe_unused]] std::string_view modelName{ model };
		[[maybe_unused]] double decayParam{ params.decayParam };
		[[maybe_unused]] double gridWidth{ params.gridWidth };
		[[maybe_unused]] int gridExponent{ params.gridExponent };

		[[maybe_unused]] int gridNum{ intPow(2,gridExponent)};
		// step - size in log strike space
		[[maybe_unused]] double gridWidthLogStrikeSpace{ (2 * PI / gridNum) / gridWidth };

		// Choice of beta
		//double lowestLogStrike{ std::log(S0) - gridNum * gridWidthLogStrikeSpace / 2 };

		// under construction
		std::vector<double> prices{};

		return prices;
	}

	void fft(const auto& model, const std::vector<double>& params) 
	{
		std::vector<double> params{ 1.5,0.2,12 };
	}

}