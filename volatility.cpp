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
				21,
				"European call price"sv
			);

			/*
			priceSurface.m_table =
			{ { {25.40,	22.85,	20.25,	18.40,	16.60,	14.85,	13.15,	11.50,	9.90,	8.35,	6.85,	5.40,	4.00,	2.65,	1.35,	1.10,	0.80,	0.65,	0.55,	0.45,	0.35},
				{26.00,	24.10,	22.25,	20.45,	18.70,	17.00,	15.35,	13.75,	12.20,	10.70,	9.25,	7.85,	6.50,	5.20,	3.95,	2.75,	1.60,	1.20,	1.00,	0.85,	0.70},
				{26.90,	25.05,	23.25,	21.50,	19.80,	18.15,	16.55,	15.00,	13.50,	12.05,	10.65,	9.30,	8.00,	6.75,	5.55,	4.40,	3.30,	2.30,	1.80,	1.50,	1.25},
				{27.75,	25.95,	24.20,	22.50,	20.85,	19.25,	17.70,	16.20,	14.75,	13.35,	12.00,	10.70,	9.45,	8.25,	7.10,	6.00,	4.95,	3.95,	3.35,	2.80,	2.35},
				{28.55,	26.80,	25.10,	23.40,	21.80,	20.25,	18.75,	17.30,	15.90,	14.55,	13.25,	12.00,	10.80,	9.65,	8.55,	7.50,	6.50,	5.55,	4.85,	4.25,	3.75},
				{29.30,	27.60,	25.90,	24.25,	22.70,	21.20,	19.75,	18.35,	17.00,	15.70,	14.45,	13.25,	12.10,	11.00,	9.95,	8.95,	8.00,	7.10,	6.40,	5.75,	5.15},
				{30.00,	28.35,	26.65,	25.05,	23.55,	22.10,	20.70,	19.35,	18.05,	16.80,	15.60,	14.45,	13.35,	12.30,	11.30,	10.35,	9.45,	8.60,	7.90,	7.25,	6.65},
				{30.65,	29.05,	27.35,	25.75,	24.30,	22.90,	21.55,	20.25,	19.00,	17.80,	16.65,	15.55,	14.50,	13.50,	12.55,	11.65,	10.80,	10.00,	9.30,	8.65,	8.05},
				{31.25,	29.70,	28.00,	26.40,	25.00,	23.60,	22.30,	21.05,	19.85,	18.70,	17.60,	16.55,	15.55,	14.60,	13.70,	12.85,	12.05,	11.30,	10.60,	9.95,	9.35},
				{31.80,	30.30,	28.60,	27.00,	25.65,	24.30,	23.00,	21.80,	20.65,	19.55,	18.50,	17.50,	16.55,	15.65,	14.80,	14.00,	13.25,	12.55,	11.90,	11.30,	10.75}
			
			} };
			*/

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

			/*
			priceSurface.m_table = {
				{
					{ 17.00, 16.85, 16.60, 16.30, 16.05, 15.75, 15.35, 15.00, 14.65, 14.20, 13.80, 13.35, 12.90, 12.45, 12.00, 11.55, 11.15, 10.75, 10.35, 10.00, 9.65 },  // 1 week
					{ 18.00, 17.75, 17.50, 17.25, 17.00, 16.75, 16.45, 16.10, 15.75, 15.40, 15.00, 14.65, 14.20, 13.80, 13.35, 12.95, 12.55, 12.10, 11.70, 11.30, 10.95 },  // 2 weeks
					{ 19.00, 18.75, 18.45, 18.20, 17.90, 17.65, 17.35, 17.00, 16.70, 16.30, 15.95, 15.60, 15.20, 14.80, 14.40, 14.00, 13.60, 13.20, 12.80, 12.45, 12.05 },  // 3 weeks
					{ 19.90, 19.65, 19.40, 19.10, 18.85, 18.55, 18.25, 17.95, 17.60, 17.25, 16.85, 16.50, 16.10, 15.75, 15.40, 15.00, 14.60, 14.25, 13.85, 13.50, 13.10 },  // 4 weeks
					{ 20.80, 20.55, 20.25, 20.00, 19.70, 19.40, 19.10, 18.75, 18.45, 18.10, 17.75, 17.40, 17.00, 16.65, 16.30, 15.95, 15.60, 15.25, 14.90, 14.55, 14.15 },  // 5 weeks
					{ 21.65, 21.40, 21.10, 20.85, 20.55, 20.30, 19.95, 19.65, 19.35, 19.00, 18.65, 18.30, 17.95, 17.55, 17.20, 16.85, 16.50, 16.15, 15.85, 15.50, 15.10 },  // 6 weeks
					{ 22.45, 22.20, 21.95, 21.65, 21.35, 21.05, 20.75, 20.45, 20.15, 19.85, 19.50, 19.15, 18.80, 18.45, 18.10, 17.75, 17.40, 17.05, 16.70, 16.35, 16.00 },  // 7 weeks
					{ 23.30, 23.00, 22.75, 22.50, 22.20, 21.95, 21.65, 21.35, 21.00, 20.70, 20.35, 20.00, 19.65, 19.30, 18.95, 18.60, 18.25, 17.90, 17.55, 17.20, 16.85 },  // 8 weeks
					{ 24.15, 23.90, 23.60, 23.35, 23.05, 22.75, 22.50, 22.20, 21.90, 21.60, 21.25, 20.90, 20.55, 20.20, 19.85, 19.50, 19.15, 18.80, 18.45, 18.10, 17.75 },  // 9 weeks
					{ 25.00, 24.70, 24.50, 24.20, 23.90, 23.60, 23.30, 23.00, 22.70, 22.40, 22.05, 21.70, 21.35, 21.00, 20.65, 20.30, 19.95, 19.60, 19.25, 18.90, 18.55 }   // 10 weeks
				}
			};
			*/


			/*
			priceSurface.m_table = {
				{
					{21.27499962, 18.84999084, 16.54999542, 14.25, 12.0999999, 10.07499981, 8.199999809, 6.5, 5, 3.700000048, 2.644999981, 1.815000057, 1.189999998, 0.754999995, 0.464999989, 0.280000001, 0.164999962, 0.435000002, 0.569999993, 0.764999986, 1.034999967, 1.414999962, 1.929999828, 2.605000019, 3.474999905, 4.525000095, 5.775000095, 7.25, 8.949999809, 10.82499981, 12.9000001, 15.1500001, 17.52499962, 19.87499714},
					{41.07500076, 36.05000114, 31.25000286, 26.47499943, 21.82499695, 17.40000057, 13.3499999, 9.75, 6.700000048, 4.299999952, 2.539999962, 1.394999981, 0.725000024, 0.375, 0.194999993, 0.124999993, 0.069999998, 0.214999996, 0.310000002, 0.455000013, 0.680000007, 1.039999962, 1.634999275, 2.585000038, 4.000000119, 5.950000286, 8.599999905, 11.9000001, 15.82499981, 20.24999237, 24.97500038, 29.80000114, 34.74999237, 39.70000076, 41.32500076},
					{36.47500038, 31.69999981, 27.07499981, 22.59999943, 18.375, 14.5, 11.0250001, 8.050000191, 5.600000381, 3.675000072, 2.299999952, 1.379999995, 0.819999993, 0.49000001, 0.295000002, 0.185000002, 0.354999989, 0.50999999, 0.724999994, 1.054999828, 1.555000007, 2.309999943, 3.399999976, 4.924999952, 6.950000048, 9.5, 12.69999981, 16.42499924, 20.625, 25.125, 29.92499542, 34.77499962, 39.75000191, 41.82498932, 37.07500458},
					{32.42500114, 28.02500057, 23.72499847, 19.75, 16.0250001, 12.69999981, 9.775000095, 7.300000191, 5.299999952, 2.549999952, 1.135000229, 0.50999999, 0.260000005, 0.149999999, 0.094999997, 0.715000004, 0.980000019, 1.35499984, 1.885000229, 2.619999886, 3.600000024, 4.899999857, 6.600000143, 8.699999809, 11.2750001, 14.2750001, 21.625, 30.39999962, 39.92499542, 49.75, 59.74999237, 69.69999695, 42.09999084, 37.45000076, 32.95000458},
					{28.62499237, 24.47499943, 20.55000019, 17, 13.75, 10.875, 8.449999809, 6.375, 4.699999809, 3.399999976, 2.42500031, 1.705000162, 1.195000172, 0.840000004, 0.960000008, 1.300000012, 1.764999986, 2.389999986, 3.25, 4.350000143, 5.75, 7.5, 9.625, 12.17500019, 15.125, 18.44999695, 22.22499084, 26.29999542, 30.67500305, 35.25, 40.05000114, 42.57500458, 37.92500305, 33.52500916, 29.29999828},
					{25.25, 21.44999981, 17.90000153, 14.7249999, 11.875, 9.375, 7.274999619, 5.524999857, 4.100000143, 3.024999976, 2.179999828, 1.575000226, 1.134999931, 1.194999993, 1.585000098, 2.100000024, 2.784999967, 3.700000048, 4.875000238, 6.325000286, 8.099999905, 10.2750001, 12.7750001, 15.67500019, 18.94998932, 22.625, 26.59999847, 30.87499237, 35.39999962, 40.07500458, 44.625, 41, 36.75, 33.04999924},
					{29.44999981, 26.10000038, 22.92499924, 19.94999981, 17.27499962, 14.8499999, 12.625, 10.67500019, 8.950000286, 7.425000191, 6.174999952, 5.075000048, 4.125, 3.149999976, 3.924999952, 4.825000048, 5.924999952, 7.250000238, 8.75, 10.4749999, 12.5, 14.7249999, 17.22500038, 19.97500038, 22.97500324, 26.25, 29.75, 33.5, 37.67499924, 41.65000153, 48.375, 44.625, 41.42500114, 37.625},
					{34.39999962, 31.34999943, 28.375, 25.67500019, 23.02500057, 20.625, 18.375, 16.375, 14.5, 12.8499999, 11.29999971, 9.900000095, 8.675000191, 5.650000095, 6.700000286, 7.925000191, 9.225000381, 10.8499999, 12.5250001, 14.47500038, 16.625, 18.875, 21.32499981, 23.94999981, 26.92499828, 29.95000076, 33.25, 36.625, 40.25000763, 44.10000038, 51.
				}
			};
			*/


			/*
			// scale down prices
			for (std::size_t row{ 0 }; row < std::size(priceSurface.m_rowVals); ++row)
			{
				for (std::size_t col{ 0 }; col < std::size(priceSurface.m_colVals); ++col)
				{
					priceSurface.m_table[row][col] *= 0.5/static_cast<double>(col+1);
				}
			}
			*/


			priceSurface.m_colVals = { 150, 152.5, 155, 157.5, 160, 162.5, 165,
									167.5, 170, 172.5, 175, 177.5, 180, 182.5,
									185, 187.5, 190, 192.5, 195, 197.5, 200
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

	}
}