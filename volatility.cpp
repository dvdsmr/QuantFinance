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
			LabeledTable volSurface(priceSurface.m_tableName,
				priceSurface.m_rowLabel,
				priceSurface.m_numRows,
				priceSurface.m_colLabel,
				priceSurface.m_numCols,
				"Implied volatility"sv
			);

			// define strikes and maturities
			volSurface.m_rowVals = priceSurface.m_rowVals; // time to maturity
			volSurface.m_colVals = priceSurface.m_rowVals; // strike

			// calibrate the vol surface. 
			double volGuess{ 0.1 };
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
					volSurface.m_table[row][col] = adam.optimize(func, deriv);

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
	}
}