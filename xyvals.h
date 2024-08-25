#ifndef XYVALS_H
#define XYVALS_H
#include <vector>

struct XYVals
{
	std::size_t m_length{ 100 };
	std::vector<double> m_yVals;
	std::vector<double> m_xVals;

	XYVals(std::size_t length)
		: m_length{length}
		, m_yVals{ std::vector<double>(length) }
		, m_xVals{ std::vector<double>(length) }
	{}

	XYVals(std::size_t length, std::vector<double> yVals, std::vector<double> xVals)
		: m_length{ length }
		, m_yVals{ yVals }
		, m_xVals{ xVals }
	{}

};


struct DataTable
{
	std::vector<std::vector<double>> m_table;
	std::size_t m_numRows{ 1 };
	std::size_t m_numCols{ 1 };

	DataTable(std::size_t numRows, std::size_t numCols)
		: m_numRows{ numRows }
		, m_numCols{ numCols }
		, m_table{ std::vector<std::vector<double>>(numRows, std::vector<double>(numCols, 0)) }
	{}
};


#endif
