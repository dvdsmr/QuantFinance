#ifndef SECURITIES_H
#define SECURITIES_H
#include "sdes.h"
#include <string_view>


namespace Securities
{
	class AbstractStock
	{
	public:
		virtual ~AbstractStock() = default; // Polymorphic base requires a virtual destructor

		virtual const double& getSpot() const = 0; // Pure virtual function to get the spot price
		virtual void setSpot(double spot) = 0;

		virtual void printInfo() const = 0;

		//virtual ParamsBase getParams() const = 0;

		// Add virtual methods for stochastic behavior
		virtual auto path(double terminalTime, std::size_t timePoints, double drift) -> XYVals = 0;
		virtual auto monteCarlo(double terminalTime, std::size_t samples, double drift) -> XYVals = 0;

	};

	class Stock : public AbstractStock
	{
	public:
		explicit constexpr Stock(double spot = 100.)
			: m_spot{ spot }
		{}

		constexpr void setSpot(double spot) { m_spot = spot; }
		constexpr const double& getSpot() const { return m_spot; }


		void printInfo() const;

		/*
		auto getParams() const -> ParamsBase { 
			throw std::runtime_error("No model params for Stock without stochastic model");
		}
		*/


		// Default implementation for stochastic behavior
		auto path([[maybe_unused]] double terminalTime, [[maybe_unused]] std::size_t timePoints, [[maybe_unused]] double drift) -> XYVals {
			throw std::runtime_error("path not supported for Stock without stochastic model");
		}

		auto monteCarlo([[maybe_unused]] double terminalTime, [[maybe_unused]] std::size_t samples, [[maybe_unused]] double drift) -> XYVals {
			throw std::runtime_error("monteCarlo not supported for Stock without stochastic model");
		}

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
		
		void printInfo() const;
		
		auto path(double terminalTime, std::size_t timePoints, double drift) -> XYVals { return SDE::path(getSpot(), terminalTime, timePoints, drift, m_params); }
		auto monteCarlo(double terminalTime, std::size_t samples, double drift) -> XYVals { return SDE::monteCarlo(getSpot(), terminalTime, samples, drift, m_params); }


	private:
		Params m_params{};
	};


	template <typename Params>
	void ModelStock<Params>::printInfo() const
	{
		std::cout << "Stock with initial spot price of $" << getSpot() << " following a ";
		if constexpr (std::is_same_v<Params, BSMParams>) {
			std::cout << "Black-Scholes-Merton ";
		}
		else if constexpr (std::is_same_v<Params, BachelierParams>) {
			std::cout << "Bachelier ";
		}
		else if constexpr (std::is_same_v<Params, MertonJumpParams>) {
			std::cout << "Merton Jump ";
		}
		else if constexpr (std::is_same_v<Params, CEVParams>) {
			std::cout << "CEV ";
		}
		else if constexpr (std::is_same_v<Params, HestonParams>) {
			std::cout << "Heston ";
		}
		else if constexpr (std::is_same_v<Params, VarianceGammaParams>) {
			std::cout << "Variance Gamma ";
		}
		else {
			std::cout << "<Unknown type> ";
		}
		std::cout << "model.\n";
	}

}




#endif
