#include "numpy.h"
#include "xyvals.h"
#include "options.h"
#include "adam.h"
#include <string>
#include <string_view>


namespace Volatility
{
	namespace Surface
	{
		//	UNDER CONSTRUCTION
		auto bsm(const LabeledTable& priceSurface, double riskFreeReturn, double spot, double dividendYield, std::string_view type = "call", std::string_view optimizer = "adam") -> LabeledTable
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

			//priceSurface.m_table =
			//{ { {25.10,	23.15,	21.25,	19.40,	17.60,	15.85,	14.15,	12.50,	10.90,	9.35,	7.85,	6.40,	5.00,	3.65,	2.35,	1.10,	0.80,	0.65,	0.55,	0.45,	0.35},
			//	{26.00,	24.10,	22.25,	20.45,	18.70,	17.00,	15.35,	13.75,	12.20,	10.70,	9.25,	7.85,	6.50,	5.20,	3.95,	2.75,	1.60,	1.20,	1.00,	0.85,	0.70},
			//	{26.90,	25.05,	23.25,	21.50,	19.80,	18.15,	16.55,	15.00,	13.50,	12.05,	10.65,	9.30,	8.00,	6.75,	5.55,	4.40,	3.30,	2.30,	1.80,	1.50,	1.25},
			//	{27.75,	25.95,	24.20,	22.50,	20.85,	19.25,	17.70,	16.20,	14.75,	13.35,	12.00,	10.70,	9.45,	8.25,	7.10,	6.00,	4.95,	3.95,	3.35,	2.80,	2.35},
			//	{28.55,	26.80,	25.10,	23.40,	21.80,	20.25,	18.75,	17.30,	15.90,	14.55,	13.25,	12.00,	10.80,	9.65,	8.55,	7.50,	6.50,	5.55,	4.85,	4.25,	3.75},
			//	{29.30,	27.60,	25.90,	24.25,	22.70,	21.20,	19.75,	18.35,	17.00,	15.70,	14.45,	13.25,	12.10,	11.00,	9.95,	8.95,	8.00,	7.10,	6.40,	5.75,	5.15},
			//	{30.00,	28.35,	26.65,	25.05,	23.55,	22.10,	20.70,	19.35,	18.05,	16.80,	15.60,	14.45,	13.35,	12.30,	11.30,	10.35,	9.45,	8.60,	7.90,	7.25,	6.65},
			//	{30.65,	29.05,	27.35,	25.75,	24.30,	22.90,	21.55,	20.25,	19.00,	17.80,	16.65,	15.55,	14.50,	13.50,	12.55,	11.65,	10.80,	10.00,	9.30,	8.65,	8.05},
			//	{31.25,	29.70,	28.00,	26.40,	25.00,	23.60,	22.30,	21.05,	19.85,	18.70,	17.60,	16.55,	15.55,	14.60,	13.70,	12.85,	12.05,	11.30,	10.60,	9.95,	9.35},
			//	{31.80,	30.30,	28.60,	27.00,	25.65,	24.30,	23.00,	21.80,	20.65,	19.55,	18.50,	17.50,	16.55,	15.65,	14.80,	14.00,	13.25,	12.55,	11.90,	11.30,	10.75}
			//
			//} };

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
			priceSurface.m_rowVals = { 1.0 / 250.,2.0 / 250. ,3.0 / 250. ,4.0 / 250. ,5.0 / 250. ,
										6.0 / 250. ,7.0 / 250. ,8.0 / 250. ,9.0 / 250., 10.0 / 250. };

			const double dividendYield = 0.007;
			const double spot{ 175.0 };
			const double riskFreeReturn{ 0.045 };
			LabeledTable volSurface{ bsm(priceSurface, riskFreeReturn, spot, dividendYield, "call", "bruteForce")};

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