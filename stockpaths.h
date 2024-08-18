#ifndef STOCKPATHS_H
#define STOCKPATHS_H
#include <vector>

struct StockPath
{
	std::size_t m_length{ 100 };
	std::vector<double> m_stockVals;
	std::vector<double> m_timeVals;

	StockPath(std::size_t length)
		: m_length{length}
		, m_stockVals{ std::vector<double>(length)}
		, m_timeVals{ std::vector<double>(length) }
	{}

	StockPath(std::size_t length, std::vector<double> stockVals, std::vector<double> timeVals)
		: m_length{ length }
		, m_stockVals{ stockVals }
		, m_timeVals{ timeVals }
	{}

};


#endif
