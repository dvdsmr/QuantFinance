#ifndef COMPOUNDING_H
#define COMPOUNDING_H

namespace Compounding
{
	auto discrete(double value, int timeUnits, double rate) -> double;
	auto continuous(double value, int time, double rate) -> double;
	void continuousUnitTest();
}


#endif