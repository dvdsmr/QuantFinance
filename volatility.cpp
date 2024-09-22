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
			double volGuess{ 0.9 };
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
							double price{ Options::Pricing::BSM::call(riskFreeReturn, vol, priceSurface.m_colVals[col], priceSurface.m_rowVals[row], spot, dividendYield) };
							return (price - truePrice) * (price - truePrice);
						}
					};
					auto deriv
					{
						[&](double vol) {
							double price{ Options::Pricing::BSM::call(riskFreeReturn, vol, priceSurface.m_colVals[col], priceSurface.m_rowVals[row], spot, dividendYield) };
							return 2 * (price - truePrice) * Options::Pricing::BSM::callVega(riskFreeReturn, vol, priceSurface.m_colVals[col], priceSurface.m_rowVals[row], spot, dividendYield);
						}
					};

					// Note that adam keeps the optimized vol as its state, so each iteration
					// starts with the previously calibrated vol
					volSurface.m_table[row][col] = adam.optimize(func, deriv, true);

					// once we have iterated through all strikes for one maturity, we want
					// to start the optimizer with the vol previously calibrated for the lowest strike
					if (col == static_cast<std::size_t>(0))
					{
						volGuess = volSurface.m_table[row][col];
					}

				}
			}

			return volSurface;
		}

		LabeledTable testCalibration()
		{
			// initialize the price table
			using namespace std::string_view_literals;
			LabeledTable priceSurface("Price surface"sv,
				"Strikes"sv,
				21,
				"Time to maturity"sv,
				10,
				"European call price"sv
			);
			priceSurface.m_table =
			{ { {25.65,	26.30,	26.95,	27.55,	28.10,	28.60,	29.10,	29.55,	29.95,	30.35},
				{23.25,	23.95,	24.60,	25.20,	25.75,	26.25,	26.75,	27.20,	27.60,	28.00},
				{21.00,	21.70,	22.35,	23.00,	23.55,	24.05,	24.55,	25.00,	25.40,	25.80},
				{18.90,	19.65,	20.30,	20.95,	21.50,	22.00,	22.50,	22.95,	23.35,	23.75},
				{17.00,	17.75,	18.40,	19.05,	19.60,	20.10,	20.60,	21.05,	21.45,	21.85},
				{15.20,	16.00,	16.65,	17.30,	17.85,	18.35,	18.85,	19.30,	19.70,	20.10},
				{13.60,	14.40,	15.05,	15.70,	16.25,	16.75,	17.25,	17.70,	18.10,	18.50},
				{12.10,	12.90,	13.55,	14.20,	14.75,	15.25,	15.75,	16.20,	16.60,	17.00},
				{10.75,	11.50,	12.15,	12.80,	13.35,	13.85,	14.35,	14.80,	15.20,	15.60},
				{9.55,	10.30,	10.95,	11.60,	12.15,	12.65,	13.15,	13.60,	14.00,	14.40},
				{8.45,	9.20,	9.85,	10.50,	11.05,	11.55,	12.05,	12.50,	12.90,	13.30},
				{7.45,	8.20,	8.85,	9.50,	10.05,	10.55,	11.05,	11.50,	11.90,	12.30},
				{6.50,	7.25,	7.90,	8.55,	9.10,	9.60,	10.10,	10.55,	10.95,	11.35},
				{5.70,	6.45,	7.10,	7.75,	8.30,	8.80,	9.30,	9.75,	10.15,	10.55},
				{4.95,	5.70,	6.35,	7.00,	7.55,	8.05,	8.55,	9.00,	9.40,	9.80},
				{4.30,	5.00,	5.65,	6.30,	6.85,	7.35,	7.85,	8.30,	8.70,	9.10},
				{3.70,	4.40,	5.05,	5.70,	6.25,	6.75,	7.25,	7.70,	8.10,	8.50},
				{3.15,	3.85,	4.50,	5.15,	5.70,	6.20,	6.70,	7.15,	7.55,	7.95},
				{2.65,	3.35,	4.00,	4.65,	5.20,	5.70,	6.20,	6.65,	7.05,	7.45},
				{2.20,	2.90,	3.55,	4.20,	4.75,	5.25,	5.75,	6.20,	6.60,	7.00},
				{1.80,	2.45,	3.10,	3.75,	4.30,	4.80,	5.30,	5.75,	6.15,	6.55}
			} };

			priceSurface.m_rowVals = { 150, 152.5, 155, 157.5, 160, 162.5, 165,
									167.5, 170, 172.5, 175, 177.5, 180, 182.5,
									185, 187.5, 190, 192.5, 195, 197.5, 200
			};
			priceSurface.m_colVals = { 1.0 / 250.,2.0 / 250. ,3.0 / 250. ,4.0 / 250. ,5.0 / 250. ,
										6.0 / 250. ,7.0 / 250. ,8.0 / 250. ,9.0 / 250., 10.0 / 250. };

			const double dividendYield = 0.0053;
			const double spot{ 175.0 };
			const double riskFreeReturn{ 0.03 };
			LabeledTable volSurface{ call(priceSurface, riskFreeReturn, spot, dividendYield) };

			return volSurface;
		
		}
	}
}