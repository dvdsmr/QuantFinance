#ifndef SECURITIES_H
#define SECURITIES_H
#include "sdes.h"
#include <string_view>


namespace Securities
{

	class Stock
	{
	public:
		explicit constexpr Stock(double spot = 100.)
			: m_spot{ spot }
		{}

		constexpr void setSpot(double spot) { m_spot = spot; }
		constexpr const double& getSpot() const { return m_spot; }

	private:
		double m_spot{ 100. };
	};

	// Stock class following a specified stochastic model.
	// The model is determied by the parameter struct passed to the class.
	template <typename Params>
	class ModelStock : public Stock
	{
	public:
		explicit constexpr ModelStock(double spot = 100.,
			Params params = {})
			: Stock{ spot }
			, m_params{ params }
		{}

		void setParams(Params params) { m_params = params; }
		Params getParams() const { return m_params; }
		auto path(double terminalTime, std::size_t timePoints, double drift) -> XYVals { return SDE::path(getSpot(), terminalTime, timePoints, drift, m_params); }
		auto monteCarlo(double terminalTime, std::size_t samples, double drift) -> XYVals { return SDE::monteCarlo(getSpot(), terminalTime, samples, drift, m_params); }


	private:
		Params m_params{};

	};


}




#endif
