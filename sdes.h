#ifndef SDES_H
#define SDES_H

namespace SDE
{
	auto OrnsteinUhlenbeckStep(double state, double stepSize, double drift, double diffusion) -> double;
	auto geometricBrownianMotion(double initialState, double time, double drift, double volatility) -> double;
}

#endif
