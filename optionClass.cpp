#include "optionClass.h"


void Option::printInfo()
{
	std::cout << "\n=== Information on option ===\n";

	// print exercise type
	switch (get_exerciseType())
	{
	case Option::european:
		std::cout << "European ";
		break;
	case Option::american:
		std::cout << "American ";
		break;
	case Option::asian:
		std::cout << "Asian ";
		break;
	default:
		std::cout << "Unspecified ";
		break;
	}

	// print payoff type
	switch (get_payoffType())
	{
	case Option::call:
		std::cout << "call ";
		break;
	case Option::put:
		std::cout << "put ";
		break;
	default:
		std::cout << "call ";
		break;
	}
	std::cout << "option with strike $" << m_strike << ".\n";

	std::cout << "=== Information on underlying ===\n";
	get_underlying()->printInfo();
	std::cout << "\n";
}


auto Option::price(double riskFreeReturn, double dividendYield) -> double
{
	// number of MC samples for simulation
	std::size_t sampleNum{ 10000 };
	switch (m_etype)
	{
	case Option::european:
	{
		std::vector<double> predictedSpots{ m_underlying->monteCarlo(m_maturity, sampleNum, riskFreeReturn - dividendYield).m_yVals };
		switch (m_type)
		{
		case Option::call:
		{
			auto payoff_call{ [&](double value) { return Options::Payoffs::call(m_strike, value); } };
			return Options::Pricing::Utils::_discountedExpectedPayoff(payoff_call, predictedSpots, riskFreeReturn, m_maturity);
		}
		case Option::put:
		{
			auto payoff_put{ [&](double value) { return Options::Payoffs::put(m_strike, value); } };
			return Options::Pricing::Utils::_discountedExpectedPayoff(payoff_put, predictedSpots, riskFreeReturn, m_maturity);
		}
		default:
			throw std::runtime_error("Invalid payoff type.");
		}
	}
	case Option::asian:
	{
		switch (m_type)
		{
		case Option::call:
		{
			return Options::Pricing::Exotic::Asian::call(static_cast<std::size_t>(m_maturity * 250), riskFreeReturn, m_maturity, m_strike, m_underlying->getSpot(), dividendYield, m_underlying->getParams());
		}
		case Option::put:
		{
			return Options::Pricing::Exotic::Asian::put(static_cast<std::size_t>(m_maturity * 250), riskFreeReturn, m_maturity, m_strike, m_underlying->getSpot(), dividendYield, m_underlying->getParams());
		}
		default:
			throw std::runtime_error("Invalid payoff type.");
		}
	}
	default:
		throw std::runtime_error("No exercise types other than European supported at the moment.");
	}
}


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

