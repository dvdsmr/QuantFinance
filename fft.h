#ifndef FFT_H
#define FFT_H
#include <vector>

namespace FFT
{
	auto fft(const auto& model, const std::vector<double>& params) -> std::vector<double>;
}

#endif
