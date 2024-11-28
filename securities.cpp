#include "securities.h"
#include "sdes.h"
#include <string_view>
#include <iostream>

// for later definitions of more complex member functions

double SimpleStock::samplePrice(double time, std::string_view model)
{
	if (model == "bsm")
	{
		return SDE::BSM::simulate(m_initialPrice, time, m_drift, m_vol);
	}
	else if (model == "bachelier")
	{
		return SDE::OrnsteinUhlenbeck::simulate(m_initialPrice, time, m_drift, 0.0, m_vol);
	}
	else // default is BSM
	{
		return SDE::BSM::simulate(m_initialPrice, time, m_drift, m_vol);
	}
}
