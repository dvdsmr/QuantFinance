#include "copola.h"
#include <iostream>

namespace Copola
{
	namespace Gaussian
	{
        std::vector<double> probOfNDefaultsUncorrolated(std::vector<double> defaultProbs)
        {
            std::size_t numBonds{ std::size(defaultProbs) };

            std::vector<double> probOfNDefaults(numBonds + 1, 0.0);

            // Initialize probabilities for the first bond
            probOfNDefaults[0] = 1.0 - defaultProbs[0];
            probOfNDefaults[1] = defaultProbs[0];

            // Loop over each bond starting from the second
            for (std::size_t i = 1; i < numBonds; ++i)
            {
                // Traverse backwards to avoid overwriting previous values prematurely
                for (std::size_t j = i + 1; j > 0; --j)
                {
                    probOfNDefaults[j] = probOfNDefaults[j - 1] * defaultProbs[i] + probOfNDefaults[j] * (1.0 - defaultProbs[i]);
                }
                // Update the 0 defaults case
                probOfNDefaults[0] *= (1.0 - defaultProbs[i]);
            }

            return probOfNDefaults;
        }



	}
}