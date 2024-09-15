#include "adam.h"
#include <cmath>
#include <iostream>


// based on a medium article: https://medium.com/the-ml-practitioner/how-to-implement-an-adam-optimizer-from-scratch-76e7b217f1cc
constexpr void Adam::update(const auto& deriv)
{
    weightAndBias gradWeightBias{ deriv(m_state.m_weight), deriv(m_state.m_bias) };
    // momentum beta
    // *** weights *** 
    m_means.m_weight = m_firstOrderExpDecay * m_means.m_weight + (1 - m_firstOrderExpDecay) * gradWeightBias.m_weight;
    // *** biases *** 
    m_means.m_bias = m_firstOrderExpDecay * m_means.m_bias + (1 - m_firstOrderExpDecay) * gradWeightBias.m_bias;
	
    // rms beta
    // *** weights *** 
    m_variances.m_weight = m_secondOrderExpDecay * m_variances.m_weight + (1 - m_secondOrderExpDecay) * (gradWeightBias.m_weight * gradWeightBias.m_weight);
    // *** biases *** 
    m_variances.m_bias = m_secondOrderExpDecay * m_variances.m_bias + (1 - m_secondOrderExpDecay) * (gradWeightBias.m_bias);

    // bias correction
    weightAndBias meanCorrection{ m_means.m_weight / (1 - m_firstOrderExpDecay), m_means.m_bias / (1 - m_firstOrderExpDecay) };
    weightAndBias varianceCorrection{ m_variances.m_weight / (1 - m_secondOrderExpDecay), m_variances.m_bias / (1 - m_secondOrderExpDecay) };
    
    // update weights and biases
    m_state.m_weight = m_state.m_weight - m_stepSize * (meanCorrection.m_weight / (std::sqrt(varianceCorrection.m_weight) + m_tol));
    m_state.m_bias = m_state.m_bias - m_stepSize * (meanCorrection.m_bias / (std::sqrt(varianceCorrection.m_bias) + m_tol));

    return;
}

constexpr double Adam::optimize(const auto& func, const auto& deriv)
{
    bool converged{ false };
    int numIt{ 0 };
    double oldWeight{};
    while (!converged)
    {
        oldWeight = m_state.m_weight;
        update(deriv);
        converged = (std::abs(m_state.m_weight - oldWeight) < m_tol);
        ++numIt;
    }
    std::cout << "Adam optimization converged after " << numIt << " gradient steps.\n";
    std::cout << "Parameter value is " << m_state.m_weight << "\n";
    std::cout << "with a function value of " << func(m_state.m_weight) << ".\n";
    return m_state.m_weight;
}

