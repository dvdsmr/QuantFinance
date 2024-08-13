#include "securities.h"
#include "sdes.h"
#include <string_view>
#include <iostream>

// for later definitions of more complex member functions

double SimpleStock::samplePrice(double time, std::string_view model)
{
	if (model == "bsm")
	{
		return SDE::geometricBrownianMotion(m_initialPrice, time, m_drift, m_vol);
	}
	else if (model == "bachelier")
	{
		return SDE::OrnsteinUhlenbeckSimulate(m_initialPrice, time/100.0, m_drift, m_vol, time);
	}
	else // default is BSM
	{
		return SDE::geometricBrownianMotion(m_initialPrice, time, m_drift, m_vol);
	}
}