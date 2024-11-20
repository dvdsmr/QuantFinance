#ifndef COPOLA_H
#define COPOLA_H
#include <vector>

namespace Copola
{
	namespace Gaussian
	{
		auto probOfNDefaultsUncorrolated(std::vector<double> defaultProbs) -> std::vector<double>;
		void testDefaultProbs();
	}
}


#endif

