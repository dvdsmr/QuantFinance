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


	// TO BE REMOVED
	class SimpleStock : public Stock
	{
		// This is a class of modelled stocks which follow GBM/Bachelier.
		// The stock's possible sample paths are entirely determined
		// by its starting value, its drift and its volatility
	public:
		// Constructor
		explicit constexpr SimpleStock(double spot = 100.,
			double drift = 0.1,
			double vol = 0.05)
			: Stock{ spot }
			, m_drift{ drift }
			, m_vol{ vol }
		{}
		// Setters
		constexpr void setDrift(double drift) { m_drift = drift; }
		constexpr void setVol(double vol) { m_vol = vol; }
		// Getters
		constexpr const double& getDrift() const { return m_drift; }
		constexpr const double& getVol() const { return m_vol; }

		double samplePrice(double time = 0., std::string_view model = "bsm");

	private:
		double m_drift{ 0.1 };
		double m_vol{ 0.05 };

	};


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
		XYVals path(double terminalTime, std::size_t timePoints, double drift) { return SDE::path(getSpot(), terminalTime, timePoints, drift, m_params);}


	private:
		Params m_params{};

	};


}




#endif
