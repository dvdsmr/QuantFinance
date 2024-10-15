#include "calibrate.h"
#include <algorithm>
#include <cassert>

namespace Calibrate
{

	auto interpolatePrices(const FFT::LogStrikePricePair& pair, const std::vector<double>& strikes) -> std::vector<double>
	{
		std::size_t lengthFftStrikes{ std::size(pair.logStrikes) };
		std::size_t lengthStrikes{ std::size(strikes) };

		// first, we get the actual strikes from the log strikes
		std::vector<double> fftStrikes(lengthFftStrikes);
		for (std::size_t i{ 0 }; i < lengthFftStrikes; ++i)
		{
			fftStrikes[i] = std::exp(pair.logStrikes[i]);
		}

		std::vector<double> prices{};
		std::vector<double> fftPrices{ pair.prices };
		// now we interpolate between the prices computed by FFT
		for (std::size_t i{ 0 }; i < lengthStrikes; ++i)
		{
			double queryStrike{ strikes[i] };
			// we search for the first FFT strike which is greater than the query strike
			auto found{ std::find_if(std::begin(fftStrikes), std::end(fftStrikes),
						   [&queryStrike](double val)
						   {
							 return val > queryStrike;
						   }) };

			// we have to make sure that the queried price is lower that the last FFT price...
			assert(found != std::end(fftStrikes));
			// ...and higher than the first FFT price.
			// we don't want to extrapolate, only interpolate. Otherwise the program terminates here.
			std::size_t index = static_cast<std::size_t>(found - std::begin(fftStrikes));
			assert(index > 0);

			// If the program did not terminate, we now have the index of the lower and higher prices
			// now, we can interpolate these prices to get the price at the query strike.
			double t{ (queryStrike - fftStrikes[index - 1]) / (fftStrikes[index] - fftStrikes[index - 1]) };

			double interpolatedPrice{ (1-t)*fftPrices[index-1] + t * fftPrices[index] };
			prices[i] = interpolatedPrice;
		}

		return prices;
	}

	auto hestonTest(const LabeledTable& priceSurface, double riskFreeReturn, double spot, double dividendYield, std::string_view, std::string_view optimizer) -> HestonParams
	{


		std::vector<double> reversionRates{ np::linspace<double>(0.1,0.5,5) };
		std::vector<double> longVariances{ np::linspace<double>(10.,20.,5) };
		std::vector<double> volVols{ np::linspace<double>(0.1,0.5,5) };
		std::vector<double> correlations{ np::linspace<double>(0.1,0.5,5) };
		std::vector<double> initialVariances{ np::linspace<double>(1.,10.,5) };

		HestonParams finalParams{
			reversionRates[static_cast<std::size_t>(0)],
			longVariances[static_cast<std::size_t>(0)],
			volVols[static_cast<std::size_t>(0)],
			correlations[static_cast<std::size_t>(0)],
			initialVariances[static_cast<std::size_t>(0)],
		};

		// define params of FFT pricing method
		FFT::fttParams params{};

		// the maturity of the market variable will change later, the other values are fixed
		MarketParams marketParams{ 1.0,spot,riskFreeReturn,dividendYield };

		[[maybe_unused]]double error{};
		[[maybe_unused]] double newError{};
		for (const auto& rr : reversionRates)
		{
			for (const auto& lv : longVariances)
			{
				for (const auto& vv : volVols)
				{
					for (const auto& cr : correlations)
					{
						for (const auto& iv : initialVariances)
						{
							// collect model parameters
							HestonParams modelParams{rr,lv,vv,cr,iv};

							// add up errors of predicted prices
							// rows are maturities
							for (std::size_t row{ 0 }; row < std::size(priceSurface.m_rowVals); ++row)
							{
								// adjust maturity of market params
								marketParams.maturity = priceSurface.m_rowVals[row];

								FFT::LogStrikePricePair pair{ FFT::pricingfftHeston(modelParams, marketParams, params) };
								// cols are strikes
								for (std::size_t col{ 0 }; col < std::size(priceSurface.m_colVals); ++col)
								{
									rr;
									lv;
									vv;
									cr;
									iv;
									row;
									col;
									optimizer;
									dividendYield;
									spot;
									riskFreeReturn;
								}
							}

						}
					}
				}
			}
		}


		return finalParams;
	}
}