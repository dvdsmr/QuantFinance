#include "copola.h"

namespace Copola
{
	namespace Gaussian
	{
		std::vector<double> probOfNDefaultsUncorrolated(std::vector<double> defaultProbs)
		{
			std::size_t numBonds{ std::size(defaultProbs) };

			std::vector<double> probOfNDefaults(numBonds+1);
			// initialize default probs
			probOfNDefaults[static_cast<std::size_t>(0)] = 1. - defaultProbs[static_cast<std::size_t>(0)];
			probOfNDefaults[static_cast<std::size_t>(1)] = defaultProbs[static_cast<std::size_t>(0)];
			for (std::size_t i{ 2 }; i <= numBonds; ++i)
			{
				for (std::size_t j{ 1 }; j <= i; ++j)
				{
					probOfNDefaults[j] = probOfNDefaults[j - 1] * defaultProbs[i-1] + probOfNDefaults[j] * (1. - defaultProbs[i-1]);
				}
				probOfNDefaults[0] = probOfNDefaults[0] * (1. - defaultProbs[i-1]);
			}

			return probOfNDefaults;
		}



	}
}