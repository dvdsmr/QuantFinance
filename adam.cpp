#include "adam.h"



constexpr void Adam::update(const auto& func, const auto& deriv)
{
    weightAndBias gradWeightBias{ deriv(m_state.m_weight), deriv(m_state.m_bias) };
    // *** weights *** 
    m_means.m_weight = m_firstOrderExpDecay * m_means.m_weight + (1 - m_firstOrderExpDecay) * gradWeightBias.m_weight;
    // *** biases *** #
    m_means.m_bias = m_firstOrderExpDecay * m_means.m_bias + (1 - m_firstOrderExpDecay) * gradWeightBias.m_bias;
	return;
}

