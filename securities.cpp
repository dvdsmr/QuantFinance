#include "securities.h"
#include "sdes.h"

// for later definitions of more complex member functions

double SimpleStock::samplePrice(double time)
{
	return SDE::geometricBrownianMotion(m_initialPrice, time, m_drift, m_vol);
}