#ifndef OPTION_RETURNS_H
#define OPTION_RETURNS_H
#include "out.h"
#include "sdes.h"
#include "securities.h"
#include "saving.h"
#include "distributions.h"
#include "xyvals.h"
#include "numpy.h"
#include "fft.h"
#include <functional>


namespace Options
{
	namespace Payoffs
	{

		enum class Type
		{
			call,
			put,
			straddle,
			strangle,
			callDebitSpread,
			callCreditSpread,
			putDebitSpread,
			putCreditSpread,
		};

		auto call(double strikePrice, double spotPrice) -> double;
		auto put(double strikePrice, double spotPrice) -> double;
		auto straddle(double strikePrice, double spotPrice) -> double;
		auto strangle(double strikeCall, double strikePut, double spotPrice) -> double;
		auto callDebitSpread(double lower, double higher, double spotPrice) -> double;
		auto callCreditSpread(double lower, double higher, double spotPrice) -> double;
		auto putDebitSpread(double lower, double higher, double spotPrice) -> double;
		auto putCreditSpread(double lower, double higher, double spotPrice) -> double;
	}

	namespace Pricing
	{
		namespace BinomialOneStep
		{
			auto call(double riskFreeRate, double upTick, double strike, double spot, double dividendYield=0.) -> double;
			auto put(double riskFreeRate, double upTick, double strike, double spot, double dividenYield=0.) -> double;
			auto callGrid(double riskFreeRate, double upTick, double strike, double spot, int length, double dividendYield) -> PriceGrid;

		}
		namespace BSM
		{
			auto _d1(double riskFreeReturn, double vol, double maturity, double strike, double spot, double dividendYield) -> double;
			auto _d2(double riskFreeReturn, double vol, double maturity, double strike, double spot, double dividendYield) -> double;

			auto call(double riskFreeReturn, double vol, double maturity, double strike, double spot, double dividendYield=0.) -> double;
			auto put(double riskFreeReturn, double vol, double maturity, double strike, double spot, double dividendYield=0.) -> double;

			auto callDelta(double riskFreeReturn, double vol, double maturity, double strike, double spot, double dividendYield) -> double;
			auto putDelta(double riskFreeReturn, double vol, double maturity, double strike, double spot, double dividendYield) -> double;

			auto callGamma(double riskFreeReturn, double vol, double maturity, double strike, double spot, double dividendYield) -> double;
			auto putGamma(double riskFreeReturn, double vol, double maturity, double strike, double spot, double dividendYield) -> double;

			auto callVega(double riskFreeReturn, double vol, double maturity, double strike, double spot, double dividendYield) -> double;
			auto putVega(double riskFreeReturn, double vol, double maturity, double strike, double spot, double dividendYield) -> double;

			auto callTheta(double riskFreeReturn, double vol, double maturity, double strike, double spot, double dividendYield) -> double;
			auto putTheta(double riskFreeReturn, double vol, double maturity, double strike, double spot, double dividendYield) -> double;

			auto callStrikeDerivative(double riskFreeReturn, double vol, double maturity, double strike, double spot, double dividendYield) -> double;
			auto callStrikeDerivativeApprox(double riskFreeReturn, double vol, double maturity, double strike, double spot, double dividendYield) -> double;
			auto callStrikeSpotDerivativeApprox(double riskFreeReturn, double vol, double maturity, double strike, double spot, double dividendYield) -> double;

			auto monteCarlo(const std::function<double(double)>& payoff, double riskFreeReturn, double vol, double maturity, double spot, double dividendYield) -> double;
			void testMonteCarlo();

			namespace DataGeneration
			{
				auto call(double riskFreeReturn, double vol, double maturity, double strike, double dividendYield) -> DataTable;
				auto callPriceSurface(double riskFreeReturn, double vol, double spot, double dividendYield) -> LabeledTable;
			}
		}

		namespace Bachelier
		{
			auto _dplus(double riskFreeReturn, double vol, double maturity, double strike, double spot, double dividendYield) -> double;

			auto call(double riskFreeReturn, double vol, double maturity, double strike, double spot, double dividendYield = 0.) -> double;
			auto put(double riskFreeReturn, double vol, double maturity, double strike, double spot, double dividendYield = 0.) -> double;

			auto callVega(double riskFreeReturn, double vol, double maturity, double strike, double spot, double dividendYield) -> double;
			auto putVega(double riskFreeReturn, double vol, double maturity, double strike, double spot, double dividendYield) -> double;

			auto monteCarlo(const std::function<double(double)>& payoff, double riskFreeReturn, double vol, double maturity, double spot, double dividendYield) -> double;
			void testMonteCarlo();
		}

		namespace CEV
		{
			auto _nu(double exponent) -> double;
			// WARNING: CEV PRICING FORMULAS ARE HIGHLY UNSTABLE
			auto call(double riskFreeReturn, double vol, double maturity, double strike, double spot, double dividendYield, double exponent) -> double;
			auto put(double riskFreeReturn, double vol, double maturity, double strike, double spot, double dividendYield, double exponent) -> double;
			auto monteCarlo(const std::function<double(double)>& payoff, double riskFreeReturn, double vol, double maturity, double spot, double dividendYield, double exponent) -> double;
			void test();
			void testMonteCarlo();
		}

		namespace MertonJump
		{
			auto monteCarlo(const std::function<double(double)>& payoff, double riskFreeReturn, double maturity, double spot, double dividendYield, double volatility, double meanJumpSize, double stdJumpSize, double expectedJumpsPerYear) -> double;
			auto fft(double strike, double riskFreeReturn, double maturity, double spot, double dividendYield, double volatility, double meanJumpSize, double stdJumpSize, double expectedJumpsPerYear, std::string_view type = "call") -> double;
			void testPricing();
		}

		namespace Heston
		{
			auto monteCarlo(const std::function<double(double)>& payoff, double riskFreeReturn, double maturity, double spot, double dividendYield, double initialVariance, double longVariance, double correlation, double reversionRate, double volVol) -> double;
			auto fft(double strike, double riskFreeReturn, double maturity, double spot, double dividendYield, double initialVariance, double longVariance, double correlation, double reversionRate, double volVol, std::string_view type = "call") -> double;
			void testPricing();
		}

		namespace VarianceGamma
		{
			auto monteCarlo(const std::function<double(double)>& payoff, double riskFreeReturn, double maturity, double spot, double dividendYield,double gammaDrift, double variance, double vol) -> double;
			auto fft(double strike, double riskFreeReturn, double maturity, double spot, double dividendYield, double gammaDrift, double variance, double vol, std::string_view type = "call") -> double;
			void testPricing();
		}

		namespace Utils
		{
			auto _discountedExpectedPayoff(const std::function<double(double)>& payoff, std::vector<double> predictedSpots, double riskFreeReturn, double maturity) -> double;
		}


	}
	
	const void testCallGrid();
	const void savePriceSurface();
	const void testPricing();
	void strangleUnitTest();
	void callCreditSpreadUnitTest();
	void binomialPricingUnitTest();
}

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
					 SimpleStock underlying=SimpleStock(),
					 double quantity=1)
		: m_type{pay}
		, m_etype{ex}
		, m_position{pos}
		, m_underlying{underlying}
		, m_quantity{quantity}
	{}

	explicit constexpr Option() = default;
	double samplePayoff(double time = 0.);

	// setters
	void set_payoffType(Option::PayoffType pay) { m_type = pay; }
	void set_exerciseType(Option::ExerciseType ex) { m_etype = ex; }
	void set_position(Option::Position pos) { m_position = pos; }
	void set_quantity(double quant) { m_quantity = quant; }
	void set_underlying(SimpleStock stock) { m_underlying = stock; }

	// getters
	Option::PayoffType get_payoffType() const { return m_type; }
	Option::ExerciseType get_exerciseType() const { return m_etype; }
	Option::Position get_position() const { return m_position; }
	double get_quantity() const { return m_quantity; }
	SimpleStock get_underlying() const { return m_underlying; }

private:
	double m_strike{ 100. };
	double m_maturity{ 10. };
	double m_quantity{ 1. };
	PayoffType m_type{ call };
	ExerciseType m_etype{ european };
	Position m_position{ longPosition };
	SimpleStock m_underlying{ SimpleStock() };
};


// overload operators for Options
Option operator-(const Option& opt);
Option operator+(const Option& opt);
Option operator*(double quantity, const Option& opt);
Option operator*(const Option& opt, double quantity);




#endif
