#include "calibrate.h"
#include "options.h"
#include "saving.h"
#include "pso.h"
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

		std::vector<double> prices(lengthStrikes);
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

			double interpolatedPrice{ (1 - t) * fftPrices[index - 1] + t * fftPrices[index] };
			prices[i] = interpolatedPrice;
		}

		return prices;
	}

	auto computeFFTModelMRSE(const LabeledTable& priceSurface,
		MarketParams& marketParams,
		const auto& modelParams,
		const FFT::FFTParams& params,
		LabeledTable& modelPriceSurface,
		LabeledTable& errorSurface) -> double
	{

		double newError{ 0.0 };

		// add up errors of predicted prices
		// rows are maturities
		for (std::size_t row{ 0 }; row < std::size(priceSurface.m_rowVals); ++row)
		{
			// adjust maturity of market params
			marketParams.maturity = priceSurface.m_rowVals[row];

			std::vector<double> modelPrices(priceSurface.m_numCols);

			// get model prediction and interpolate to fit the query strikes (cols are strikes)
			FFT::LogStrikePricePair pair{ FFT::pricingfft(modelParams, marketParams, params) };
			modelPrices = interpolatePrices(pair, priceSurface.m_colVals);

			// cols are strikes
			for (std::size_t col{ 0 }; col < std::size(priceSurface.m_colVals); ++col)
			{
				// add up the errors
				double currentError{ (modelPrices[col] - priceSurface.m_table[row][col]) * (modelPrices[col] - priceSurface.m_table[row][col])
												/ (priceSurface.m_table[row][col] * priceSurface.m_table[row][col]) };
				newError += currentError;
				// update model price surface
				modelPriceSurface.m_table[row][col] = modelPrices[col];
				errorSurface.m_table[row][col] = currentError;
			}
		}
		// get mean error over all entries
		newError /= (priceSurface.m_numCols * priceSurface.m_numRows);

		return newError;
	}

	auto computeBSM_MRSE(const LabeledTable& priceSurface,
		MarketParams& marketParams,
		const BSMParams& modelParams,
		LabeledTable& modelPriceSurface,
		LabeledTable& errorSurface) -> double
	{

		double newError{ 0.0 };

		// add up errors of predicted prices
		// rows are maturities
		for (std::size_t row{ 0 }; row < std::size(priceSurface.m_rowVals); ++row)
		{
			// adjust maturity of market params
			marketParams.maturity = priceSurface.m_rowVals[row];

			std::vector<double> modelPrices(priceSurface.m_numCols);

			// get model prediction and interpolate to fit the query strikes (cols are strikes)
			for (std::size_t i{ 0 }; i < priceSurface.m_numCols; ++i)
			{
				modelPrices[i] = Options::Pricing::BSM::call(marketParams.riskFreeReturn, modelParams.vol,
					priceSurface.m_rowVals[row], priceSurface.m_colVals[i],
					marketParams.spot, marketParams.dividendYield);
			}

			// cols are strikes
			for (std::size_t col{ 0 }; col < std::size(priceSurface.m_colVals); ++col)
			{
				// add up the errors
				double currentError{ (modelPrices[col] - priceSurface.m_table[row][col]) * (modelPrices[col] - priceSurface.m_table[row][col])
												/ (priceSurface.m_table[row][col] * priceSurface.m_table[row][col]) };
				newError += currentError;
				// update model price surface
				modelPriceSurface.m_table[row][col] = modelPrices[col];
				errorSurface.m_table[row][col] = currentError;
			}
		}
		// get mean error over all entries
		newError /= (priceSurface.m_numCols * priceSurface.m_numRows);
		return newError;

	}


	auto hestonCall(const LabeledTable& priceSurface, double riskFreeReturn, double spot, double dividendYield) -> HestonParams
	{

		// Since we need to populate the surface of model generated prices anyway to compute the model error,
		// we store this surface (initialized as a copy of the true surface) 
		// to be able to plot it later (and compare it to the true surface)
		LabeledTable modelPriceSurface{ priceSurface };
		modelPriceSurface.m_tableName = "Heston price surface";
		LabeledTable errorSurface{ priceSurface };
		errorSurface.m_tableName = "Relative squared error";
		errorSurface.m_tableLabel = "Error";

		std::vector<double> reversionRates{ np::linspace<double>(0.01,2.0,2) };
		std::vector<double> longVariances{ np::linspace<double>(0.1,2.0,2) };
		std::vector<double> volVols{ np::linspace<double>(0.1,2.0,2) };
		std::vector<double> correlations{ np::linspace<double>(-0.9,0.9,2) };
		std::vector<double> initialVariances{ np::linspace<double>(0.1,2.0,2) };
		

		/*
		std::vector<double> reversionRates{ np::linspace<double>(0.2,0.2,1) };
		std::vector<double> longVariances{ np::linspace<double>(0.62,0.62,1) };
		std::vector<double> volVols{ np::linspace<double>(6.64,6.64,1) };
		std::vector<double> correlations{ np::linspace<double>(-0.15,-0.15,1) };
		std::vector<double> initialVariances{ np::linspace<double>(0.62,0.62,1) };
		*/

		HestonParams finalParams{
			reversionRates[static_cast<std::size_t>(0)],
			longVariances[static_cast<std::size_t>(0)],
			volVols[static_cast<std::size_t>(0)],
			correlations[static_cast<std::size_t>(0)],
			initialVariances[static_cast<std::size_t>(0)],
		};

		// define params of FFT pricing method
		FFT::FFTParams params{};

		// the maturity of the market variable will change later, the other values are fixed
		MarketParams marketParams{ 1.0,spot,riskFreeReturn,dividendYield };

		double error{ 1e20 };
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
							// collect model parameters, compute error and update surfaces
							HestonParams modelParams{rr,lv,vv,cr,iv};
							double newError{ computeFFTModelMRSE(priceSurface, marketParams, modelParams, params, modelPriceSurface, errorSurface) };

							if (newError < error)
							{
								error = newError;
								finalParams = modelParams;
								std::cout << "New optimal parameter set with mean relative squared error " << error << " found.\n";
							}

						}
					}
				}
			}
		}
		std::cout << "Final parameter set has error " << error << ".\n";

		// save the model price table to file
		Saving::write_labeledTable_to_csv("Data/HestonModelPriceSurface.csv", modelPriceSurface);
		Saving::write_labeledTable_to_csv("Data/HestonModelErrorSurface.csv", errorSurface);

		return finalParams;
	}

	// NOTE: we use FFT but we could replace it with the explicit pricing formula
	auto bsmCall(const LabeledTable& priceSurface, double riskFreeReturn, double spot, double dividendYield, std::string_view pricing) -> BSMParams
	{
		// Since we need to populate the surface of model generated prices anyway to compute the model error,
		// we store this surface (initialized as a copy of the true surface) 
		// to be able to plot it later (and compare it to the true surface)
		LabeledTable modelPriceSurface{ priceSurface };
		modelPriceSurface.m_tableName = "BSM price surface";
		LabeledTable errorSurface{ priceSurface };
		errorSurface.m_tableName = "Relative squared error";
		errorSurface.m_tableLabel = "Error";

		std::vector<double> vols{ np::linspace<double>(0.5,.7,100) };

		BSMParams finalParams{
			vols[static_cast<std::size_t>(0)]
		};

		// define params of FFT pricing method
		FFT::FFTParams params{};

		// the maturity of the market variable will change later, the other values are fixed
		MarketParams marketParams{ 1.0,spot,riskFreeReturn,dividendYield };

		double error{ 1e20 };
		for (const auto& vol : vols)
		{
			// collect model parameters, compute error and update surfaces
			BSMParams modelParams{ vol };
			double newError{};
			if (pricing == "fft")
			{
				newError = computeFFTModelMRSE(priceSurface, marketParams, modelParams, params, modelPriceSurface, errorSurface);
			}
			else
			{
				newError = computeBSM_MRSE(priceSurface, marketParams, modelParams, modelPriceSurface, errorSurface);
			}

			if (newError < error)
			{
				error = newError;
				finalParams = modelParams;
				std::cout << "New optimal parameter set with mean relative squared error " << error << " found.\n";
			}

		}
		std::cout << "Final parameter set has error " << error << ".\n";

		// save the model price table to file
		Saving::write_labeledTable_to_csv("Data/BSMModelPriceSurface.csv", modelPriceSurface);
		Saving::write_labeledTable_to_csv("Data/BSMModelErrorSurface.csv", errorSurface);

		return finalParams;
	}


	auto varianceGammaCall(const LabeledTable& priceSurface, double riskFreeReturn, double spot, double dividendYield) -> VarianceGammaParams
	{
		// Since we need to populate the surface of model generated prices anyway to compute the model error,
		// we store this surface (initialized as a copy of the true surface) 
		// to be able to plot it later (and compare it to the true surface)
		LabeledTable modelPriceSurface{ priceSurface };
		modelPriceSurface.m_tableName = "Variance Gamma price surface";
		LabeledTable errorSurface{ priceSurface };
		errorSurface.m_tableName = "Relative squared error";
		errorSurface.m_tableLabel = "Error";

		std::vector<double> vols{ np::linspace<double>(0.1,1.0,3) };
		std::vector<double> drifts{ np::linspace<double>(0.2,1.2,3) };
		std::vector<double> variances{ np::linspace<double>(0.01,1.0,3) };

		VarianceGammaParams finalParams{
			vols[static_cast<std::size_t>(0)],
			drifts[static_cast<std::size_t>(0)],
			variances[static_cast<std::size_t>(0)]
		};

		// define params of FFT pricing method
		FFT::FFTParams params{};

		// the maturity of the market variable will change later, the other values are fixed
		MarketParams marketParams{ 1.0,spot,riskFreeReturn,dividendYield };

		double error{ 1e20 };
		for (const auto& vol : vols)
		{
			for (const auto& drift : drifts)
			{
				for (const auto& variance : variances)
				{
					// collect model parameters, compute error and update surfaces
					VarianceGammaParams modelParams{ vol,drift,variance };
					double newError{ computeFFTModelMRSE(priceSurface, marketParams, modelParams, params, modelPriceSurface, errorSurface) };

					if (newError < error)
					{
						error = newError;
						finalParams = modelParams;
						std::cout << "New optimal parameter set with mean relative squared error " << error << " found.\n";
					}

				}

			}
		}

		std::cout << "Final parameter set has error " << error << ".\n";

		// save the model price table to file
		Saving::write_labeledTable_to_csv("Data/VGModelPriceSurface.csv", modelPriceSurface);
		Saving::write_labeledTable_to_csv("Data/VGModelErrorSurface.csv", errorSurface);

		return finalParams;
	}

	void testHeston()
	{
		// initialize the price table
		using namespace std::string_view_literals;
		LabeledTable priceSurface("Price surface"sv,
			"Time to maturity"sv,
			10,
			"Strikes"sv,
			16,
			"European call price"sv
		);

		priceSurface.m_table =
		{ {
		{ 24.75, 22.90, 21.10, 19.35, 17.65, 16.00, 14.40, 12.85, 11.35, 9.90, 8.50, 7.15, 5.85, 4.60, 3.40, 2.50 },
		{ 25.40, 23.65, 21.90, 20.15, 18.50, 16.90, 15.35, 13.85, 12.40, 11.00, 9.65, 8.35, 7.10, 5.90, 4.75, 3.70 },
		{ 26.20, 24.35, 22.60, 20.90, 19.25, 17.65, 16.10, 14.60, 13.15, 11.75, 10.40, 9.10, 7.85, 6.65, 5.50, 4.40 },
		{ 26.90, 25.05, 23.30, 21.60, 19.95, 18.35, 16.80, 15.30, 13.85, 12.45, 11.10, 9.80, 8.55, 7.35, 6.20, 5.10 },
		{ 27.65, 25.80, 24.05, 22.35, 20.70, 19.10, 17.55, 16.05, 14.60, 13.20, 11.85, 10.55, 9.30, 8.10, 6.95, 5.85 },
		{ 28.40, 26.55, 24.80, 23.10, 21.45, 19.85, 18.30, 16.80, 15.35, 13.95, 12.60, 11.30, 10.05, 8.85, 7.70, 6.60 },
		{ 29.15, 27.30, 25.55, 23.85, 22.20, 20.60, 19.05, 17.55, 16.10, 14.70, 13.35, 12.05, 10.80, 9.60, 8.45, 7.35 },
		{ 29.85, 28.00, 26.25, 24.55, 22.90, 21.30, 19.75, 18.25, 16.80, 15.40, 14.05, 12.75, 11.50, 10.30, 9.15, 8.05 },
		{ 30.50, 28.65, 26.90, 25.20, 23.55, 21.95, 20.40, 18.90, 17.45, 16.05, 14.70, 13.40, 12.15, 10.95, 9.80, 8.70 },
		{ 31.10, 29.25, 27.50, 25.80, 24.15, 22.55, 21.00, 19.50, 18.05, 16.65, 15.30, 14.00, 12.75, 11.55, 10.40, 9.30 }
		} };


		priceSurface.m_colVals = { 155, 157.5, 160, 162.5, 165,
								167.5, 170, 172.5, 175, 177.5, 180, 182.5,
								185, 187.5, 190, 192.5,
		};
		priceSurface.m_rowVals = { 1.0 / 52.,2.0 / 52. ,3.0 / 52. ,4.0 / 52. ,5.0 / 52. ,
									6.0 / 52. ,7.0 / 52. ,8.0 / 52. ,9.0 / 52., 10.0 / 52. };


		const double dividendYield = 0.007;
		const double spot{ 175.0 };
		const double riskFreeReturn{ 0.045 };

		HestonParams fitParams{ hestonCall(priceSurface,riskFreeReturn,spot,dividendYield) };

		std::cout << "The optimal params found are: \n";
		std::cout << "reversionRate: " << fitParams.reversionRate << "\n";
		std::cout << "longVariance: " << fitParams.longVariance << "\n";
		std::cout << "volVol: " << fitParams.volVol << "\n";
		std::cout << "correlation: " << fitParams.correlation << "\n";
		std::cout << "initialVariance: " << fitParams.initialVariance << "\n";
	}


	void testBSM()
	{
		// initialize the price table
		using namespace std::string_view_literals;
		LabeledTable priceSurface("Price surface"sv,
			"Time to maturity"sv,
			10,
			"Strikes"sv,
			16,
			"European call price"sv
		);

		priceSurface.m_table =
		{ {
		{ 24.75, 22.90, 21.10, 19.35, 17.65, 16.00, 14.40, 12.85, 11.35, 9.90, 8.50, 7.15, 5.85, 4.60, 3.40, 2.50 },
		{ 25.40, 23.65, 21.90, 20.15, 18.50, 16.90, 15.35, 13.85, 12.40, 11.00, 9.65, 8.35, 7.10, 5.90, 4.75, 3.70 },
		{ 26.20, 24.35, 22.60, 20.90, 19.25, 17.65, 16.10, 14.60, 13.15, 11.75, 10.40, 9.10, 7.85, 6.65, 5.50, 4.40 },
		{ 26.90, 25.05, 23.30, 21.60, 19.95, 18.35, 16.80, 15.30, 13.85, 12.45, 11.10, 9.80, 8.55, 7.35, 6.20, 5.10 },
		{ 27.65, 25.80, 24.05, 22.35, 20.70, 19.10, 17.55, 16.05, 14.60, 13.20, 11.85, 10.55, 9.30, 8.10, 6.95, 5.85 },
		{ 28.40, 26.55, 24.80, 23.10, 21.45, 19.85, 18.30, 16.80, 15.35, 13.95, 12.60, 11.30, 10.05, 8.85, 7.70, 6.60 },
		{ 29.15, 27.30, 25.55, 23.85, 22.20, 20.60, 19.05, 17.55, 16.10, 14.70, 13.35, 12.05, 10.80, 9.60, 8.45, 7.35 },
		{ 29.85, 28.00, 26.25, 24.55, 22.90, 21.30, 19.75, 18.25, 16.80, 15.40, 14.05, 12.75, 11.50, 10.30, 9.15, 8.05 },
		{ 30.50, 28.65, 26.90, 25.20, 23.55, 21.95, 20.40, 18.90, 17.45, 16.05, 14.70, 13.40, 12.15, 10.95, 9.80, 8.70 },
		{ 31.10, 29.25, 27.50, 25.80, 24.15, 22.55, 21.00, 19.50, 18.05, 16.65, 15.30, 14.00, 12.75, 11.55, 10.40, 9.30 }
		} };


		priceSurface.m_colVals = { 155, 157.5, 160, 162.5, 165,
								167.5, 170, 172.5, 175, 177.5, 180, 182.5,
								185, 187.5, 190, 192.5,
		};
		priceSurface.m_rowVals = { 1.0 / 52.,2.0 / 52. ,3.0 / 52. ,4.0 / 52. ,5.0 / 52. ,
									6.0 / 52. ,7.0 / 52. ,8.0 / 52. ,9.0 / 52., 10.0 / 52. };


		const double dividendYield = 0.007;
		const double spot{ 175.0 };
		const double riskFreeReturn{ 0.045 };

		BSMParams fitParams{ bsmCall(priceSurface,riskFreeReturn,spot,dividendYield)};

		std::cout << "The optimal params found with analytic pricing are: \n";
		std::cout << "Vol: " << fitParams.vol << "\n";

		// test FFT
		/*
		fitParams = bsmCall(priceSurface, riskFreeReturn, spot, dividendYield, "fft");

		std::cout << "The optimal params found with FFT pricing are: \n";
		std::cout << "Vol: " << fitParams.vol << "\n";
		*/
	}


	void testVarianceGamma()
	{
		// initialize the price table
		using namespace std::string_view_literals;
		LabeledTable priceSurface("Price surface"sv,
			"Time to maturity"sv,
			10,
			"Strikes"sv,
			16,
			"European call price"sv
		);

		priceSurface.m_table =
		{ {
		{ 24.75, 22.90, 21.10, 19.35, 17.65, 16.00, 14.40, 12.85, 11.35, 9.90, 8.50, 7.15, 5.85, 4.60, 3.40, 2.50 },
		{ 25.40, 23.65, 21.90, 20.15, 18.50, 16.90, 15.35, 13.85, 12.40, 11.00, 9.65, 8.35, 7.10, 5.90, 4.75, 3.70 },
		{ 26.20, 24.35, 22.60, 20.90, 19.25, 17.65, 16.10, 14.60, 13.15, 11.75, 10.40, 9.10, 7.85, 6.65, 5.50, 4.40 },
		{ 26.90, 25.05, 23.30, 21.60, 19.95, 18.35, 16.80, 15.30, 13.85, 12.45, 11.10, 9.80, 8.55, 7.35, 6.20, 5.10 },
		{ 27.65, 25.80, 24.05, 22.35, 20.70, 19.10, 17.55, 16.05, 14.60, 13.20, 11.85, 10.55, 9.30, 8.10, 6.95, 5.85 },
		{ 28.40, 26.55, 24.80, 23.10, 21.45, 19.85, 18.30, 16.80, 15.35, 13.95, 12.60, 11.30, 10.05, 8.85, 7.70, 6.60 },
		{ 29.15, 27.30, 25.55, 23.85, 22.20, 20.60, 19.05, 17.55, 16.10, 14.70, 13.35, 12.05, 10.80, 9.60, 8.45, 7.35 },
		{ 29.85, 28.00, 26.25, 24.55, 22.90, 21.30, 19.75, 18.25, 16.80, 15.40, 14.05, 12.75, 11.50, 10.30, 9.15, 8.05 },
		{ 30.50, 28.65, 26.90, 25.20, 23.55, 21.95, 20.40, 18.90, 17.45, 16.05, 14.70, 13.40, 12.15, 10.95, 9.80, 8.70 },
		{ 31.10, 29.25, 27.50, 25.80, 24.15, 22.55, 21.00, 19.50, 18.05, 16.65, 15.30, 14.00, 12.75, 11.55, 10.40, 9.30 }
		} };


		priceSurface.m_colVals = { 155, 157.5, 160, 162.5, 165,
								167.5, 170, 172.5, 175, 177.5, 180, 182.5,
								185, 187.5, 190, 192.5,
		};
		priceSurface.m_rowVals = { 1.0 / 52.,2.0 / 52. ,3.0 / 52. ,4.0 / 52. ,5.0 / 52. ,
									6.0 / 52. ,7.0 / 52. ,8.0 / 52. ,9.0 / 52., 10.0 / 52. };


		const double dividendYield = 0.007;
		const double spot{ 175.0 };
		const double riskFreeReturn{ 0.045 };

		VarianceGammaParams fitParams{ varianceGammaCall(priceSurface,riskFreeReturn,spot,dividendYield) };

		std::cout << "The optimal params found are: \n";
		std::cout << "Vol: " << fitParams.vol << "\n";
		std::cout << "Drift: " << fitParams.drift << "\n";
		std::cout << "Variance: " << fitParams.variance << "\n";
	}

}