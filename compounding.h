#ifndef COMPOUNDING_H
#define COMPOUNDING_H

namespace Compounding
{
	auto discrete(double value, int timeUnits, double rate, int compoundingsPerTimeUnit) -> double;
	auto continuous(double value, double time, double rate) -> double;
	void continuousUnitTest();
	void discreteUnitTest();
}


#endif