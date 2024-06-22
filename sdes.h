#ifndef SDES_H
#define SDES_H

namespace SDE
{
	auto OrnsteinUhlenbeckStep(double state, double stepSize, double drift, double diffusion) -> double;
}

#endif
