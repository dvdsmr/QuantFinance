#ifndef OPTION_CLASS_H
#define OPTION_CLASS_H

#include "securities.h"

// Template for option class
// todo: make payoffs etc friend functions
// todo: - complex options should be defined as linear combinations of basic options
class Option
{
public:
	enum PayoffType
	{
		call,
		put,
	};

	enum ExerciseType
	{
		european,
		american,
		asian,
	};

	enum Position
	{
		longPosition,
		shortPosition,
	};

	constexpr Option(Option::PayoffType pay,
		Option::ExerciseType ex,
		Option::Position pos,
		Securities::Stock underlying = Securities::Stock(),
		double quantity = 1)
		: m_type{ pay }
		, m_etype{ ex }
		, m_position{ pos }
		, m_underlying{ underlying }
		, m_quantity{ quantity }
	{}

	explicit constexpr Option() = default;
	//double samplePayoff(double time = 0.);

	// setters
	void set_payoffType(Option::PayoffType pay) { m_type = pay; }
	void set_exerciseType(Option::ExerciseType ex) { m_etype = ex; }
	void set_position(Option::Position pos) { m_position = pos; }
	void set_quantity(double quant) { m_quantity = quant; }
	void set_underlying(Securities::Stock stock) { m_underlying = stock; }

	// getters
	Option::PayoffType get_payoffType() const { return m_type; }
	Option::ExerciseType get_exerciseType() const { return m_etype; }
	Option::Position get_position() const { return m_position; }
	double get_quantity() const { return m_quantity; }
	Securities::Stock get_underlying() const { return m_underlying; }

private:
	double m_strike{ 100. };
	double m_maturity{ 10. };
	double m_quantity{ 1. };
	PayoffType m_type{ call };
	ExerciseType m_etype{ european };
	Position m_position{ longPosition };
	Securities::Stock m_underlying{ Securities::Stock() };
};


// overload operators for Options
Option operator-(const Option& opt);
Option operator+(const Option& opt);
Option operator*(double quantity, const Option& opt);
Option operator*(const Option& opt, double quantity);



#endif
