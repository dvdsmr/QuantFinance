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
		auto call(const LabeledTable& priceSurface, double riskFreeReturn, double spot, double dividendYield) -> LabeledTable
		{

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
							double price{ Options::Pricing::BSM::call(riskFreeReturn, vol, priceSurface.m_rowVals[row], priceSurface.m_colVals[col], spot, dividendYield) };
							return (price - truePrice) * (price - truePrice);
						}
					};
					auto deriv
					{
						[&](double vol) {
							double price{ Options::Pricing::BSM::call(riskFreeReturn, vol, priceSurface.m_rowVals[row], priceSurface.m_colVals[col], spot, dividendYield) };
							return 2 * (price - truePrice) * Options::Pricing::BSM::callVega(riskFreeReturn, vol, priceSurface.m_rowVals[row], priceSurface.m_colVals[col], spot, dividendYield);
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

			return volSurface;
		}

		LabeledTable testCalibration()
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

			priceSurface.m_table =
			{
				{
					{ 17.10, 16.85, 16.50, 16.10, 15.75, 15.30, 14.85, 14.35, 13.85, 13.35, 12.85, 12.30, 11.75, 11.20, 10.65, 10.10, 9.55, 9.05, 8.50, 8.00, 7.50 },  // 1 week
					{ 18.05, 17.80, 17.40, 17.00, 16.55, 16.10, 15.60, 15.15, 14.65, 14.15, 13.60, 13.05, 12.55, 12.00, 11.45, 10.90, 10.35, 9.85, 9.30, 8.80, 8.25 },  // 2 weeks
					{ 18.90, 18.60, 18.25, 17.85, 17.45, 16.95, 16.50, 16.05, 15.55, 15.05, 14.55, 14.00, 13.50, 12.95, 12.40, 11.85, 11.30, 10.80, 10.25, 9.75, 9.20 },  // 3 weeks
					{ 19.75, 19.45, 19.05, 18.65, 18.25, 17.80, 17.30, 16.85, 16.35, 15.85, 15.30, 14.80, 14.25, 13.70, 13.15, 12.60, 12.05, 11.55, 11.00, 10.50, 10.00 },  // 4 weeks
					{ 20.50, 20.20, 19.80, 19.40, 18.95, 18.50, 18.05, 17.60, 17.10, 16.60, 16.10, 15.60, 15.05, 14.50, 13.95, 13.40, 12.85, 12.30, 11.80, 11.30, 10.80 },  // 5 weeks
					{ 21.25, 20.90, 20.50, 20.10, 19.65, 19.15, 18.75, 18.25, 17.75, 17.25, 16.75, 16.20, 15.65, 15.10, 14.55, 14.00, 13.50, 13.00, 12.50, 12.00, 11.50 },  // 6 weeks
					{ 22.00, 21.65, 21.25, 20.80, 20.35, 19.90, 19.40, 18.95, 18.45, 17.95, 17.45, 16.90, 16.35, 15.80, 15.25, 14.70, 14.20, 13.70, 13.20, 12.70, 12.20 },  // 7 weeks
					{ 22.75, 22.35, 21.95, 21.50, 21.05, 20.55, 20.10, 19.65, 19.15, 18.65, 18.10, 17.55, 17.00, 16.45, 15.90, 15.35, 14.85, 14.35, 13.85, 13.35, 12.85 },  // 8 weeks
					{ 23.50, 23.10, 22.70, 22.25, 21.75, 21.30, 20.85, 20.35, 19.85, 19.35, 18.80, 18.25, 17.70, 17.15, 16.60, 16.05, 15.55, 15.05, 14.55, 14.05, 13.55 },  // 9 weeks
					{ 24.25, 23.85, 23.40, 23.00, 22.50, 22.00, 21.55, 21.05, 20.55, 20.05, 19.50, 19.00, 18.45, 17.90, 17.35, 16.80, 16.30, 15.80, 15.30, 14.80, 14.30 }   // 10 weeks
				}
			};

			priceSurface.m_colVals = { 150, 152.5, 155, 157.5, 160, 162.5, 165,
									167.5, 170, 172.5, 175, 177.5, 180, 182.5,
									185, 187.5, 190, 192.5, 195, 197.5, 200
									};
			priceSurface.m_rowVals = { 1.0 / 250.,2.0 / 250. ,3.0 / 250. ,4.0 / 250. ,5.0 / 250. ,
										6.0 / 250. ,7.0 / 250. ,8.0 / 250. ,9.0 / 250., 10.0 / 250. };

			const double dividendYield = 0.007;
			const double spot{ 175.0 };
			const double riskFreeReturn{ 0.045 };
			LabeledTable volSurface{ call(priceSurface, riskFreeReturn, spot, dividendYield) };

			return volSurface;
		
		}
	}
}