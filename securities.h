#ifndef SECURITIES_H
#define SECURITIES_H

class SimpleStock
{
	// This is a class of modelled stocks which follow GBM/Bachelier.
	// The stock's possible sample paths are entirely determined
	// by its starting value, its drift and its volatility
public:
	// Constructor
	constexpr SimpleStock(double initialPrice = 100., double drift = 0.1, double vol = 0.05)
		: m_initialPrice{ initialPrice }
		, m_drift{ drift }
		, m_vol{ vol }
	{}
	// Setters
	constexpr void setInitialPrice(double initialPrice) { m_initialPrice = initialPrice; }
	constexpr void setDrift(double drift) { m_drift = drift; }
	constexpr void setVol(double vol) { m_vol = vol; }
	// Getters
	constexpr const double& getInitialPrice() const { return m_initialPrice; }
	constexpr const double& getDrift() const { return m_drift; }
	constexpr const double& getVol() const { return m_vol; }

private:
	double m_initialPrice{ 100. };
	double m_drift{ 0.1 };
	double m_vol{ 0.05 };

};

#endif
