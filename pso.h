#ifndef PSO_H
#define PSO_H

#include "Random.h"
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
    void set_normalRandomVelocities(std::vector<double> means, std::vector<double> variances);

private:
    Swarm m_swarm{};
};

void PSO::set_uniformRandomPositions(std::vector<double> lowerIntervalBounds, std::vector<double> higherIntervalBounds)
{
    assert (std::size(lowerIntervalBounds) == m_swarm.m_dimension);
    assert (std::size(higherIntervalBounds) == m_swarm.m_dimension);
    for (std::size_t i{ 0 }; i < m_swarm.m_dimension; ++i)
    {
        assert (lowerIntervalBounds[i] < higherIntervalBounds[i]);
    }

    for (std::size_t i{ 0 }; i < m_swarm.m_numParticles; ++i)
    {
        m_swarm.m_positions[i] = Random::getVector(lowerIntervalBounds, higherIntervalBounds);
    }
}

void PSO::set_normalRandomVelocities(std::vector<double> means, std::vector<double> variances)
{
    assert (std::size(means) == m_swarm.m_dimension);
    assert (std::size(variances) == m_swarm.m_dimension);

    for (std::size_t i{ 0 }; i < m_swarm.m_numParticles; ++i)
    {
        m_swarm.m_velocities[i] = Random::getVectorNormals(means, variances);
    }
}


#endif
