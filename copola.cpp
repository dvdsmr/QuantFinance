#include "copola.h"
#include <iostream>
#include <vector>

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

        void testDefaultProbs()
        {
			// test Gaussian copola default probs
			std::vector<double> defaultProbs{ 0.2, 0.2, 0.06, 0.3, 0.4, 0.65, 0.3, 0.23, 0.02, 0.12, 0.134, 0.21,
				0.08, 0.1, 0.1, 0.02, 0.3, 0.015, 0.2, 0.03 };
			//std::vector<double> defaultProbs{ 0.5,0.5,0.5 };

			std::vector<double> probOfNDefaults{ Copola::Gaussian::probOfNDefaultsUncorrolated(defaultProbs) };


			// default probs
			double sum{ 0.0 };
			for (std::size_t i{ 0 }; i < std::size(probOfNDefaults); ++i)
			{
				sum += probOfNDefaults[i];
				std::cout << "The probability of exactly " << i << " defaults is " << probOfNDefaults[i] << ".\n";
			}
			std::cout << "The sum of all probs is " << sum << ".\n";

			// expected number of defaults
			double expNum{ 0.0 };
			for (std::size_t i{ 0 }; i < std::size(probOfNDefaults); ++i)
			{
				expNum += probOfNDefaults[i] * static_cast<double>(i);
			}
			std::cout << "The expected number of defaults is " << expNum << ".\n";

			// variance of number of defaults
			double varNum{ 0.0 };
			for (std::size_t i{ 0 }; i < std::size(probOfNDefaults); ++i)
			{
				varNum += probOfNDefaults[i] * (static_cast<double>(i) - expNum) * (static_cast<double>(i) - expNum);
			}
			std::cout << "The variance of the number of defaults is " << varNum << ".\n";

			// first tranche
			double expLossFirstTranche{ 0.0 };
			expLossFirstTranche += probOfNDefaults[1] * 1. / 2.;
			expLossFirstTranche += probOfNDefaults[2] * 1.;
			for (std::size_t i{ 3 }; i < std::size(probOfNDefaults); ++i)
			{
				expLossFirstTranche += probOfNDefaults[i];
			}
			std::cout << "The expected loss is " << expLossFirstTranche * 2. << ".\n";

			// this means that the expected loss in tranche 2 to 4 is
			double expLossSecondTranche{ 0.0 };
			//expLossFirstTranche += probOfNDefaults[2] * 1./3.;
			expLossSecondTranche += probOfNDefaults[3] * 1. / 2.;
			expLossSecondTranche += probOfNDefaults[4] * 1.;
			for (std::size_t i{ 5 }; i < std::size(probOfNDefaults); ++i)
			{
				expLossSecondTranche += probOfNDefaults[i];
			}
			std::cout << "The expected loss is " << expLossSecondTranche * 2. << ".\n";

			// this means that the expected loss in tranche 4 to 20 is
			double expLossLastTranche{ 0.0 };
			for (std::size_t i{ 5 }; i < std::size(probOfNDefaults); ++i)
			{
				expLossLastTranche += probOfNDefaults[i] * static_cast<double>(i - 4) / 16.;
			}
			std::cout << "The expected loss is " << expLossLastTranche * 16. << ".\n";

			std::cout << "sum of all expected losses is " << expLossLastTranche * 16. + expLossFirstTranche * 2. + expLossZeroTranche * 2. << ".\n";

        }

	}
}