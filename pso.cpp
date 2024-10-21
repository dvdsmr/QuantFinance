#include "pso.h"
#include "options.h"


void PSO::set_uniformRandomPositions(std::vector<double> lowerIntervalBounds, std::vector<double> higherIntervalBounds)
{
    assert(std::size(lowerIntervalBounds) == m_swarm.m_dimension);
    assert(std::size(higherIntervalBounds) == m_swarm.m_dimension);
    for (std::size_t i{ 0 }; i < m_swarm.m_dimension; ++i)
    {
        assert(lowerIntervalBounds[i] < higherIntervalBounds[i]);
    }

    for (std::size_t i{ 0 }; i < m_swarm.m_numParticles; ++i)
    {
        m_swarm.m_positions[i] = Random::getVector(lowerIntervalBounds, higherIntervalBounds);
    }
}

void PSO::set_uniformRandomVelocities(std::vector<double> lowerIntervalBounds, std::vector<double> higherIntervalBounds)
{
    assert(std::size(lowerIntervalBounds) == m_swarm.m_dimension);
    assert(std::size(higherIntervalBounds) == m_swarm.m_dimension);
    for (std::size_t i{ 0 }; i < m_swarm.m_dimension; ++i)
    {
        assert(lowerIntervalBounds[i] < higherIntervalBounds[i]);
    }

    for (std::size_t i{ 0 }; i < m_swarm.m_numParticles; ++i)
    {
        m_swarm.m_positions[i] = Random::getVector(np::minus(np::abs(np::add(higherIntervalBounds, lowerIntervalBounds, 1.0, -1.0))),
            np::abs(np::add(higherIntervalBounds, lowerIntervalBounds, 1.0, -1.0)));
    }
}

void PSO::set_normalRandomVelocities(std::vector<double> means, std::vector<double> variances)
{
    assert(std::size(means) == m_swarm.m_dimension);
    assert(std::size(variances) == m_swarm.m_dimension);

    for (std::size_t i{ 0 }; i < m_swarm.m_numParticles; ++i)
    {
        m_swarm.m_velocities[i] = Random::getVectorNormals(means, variances);
    }
}


void testPSO()
{
    // we test PSO on calibrating vol to an options price
    double spot = 171.01;
    double strike = 180.0;
    double maturity = 1.0;
    double interest = 0.03;
    double dividendYield = 0.0;
    double trueVol{ 0.3 };

    double truePrice{ Options::Pricing::BSM::call(interest, trueVol, maturity, strike, spot, dividendYield) };

    auto func
    {
        [&](std::vector<double> vol) {
            double price{ Options::Pricing::BSM::call(interest, vol[static_cast<std::size_t>(0)], maturity, strike, spot, dividendYield)};
            return (price - truePrice) * (price - truePrice);
        }
    };

    PSO pso{ 10,1 };
    pso.set_uniformRandomPositions({ 0.1 }, { 0.9 });
    pso.set_uniformRandomVelocities({ 0.1 }, { 0.9 });
    std::vector<double> optVol{ pso.optimize(func,true) };
    std::cout << "PSO found vol of " << optVol[static_cast<std::size_t>(0)] << ". True vol is " << trueVol << ".\n";

}