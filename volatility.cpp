#include "numpy.h"
#include "xyvals.h"
#include "options.h"
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
			volSurface.m_rowVals = priceSurface.m_rowVals;
			volSurface.m_colVals = priceSurface.m_rowVals;

			// get the table values by the BSM model
			for (std::size_t row{ 0 }; row < std::size(volSurface.m_rowVals); ++row)
			{
				for (std::size_t col{ 0 }; col < std::size(volSurface.m_colVals); ++col)
				{
					double vol{ 0.1 };
					// ToDO: here we need to minimize the right hand side over the vol parameter
					volSurface.m_table[row][col] = Options::Pricing::BSM::call(riskFreeReturn,
						vol,
						priceSurface.m_rowVals[row],
						priceSurface.m_colVals[col],
						spot,
						dividendYield) - priceSurface.m_table[row][col];
				}
			}

			return volSurface;
		}
	}
}