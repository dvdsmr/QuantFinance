#ifndef ADAM_H
#define ADAM_H



class Adam
{
public:
    explicit Adam(double stepSize = 0.01,
                            double firstOrderExpDecay = 0.9, 
                            double secondOrderExpDecay = 0.999, 
                            double eps = 1e-8,
                            double tol = 1e-5)
        : m_stepSize{ stepSize }
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

    // updates the weight and bias of Adam in place
    constexpr void update(const auto& deriv);
    constexpr double optimize(const auto& func, const auto& deriv);

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


#endif
