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
			for (std::size_t n{ 0 }; n < length; ++n)
			{
				fourierTrafo[k] += vec[n] * std::exp(-2.0 * IMNUM * PI * static_cast<double>(k * n) / static_cast<double>(length));
			}
		}
		return fourierTrafo;
	}

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

	auto pricingfft(const auto& modelParams, const MarketParams& marketParams, const fttParams& params) -> LogStrikePricePair
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
			std::complex<double> psi_nuJ{ discount * SDE::CharacteristicFunctions::generalCF(nuJ - (decayParam + 1) * IMNUM, modelParams, marketParams) / ((decayParam + IMNUM * nuJ) * (decayParam + 1. + IMNUM * nuJ)) };
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

	auto pricingfftHeston(const HestonParams& modelParams, const MarketParams& marketParams, const fttParams& params) -> LogStrikePricePair
	{
		return pricingfft(modelParams, marketParams, params);
	}

	auto pricingfftBSM(const BSMParams& modelParams, const MarketParams& marketParams, const fttParams& params) -> LogStrikePricePair
	{
		return pricingfft(modelParams, marketParams, params);
	}



	namespace UnitTests
	{
		void separateModes()
		{

			// first we test the mode separation
			std::size_t length{ static_cast<std::size_t>(intPow(2,3)) };

			std::vector<std::complex<double>> terms(length);
			for (std::size_t i{ 0 }; i < length; ++i)
			{
				terms[i] = std::exp(-2.0 * IMNUM * PI * static_cast<double>(i) / static_cast<double>(length));
			}
			ModePair pair{ FFT::separateModes(terms) };
			std::cout << "Lenght of total vector is " << std::size(terms) << "\n";
			std::cout << "Lenght of first part is " << std::size(pair.evens) << "\n";
			std::cout << "Lenght of second part is " << std::size(pair.odds) << "\n";

			// print full vector and the the parts
			std::cout << "Elements of full vector are: \n";
			for (const auto& el : terms)
			{
				std::cout << el << ",";
			}
			std::cout << "\n";
			std::cout << "Eeven elements are: \n";
			for (const auto& el : pair.evens)
			{
				std::cout << el << ",";
			}
			std::cout << "\n";
			std::cout << "Odd elements are: \n";
			for (const auto& el : pair.odds)
			{
				std::cout << el << ",";
			}
			std::cout << "\n";

		}

		void dft()
		{
			std::size_t length{ static_cast<std::size_t>(intPow(2,6)) };

			std::vector<std::complex<double>> terms(length);
			for (std::size_t i{ 0 }; i < length; ++i)
			{
				//terms[i] = std::exp(-1.0 * IMNUM * PI * static_cast<double>(i) / static_cast<double>(length));
				terms[i] = 1.0;
			}

			std::vector<std::complex<double>> ft{ FFT::dft(terms) };

			std::cout << "Vector elements are: \n";
			for (const auto& el : terms)
			{
				std::cout << el << ",";
			}
			std::cout << "\n";
			std::cout << "DFT elements are: \n";
			for (const auto& el : ft)
			{
				std::cout << el << ",";
			}
			std::cout << "\n";
		}

		void pricingfft()
		{
			HestonParams hestonParams{};
			BSMParams bsmParams{};
			fttParams params{};
			MarketParams marketParams{};

			LogStrikePricePair pricePairs{ pricingfft(bsmParams, marketParams, params) };
			std::cout << "Log strikes are: \n";
			for (const auto& el : pricePairs.logStrikes)
			{
				std::cout << el << ",";
			}
			std::cout << "\n";

			std::cout << "Prices are:\n ";
			for (const auto& el : pricePairs.prices)
			{
				std::cout << el << ",";
			}
			std::cout << "\n";
		}

	}
	

}