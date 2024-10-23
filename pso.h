#ifndef PSO_H
#define PSO_H

#include "Random.h"
#include "numpy.h"
#include <iostream>
#include <cmath>
#include <vector>
#include <cassert>

// Particle Swarm Optimization (PSO) for derivative free optimization problems like calibrating a Heston model

class PSO
{
public:
    explicit PSO(std::size_t numParticles = 10,
        std::size_t dimension = 2)
        : m_swarm{ Swarm(numParticles,dimension) }
    {}

    // swarm struct collecting information about the particle swarm of the PSO optimizer
    struct Swarm
    {
        std::size_t m_numParticles{ 10 };
        std::size_t m_dimension{ 2 };
        std::vector<std::vector<double>> m_positions{};
        std::vector<std::vector<double>> m_velocities{};
        std::vector<std::vector<double>> m_bestKnownPositions{};
        std::vector<double> m_bestKnownPositionSwarm{};


        Swarm(std::size_t numParticles = 10, std::size_t dimension = 2)
            : m_numParticles{ numParticles }
            , m_dimension{ dimension }
            , m_positions{ std::vector<std::vector<double>>(numParticles, std::vector<double>(dimension, 0)) }
            , m_velocities{ std::vector<std::vector<double>>(numParticles, std::vector<double>(dimension, 0)) }
            , m_bestKnownPositions{ std::vector<std::vector<double>>(numParticles, std::vector<double>(dimension, 0)) }
            , m_bestKnownPositionSwarm{ std::vector<double>(dimension) }
        {}
    };

    // setters
    void set_uniformRandomPositions(std::vector<double> lowerIntervalBounds, std::vector<double> higherIntervalBounds);
    void set_uniformRandomVelocities(std::vector<double> lowerIntervalBounds, std::vector<double> higherIntervalBounds);
    void set_normalRandomVelocities(std::vector<double> means, std::vector<double> variances);
    void set_coefficients(double cognitive, double social) { m_cognitiveCoeff = cognitive; m_socialCoeff = social; }
    void set_inertia(double inertia) { m_inertiaWeight = inertia; }

    // optimization routine
    constexpr std::vector<double> optimize(const auto& func, bool verbose = false);

private:
    Swarm m_swarm{};
    double m_cognitiveCoeff{ 0.1 };
    double m_socialCoeff{ 0.1 };
    double m_inertiaWeight{ 0.1 };
};



constexpr std::vector<double> PSO::optimize(const auto& func, bool verbose)
{
    // set best known positions to initial positions
    m_swarm.m_bestKnownPositions = m_swarm.m_positions;

    // initialize best known position of entire swarm
    double bestFuncVal{ 1e20 };
    double currentFuncVal{};
    std::vector<double> bestCurrentFuncVals(m_swarm.m_numParticles);
    std::size_t count{ 0 };
    for (const auto& position : m_swarm.m_positions)
    {
        currentFuncVal = func(position);
        bestCurrentFuncVals[count] = currentFuncVal;
        if (currentFuncVal < bestFuncVal)
        {
            m_swarm.m_bestKnownPositionSwarm = position;
            bestFuncVal = currentFuncVal;
        }
        ++count;
    }

    bool converged{ false };
    int counter{ 0 };
    while (!converged)
    {
        for (std::size_t i{ 0 }; i < m_swarm.m_numParticles; ++i)
        {
            std::vector<double> rp{ Random::getVector(m_swarm.m_dimension,0.0,1.0) };
            std::vector<double> rg{ Random::getVector(m_swarm.m_dimension,0.0,1.0) };

            // update velocities
            m_swarm.m_velocities[i] = np::add(np::add(np::multiply(m_swarm.m_velocities[i], m_inertiaWeight)
                , np::multiply(np::multiply(m_cognitiveCoeff, rp), np::add(m_swarm.m_bestKnownPositions[i], m_swarm.m_positions[i], 1.0, -1.0)))
                , np::multiply(np::multiply(m_socialCoeff, rg), np::add(m_swarm.m_bestKnownPositionSwarm, m_swarm.m_positions[i], 1.0, -1.0)));

            // update positions
            m_swarm.m_positions[i] = np::add(m_swarm.m_positions[i], m_swarm.m_velocities[i]);

            // check new function values
            currentFuncVal = func(m_swarm.m_positions[i]);
            if (currentFuncVal < bestCurrentFuncVals[i])
            {
                m_swarm.m_bestKnownPositions[i] = m_swarm.m_positions[i];
                bestCurrentFuncVals[i] = currentFuncVal;
                if (bestCurrentFuncVals[i] < bestFuncVal)
                {
                    m_swarm.m_bestKnownPositionSwarm = m_swarm.m_bestKnownPositions[i];
                    bestFuncVal = bestCurrentFuncVals[i];
                    if (verbose)
                    {
                        std::cout << "New best position with function value " << bestFuncVal << " found.\n";
                    }
                }
            }
        }

        ++counter;
        if (bestFuncVal < 1e-5 || counter > 1000)
        {
            converged = true;
        }
        if (verbose)
        {
            std::cout << "Iteration " << counter << " finished.\n";
        }
    }

    return m_swarm.m_bestKnownPositionSwarm;
}


void testPSO();

#endif
