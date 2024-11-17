#include "numpy.h"
#include "xyvals.h"
#include "options.h"
#include "adam.h"
#include "volatility.h"
#include <string>
#include <string_view>


namespace Volatility
{
	namespace Surface
	{
		//	UNDER CONSTRUCTION
		auto bsm(const LabeledTable& priceSurface, double riskFreeReturn, double spot, double dividendYield, std::string_view type, std::string_view optimizer) -> LabeledTable
		{
			// get price function type call/put
			auto bsmPrice
			{
				[&](double riskFreeReturn, double vol, double maturity, double strike, double spot, double dividendYield) {
					if (type == "call")
					{
						return Options::Pricing::BSM::call(riskFreeReturn, vol, maturity, strike, spot, dividendYield);
					}
					else
					{
						return Options::Pricing::BSM::put(riskFreeReturn, vol, maturity, strike, spot, dividendYield);
					}
				}
			};

			// get Vega of price option type. Note that this is the same in BSM model.
			auto bsmVega
			{
				[&](double riskFreeReturn, double vol, double maturity, double strike, double spot, double dividendYield) {
					if (type == "call")
					{
						return Options::Pricing::BSM::callVega(riskFreeReturn, vol, maturity, strike, spot, dividendYield);
					}
					else
					{
						return Options::Pricing::BSM::putVega(riskFreeReturn, vol, maturity, strike, spot, dividendYield);
					}
				}
			};

			// initialize the price table
			using namespace std::string_view_literals;
			LabeledTable volSurface("BSM volatility surface",
				priceSurface.m_rowLabel,
				priceSurface.m_numRows,
				priceSurface.m_colLabel,
				priceSurface.m_numCols,
				"Implied volatility"sv
			);

			// define strikes and maturities
			volSurface.m_rowVals = priceSurface.m_rowVals; // time to maturity
			volSurface.m_colVals = priceSurface.m_colVals; // strike

			// calibrate the vol surface. 
			if (optimizer == "adam")
			{
				double volGuess{ 0.4 };
				Adam adam{}; 
				for (std::size_t row{ 0 }; row < std::size(volSurface.m_rowVals); ++row)
				{
					adam.set_state(volGuess); // initialize Adam with a guess of the volatility
					for (std::size_t col{ 0 }; col < std::size(volSurface.m_colVals); ++col)
					{
						double truePrice{ priceSurface.m_table[row][col] };

						// define adam target function and derivative
						auto func
						{
							[&](double vol) {
								double price{ bsmPrice(riskFreeReturn, vol, priceSurface.m_rowVals[row], priceSurface.m_colVals[col], spot, dividendYield) };
								return (price - truePrice) * (price - truePrice);
							}
						};
						auto deriv
						{
							[&](double vol) {
								double price{ bsmPrice(riskFreeReturn, vol, priceSurface.m_rowVals[row], priceSurface.m_colVals[col], spot, dividendYield) };
								return 2 * (price - truePrice) * bsmVega(riskFreeReturn, vol, priceSurface.m_rowVals[row], priceSurface.m_colVals[col], spot, dividendYield);
							}
						};

						// Note that adam keeps the optimized vol as its state, so each iteration
						// starts with the previously calibrated vol
						volSurface.m_table[row][col] = adam.optimize(func, deriv, true);

						// once we have iterated through all strikes for one maturity, we want
						// to start the optimizer with the vol previously calibrated for the lowest strike
						//if (col == static_cast<std::size_t>(0))
						//{
						//	volGuess = volSurface.m_table[row][col];
						//}

					}
				}
			}

			if (optimizer == "bruteForce")
			{
				std::vector<double> vols{ np::linspace<double>(0.05,5.0,100000)};
				for (std::size_t row{ 0 }; row < std::size(volSurface.m_rowVals); ++row)
				{
					for (std::size_t col{ 0 }; col < std::size(volSurface.m_colVals); ++col)
					{
						double error{};
						double newError{};
						double volBrute{ vols[static_cast<std::size_t>(0)] };
						double truePrice{ priceSurface.m_table[row][col] };
						double price{ bsmPrice(riskFreeReturn, volBrute, priceSurface.m_rowVals[row], priceSurface.m_colVals[col], spot, dividendYield) };
						error = (truePrice - price) * (truePrice - price);

						for (const auto& vol : vols)
						{
							price = bsmPrice(riskFreeReturn, vol, priceSurface.m_rowVals[row], priceSurface.m_colVals[col], spot, dividendYield);
							newError = (truePrice - price) * (truePrice - price);
							if (newError < error)
							{
								error = newError;
								volBrute = vol;
							}
						}
						std::cout << "Brute force approach found vol " << volBrute << " with error " << error << ".\n";

						volSurface.m_table[row][col] = volBrute;

					}
				}
			}

			return volSurface;
		}

		auto testCalibration() -> LabeledTable
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

			/*
			priceSurface.m_table =
			{ {
			{ 28.50, 26.60, 24.75, 22.90, 21.10, 19.35, 17.65, 16.00, 14.40, 12.85, 11.35, 9.90, 8.50, 7.15, 5.85, 4.60, 3.40, 2.50, 1.80, 1.20, 0.80 },
			{ 29.00, 27.25, 25.40, 23.65, 21.90, 20.15, 18.50, 16.90, 15.35, 13.85, 12.40, 11.00, 9.65, 8.35, 7.10, 5.90, 4.75, 3.70, 2.70, 1.85, 1.25 },
			{ 29.75, 28.10, 26.20, 24.35, 22.60, 20.90, 19.25, 17.65, 16.10, 14.60, 13.15, 11.75, 10.40, 9.10, 7.85, 6.65, 5.50, 4.40, 3.35, 2.40, 1.70 },
			{ 30.50, 28.80, 26.90, 25.05, 23.30, 21.60, 19.95, 18.35, 16.80, 15.30, 13.85, 12.45, 11.10, 9.80, 8.55, 7.35, 6.20, 5.10, 4.05, 3.10, 2.30 },
			{ 31.25, 29.55, 27.65, 25.80, 24.05, 22.35, 20.70, 19.10, 17.55, 16.05, 14.60, 13.20, 11.85, 10.55, 9.30, 8.10, 6.95, 5.85, 4.80, 3.85, 3.00 },
			{ 32.00, 30.30, 28.40, 26.55, 24.80, 23.10, 21.45, 19.85, 18.30, 16.80, 15.35, 13.95, 12.60, 11.30, 10.05, 8.85, 7.70, 6.60, 5.55, 4.60, 3.75 },
			{ 32.75, 31.05, 29.15, 27.30, 25.55, 23.85, 22.20, 20.60, 19.05, 17.55, 16.10, 14.70, 13.35, 12.05, 10.80, 9.60, 8.45, 7.35, 6.30, 5.35, 4.50 },
			{ 33.40, 31.75, 29.85, 28.00, 26.25, 24.55, 22.90, 21.30, 19.75, 18.25, 16.80, 15.40, 14.05, 12.75, 11.50, 10.30, 9.15, 8.05, 7.00, 6.05, 5.20 },
			{ 34.00, 32.40, 30.50, 28.65, 26.90, 25.20, 23.55, 21.95, 20.40, 18.90, 17.45, 16.05, 14.70, 13.40, 12.15, 10.95, 9.80, 8.70, 7.65, 6.70, 5.85 },
			{ 34.60, 33.00, 31.10, 29.25, 27.50, 25.80, 24.15, 22.55, 21.00, 19.50, 18.05, 16.65, 15.30, 14.00, 12.75, 11.55, 10.40, 9.30, 8.25, 7.30, 6.45 }
			} };	
			*/

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

			Saving::write_labeledTable_to_csv("Data/ArtificialPriceSurface.csv", priceSurface);
			
			
			const double dividendYield = 0.007;
			const double spot{ 175.0 };
			const double riskFreeReturn{ 0.045 };
			LabeledTable volSurface{ bsm(priceSurface, riskFreeReturn, spot, dividendYield, "call", "adam")};

			return volSurface;
		
		}

		// needs cleanup
		auto sanityCheck() -> void
		{
			double strike{ 190. };
			double maturity{ 1. * 151 / 365 };
			double dividendYield{ 0.005 };
			double riskFreeReturn{ 0.0245 };
			double spot{ 190.3 };
			double truePriceCall{ 10.875 };
			double truePricePut{ 9.625 };

			// define adam target function and derivative
			auto func
			{
				[&](double vol) {
					double price{ Options::Pricing::BSM::call(riskFreeReturn, vol, maturity, strike, spot, dividendYield) };
					return (price - truePriceCall) * (price - truePriceCall);
				}
			};
			auto deriv
			{
				[&](double vol) {
					double price{ Options::Pricing::BSM::call(riskFreeReturn, vol, maturity, strike, spot, dividendYield) };
					return 2 * (price - truePriceCall) * Options::Pricing::BSM::callVega(riskFreeReturn, vol, maturity, strike, spot, dividendYield);
				}
			};
			Adam adam{};
			adam.set_state(0.1);
			double volOpt{ adam.optimize(func, deriv, true) };

			std::cout << "Found vol of " << volOpt << " for the call option.\n";
			std::cout << "the true BSM implied vol is " << 0.205054798 << ".\n";

			// define adam target function and derivative
			auto funcPut
			{
				[&](double vol) {
					double price{ Options::Pricing::BSM::put(riskFreeReturn, vol, maturity, strike, spot, dividendYield) };
					return (price - truePricePut) * (price - truePricePut);
				}
			};
			auto derivPut
			{
				[&](double vol) {
					double price{ Options::Pricing::BSM::put(riskFreeReturn, vol, maturity, strike, spot, dividendYield) };
					return 2 * (price - truePricePut) * Options::Pricing::BSM::putVega(riskFreeReturn, vol, maturity, strike, spot, dividendYield);
				}
			};

			adam.set_state(0.1);
			volOpt = adam.optimize(funcPut, derivPut, false);

			std::cout << "Found vol of " << volOpt << " for the put option.\n";
			std::cout << "the true BSM implied vol is " << 0.216923754 << ".\n";
		}

		void calibrateToRealData(std::string symbol, double spot)
		{
			// We read in options data from yahoo finance
			std::string filename = "Data/yFinance/"+symbol+"_callPriceSurface.csv"; // Replace with your file name
			std::vector<std::vector<std::string>> csvData = Reading::readCSV(filename);

			// Output the CSV data
			for (const auto& row : csvData) {
				for (const auto& cell : row) {
					std::cout << cell << " ";
				}
				std::cout << std::endl;
			}

			// we try to estimate the implied Vol with the BSM model. Market params are (for the moment) set to
			[[maybe_unused]] double riskFreeReturn{ 0.001 }; // reported to be close to the rate used by yahoo finance
			[[maybe_unused]] double dividendYield{ 0.0 };
			//[[maybe_unused]] double spot{ 61.27 };

			// calibrate implied vol
			[[maybe_unused]] double volGuess{ 0.4 };
			Adam adam{};
			for (std::size_t row{ 1 }; row < std::size(csvData); ++row)
			{
				// index for mat is 1
				// index for price is 5 for mid, or 7 for last traded
				// index for strike is 2
				double truePrice{ std::stod(csvData[row][5]) };
				double mat{ std::stod(csvData[row][1]) };
				double strike{ std::stod(csvData[row][2]) };
				// define adam target function and derivative
				auto func
				{
					[&](double vol) {
						double price{ Options::Pricing::BSM::call(riskFreeReturn, vol, mat, strike, spot, dividendYield) };
						return (price - truePrice) * (price - truePrice);
					}
				};
				auto deriv
				{
					[&](double vol) {
						double price{ Options::Pricing::BSM::call(riskFreeReturn, vol, mat, strike, spot, dividendYield) };
						return 2 * (price - truePrice) * Options::Pricing::BSM::callVega(riskFreeReturn, vol, mat, strike, spot, dividendYield);
					}
				};

				adam.set_state(volGuess);
				// adam.set_state(std::stod(csvData[row][6])); // use yahoo finance quoted implied vol as initial value
				[[maybe_unused]] double calVol{ adam.optimize(func, deriv, false) };
				std::cout << "Maturity is " << csvData[row][1] << ".\n";
				std::cout << "Yahoo implied vol is " << csvData[row][6] << ". " << "BSM calibrated implied vol is " << calVol << ".\n";
				std::cout << "The real price is " << truePrice << ". " << "The BSM prediction has error " << func(calVol) << ".\n";
				std::cout << "Using the yfinance implied vol, BSM yields the price " << Options::Pricing::BSM::call(riskFreeReturn, std::stod(csvData[row][6]), mat, strike, spot, dividendYield) << ".\n\n";

				// PSO instead of adam
				// define pso on 
				PSO pso{ 1000,1 };
				pso.set_uniformRandomPositions({ 0.1 }, { 0.9 });
				pso.set_uniformRandomVelocities({ 0.1 }, { 0.9 });

				auto func2
				{
					[&](std::vector<double> vol) 
					{
						double price{ Options::Pricing::BSM::call(riskFreeReturn, vol[static_cast<std::size_t>(0)], mat, strike, spot, dividendYield) };
						return (price - truePrice) * (price - truePrice);
					}
				};

				std::vector<double> optVol{ pso.optimize(func2,false) };
				std::cout << "PSO found vol of " << optVol[static_cast<std::size_t>(0)] << " with error " << func2(optVol) << ".\n\n";

			}
		}


	}
}