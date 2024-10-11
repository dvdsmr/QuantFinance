#include "fft.h"
#include <vector>
#include <string_view>
#include <cmath>
#include <complex>
#include <cassert>


namespace FFT
{

	auto separateModes(const std::vector<std::complex<double>>& vec) -> ModePair
	{
		assert(!vec.empty());
		std::size_t length{ std::size(vec) };
		ModePair pair(length);


		for (std::size_t i{ 0 }, j{ 0 }; i < (length - 1); i += 2, ++j) {
			pair.evens[j] = (vec[i]);
			pair.odds[j] = (vec[i + 1]);
		}

		if (vec.size() % 2)
		{
			pair.evens.push_back(vec.back());
		}
		return pair;
	}

	auto intPow(int base, int exponent) -> int 
	{
		int result = 1;
		for (int i = 0; i < exponent; i++) {
			result *= base;
		}
		return result;
	}

	template <typename T>
	auto concatenate(std::vector<T>& vec1, const std::vector<T>& vec2) -> std::vector<T>
	{
		std::vector<T> result(std::begin(vec1), std::end(vec1));
		result.insert(std::end(result), std::begin(vec2), std::end(vec2));
		return result;
	}

	auto dft(const std::vector<std::complex<double>>& vec) -> std::vector<std::complex<double>>
	{
		// discrete ft, inefficient for larger vectors and only to be used in "leaves" of fft
		std::size_t length{ std::size(vec) };
		std::vector<std::complex<double>> fourierTrafo(length);
		for (std::size_t k{ 0 }; k < length; ++k)
		{
			for (std::size_t n{ 0 }; k < length; ++n)
			{
				fourierTrafo[k] += vec[n] * std::exp(-2.0 * IMNUM * PI * static_cast<double>(k * n) / static_cast<double>(length));
			}
		}
		return fourierTrafo;
	}

	// TODO: replace with actual fft
	auto fft(const std::vector<std::complex<double>>& vec) -> std::vector<std::complex<double>>
	{
		
		std::size_t length{ std::size(vec) };
		assert(length % 2 == 0); // fft only works for arrays with length which is a power of 2

		// build vector of factors for current length
		std::vector<std::complex<double>> terms(length);
		for (std::size_t i{ 0 }; i < length; ++i)
		{
			terms[i] = std::exp(-2.0 * IMNUM * PI * static_cast<double>(i) / static_cast<double>(length));
		}

		if (length <= static_cast<std::size_t>(2))
		{
			return dft(vec);
		}
		else
		{
			ModePair modePair{ separateModes(vec) };
			std::vector<std::complex<double>> evens{ fft(modePair.evens) };
			std::vector<std::complex<double>> odds{ fft(modePair.odds) };


			std::vector<std::complex<double>> firstPart(length / 2);
			std::vector<std::complex<double>> secondPart(length / 2);
			for (std::size_t i{ 0 }; i < length / 2; ++i)
			{
				firstPart[i] = evens[i] + terms[i] * odds[i];
				secondPart[i] = evens[i] + terms[i + length / 2] * odds[i];
			}

			return concatenate(firstPart, secondPart);
		}

	}

	
	auto testPricingfft() -> LogStrikePricePair
	{
		HestonParams hestonParams{};
		fttParams params{};
		MarketParams marketParams{};

		return pricingfft("heston", hestonParams, marketParams, params);
	}
	

}