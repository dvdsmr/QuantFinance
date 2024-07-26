#ifndef SECURITIES_H
#define SECURITIES_H

class SimpleStock
{
	// This is a class of modelled stocks which follow GBM/Bachelier.
	// The stock's possible sample paths are entirely determined
	// by its starting value, its drift and its volatility
public:
	// Constructor
	explicit SimpleStock(double initialPrice = 100., double drift = 0.1, double vol = 0.05)
		: m_initialPrice{ initialPrice }
		, m_drift{ drift }
		, m_vol{ vol }
	{}
	// Setters
	auto setInitialPrice(double initialPrice) -> void { m_initialPrice = initialPrice; }
	auto setDrift(double drift) -> void { m_drift = drift; }
	auto setVol(double vol) -> void { m_vol = vol; }
	// Getters
	auto getInitialPrice() -> double const { return m_initialPrice; }
	auto getDrift() -> double const { return m_drift; }
	auto getVol() -> double const { return m_vol; }

private:
	double m_initialPrice{ 100. };
	double m_drift{ 0.1 };
	double m_vol{ 0.05 };

};

#endif
