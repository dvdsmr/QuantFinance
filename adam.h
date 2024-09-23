#ifndef ADAM_H
#define ADAM_H

#include <iostream>
#include <cmath>

class Adam
{
public:
    explicit Adam(double initialVal=0.0,
                            double stepSize = 0.01,
                            double firstOrderExpDecay = 0.9, 
                            double secondOrderExpDecay = 0.999, 
                            double eps = 1e-8,
                            double tol = 1e-8)
        : m_state{ initialVal,0.01 }
        , m_stepSize{ stepSize }
        , m_firstOrderExpDecay{ firstOrderExpDecay }
        , m_secondOrderExpDecay{ secondOrderExpDecay }
        , m_eps{ eps }
        , m_tol{ tol }
    {}

    // simple pair structure to store output of ADAM optimization step
    struct weightAndBias
    {
        double m_weight{ 0.0 };
        double m_bias{ 0.0 };

        weightAndBias(double weight=0.0, double bias=0.0)
            : m_weight{ weight }
            , m_bias{ bias }
        {}
    };

    // setter
    void set_state(double initialVal) { m_state = { initialVal,0.0 }; }

    // updates the weight and bias of Adam in place
    constexpr void update(int t, const auto& deriv);
    constexpr double optimize(const auto& func, const auto& deriv, bool verbose=false);

private:
    double m_stepSize{ 0.01 };
    double m_firstOrderExpDecay{ 0.9 };
    double m_secondOrderExpDecay{ 0.999 };
    double m_eps{ 1e-8 };
    double m_tol{ 1e-5 };

    weightAndBias m_state{};
    weightAndBias m_means{};
    weightAndBias m_variances{};
};


// based on a medium article: https://medium.com/the-ml-practitioner/how-to-implement-an-adam-optimizer-from-scratch-76e7b217f1cc
constexpr void Adam::update(int t, const auto& deriv)
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
    m_variances.m_bias = m_secondOrderExpDecay * m_variances.m_bias + (1 - m_secondOrderExpDecay) * (gradWeightBias.m_bias * gradWeightBias.m_bias);

    // bias correction
    weightAndBias meanCorrection{ m_means.m_weight / std::max(1e-8, (1 - std::pow(m_firstOrderExpDecay, t))),
                                  m_means.m_bias / std::max(1e-8, (1 - std::pow(m_firstOrderExpDecay, t))) };
    weightAndBias varianceCorrection{ m_variances.m_weight / std::max(1e-8, (1 - std::pow(m_secondOrderExpDecay, t))),
                                      m_variances.m_bias / std::max(1e-8, (1 - std::pow(m_secondOrderExpDecay, t))) };


    // update weights and biases
    m_state.m_weight = m_state.m_weight - m_stepSize * (meanCorrection.m_weight / (std::sqrt(varianceCorrection.m_weight) + m_eps));
    m_state.m_bias = m_state.m_bias - m_stepSize * (meanCorrection.m_bias / (std::sqrt(varianceCorrection.m_bias) + m_eps));

    return;
}


constexpr double Adam::optimize(const auto& func, const auto& deriv, bool verbose)
{
    bool converged{ false };
    double oldWeight{};
    double oldBias{};
    int t{ 1 };
    while (!converged)
    {
        oldWeight = m_state.m_weight;
        oldBias = m_state.m_bias;
        update(t,deriv);
        // Check for convergence on both weight and bias
        converged = (std::abs(m_state.m_weight - oldWeight) < m_tol) &&
            (std::abs(m_state.m_bias - oldBias) < m_tol);
        ++t;
    }
    if (verbose)
    {
        std::cout << "Adam optimization converged after " << t-1 << " gradient steps.\n";
        std::cout << "Parameter value is " << m_state.m_weight << "\n";
        std::cout << "with a function value of " << func(m_state.m_weight) << ".\n";
        std::cout << "bias is " << m_state.m_bias << "\n";
    }
    return m_state.m_weight;
}

void testAdam();

#endif
