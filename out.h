#ifndef OUT_H
#define OUT_H
#include "xyvals.h"
#include<iostream>

void outputPriceGrid(const PriceGrid& priceGrid)
{
	const std::size_t length{ std::size(priceGrid.m_gridVals) };
	std::cout << "\n" << priceGrid.m_gridName << "\n";
	std::cout << std::setprecision(4);
	for (std::size_t row{ 0 }; row < length; ++row)
	{
		for (std::size_t col{ 0 }; col < length; ++col)
		{
			if (static_cast<int>(col) < static_cast<int>(length-1-row))
			{
				std::cout << "\t";
			}
			else
			{
				std::cout << "\t" << priceGrid.m_gridVals[col][length-1-row];
				//std::cout << "\t" << 100.;
			}
		}
		std::cout << "\n";
	}
}

#endif
