#include "optionClass.h"
#include "options.h"


void Option::printInfo()
{
	std::cout << "\n";

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
	std::cout << "option.\n";
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

