#include "securities.h"
#include "sdes.h"
#include <string_view>
#include <iostream>

// for later definitions of more complex member functions
namespace Securities
{
	
	/*
	template <typename Params>
	auto ModelStock<Params>::path(double terminalTime, std::size_t timePoints, double drift) -> XYVals
	{
		return SDE::path(getSpot(), terminalTime, timePoints, drift, m_params);
	}
	*/
	

	double SimpleStock::samplePrice(double time, std::string_view model)
	{
		if (model == "bsm")
		{
			return SDE::BSM::simulate(getSpot(), time, m_drift, m_vol);
		}
		else if (model == "bachelier")
		{
			return SDE::OrnsteinUhlenbeck::simulate(getSpot(), time, m_drift, 0.0, m_vol);
		}
		else // default is BSM
		{
			return SDE::BSM::simulate(getSpot(), time, m_drift, m_vol);
		}
	}
}
