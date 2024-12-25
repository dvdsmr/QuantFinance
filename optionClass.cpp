#include "optionClass.h"
#include "options.h"

/*
// currently only simple options with a single strike price considered
double Option::samplePayoff(double time)
{
	double spot = m_underlying.samplePrice(time);
	switch (m_type)
	{
	case call: return Options::Payoffs::call(m_strike, spot);
	case put: return Options::Payoffs::put(m_strike, spot);
	default: return Options::Payoffs::call(m_strike, spot);
	}
}
*/

// overload operators for Options
Option operator-(const Option& opt)
{
	Option newOpt{ opt };
	newOpt.set_quantity(-opt.get_quantity());
	return newOpt;
}

Option operator+(const Option& opt)
{
	Option newOpt{ opt };
	return newOpt;
}

Option operator*(double quantity, const Option& opt)
{
	Option newOpt{ opt };
	newOpt.set_quantity(quantity * opt.get_quantity());
	return newOpt;
}

Option operator*(const Option& opt, double quantity)
{
	return quantity * opt;
}

