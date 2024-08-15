#ifndef STOCKPATHS_H
#define STOCKPATHS_H
#include <vector>

struct StockPath
{
	int m_length{ 100 };
	std::vector<double> m_stockVals;
	std::vector<double> m_timeVals;

	StockPath(int length)
		: m_length{length}
		, m_stockVals{ std::vector<double>(static_cast<std::size_t>(length))}
		, m_timeVals{ std::vector<double>(static_cast<std::size_t>(length)) }
	{}

	StockPath(int length, std::vector<double> stockVals, std::vector<double> timeVals)
		: m_length{ length }
		, m_stockVals{ stockVals }
		, m_timeVals{ timeVals }
	{}
};

#endif
