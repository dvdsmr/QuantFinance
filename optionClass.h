#ifndef OPTION_CLASS_H
#define OPTION_CLASS_H

#include "securities.h"
#include "options.h"

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


	const Option(Option::PayoffType pay,
		Option::ExerciseType ex,
		Option::Position pos,
		std::shared_ptr<Securities::AbstractStock> underlying,
		double quantity = 1)
		: m_type{ pay }
		, m_etype{ ex }
		, m_position{ pos }
		, m_underlying{ underlying }
		, m_quantity{ quantity }
	{}

	// disable default constructor
	explicit constexpr Option() = delete;
	//double samplePayoff(double time = 0.);

	// setters
	void set_payoffType(Option::PayoffType pay) { m_type = pay; }
	void set_exerciseType(Option::ExerciseType ex) { m_etype = ex; }
	void set_position(Option::Position pos) { m_position = pos; }
	void set_quantity(double quant) { m_quantity = quant; }

	// getters
	auto get_payoffType() const -> Option::PayoffType { return m_type; }
	auto get_exerciseType() const -> Option::ExerciseType { return m_etype; }
	auto get_position() const -> Option::Position { return m_position; }
	auto get_quantity() const -> double { return m_quantity; }
	auto get_underlying() const -> const std::shared_ptr<Securities::AbstractStock>& { return m_underlying; }

	// functionality
	void printInfo();
	auto price(double riskFreeReturn, double dividendYield) -> double;

private:
	double m_strike{ 100. };
	double m_maturity{ 10. };
	double m_quantity{ 1. };
	PayoffType m_type{ call };
	ExerciseType m_etype{ european };
	Position m_position{ longPosition };
	const std::shared_ptr<Securities::AbstractStock> m_underlying;
};


// overload operators for Options
Option operator-(const Option& opt);
Option operator+(const Option& opt);
Option operator*(double quantity, const Option& opt);
Option operator*(const Option& opt, double quantity);





#endif
