// we replicate useful function from Python's numpy library for use in C++

#ifndef NUMPY_H
#define NUMPY_H

#include <vector>
#include <cmath>

namespace np
{
    template<typename T>
    std::vector<T> linspace(double start, double end, int num) 
    {
        std::vector<T> linspaced;
        double delta{};
        if (num > 1)
        {
            delta = static_cast<double>(end - start) / (num - 1);
        }
        else
        {
            delta = 1.;
        }
        for (int i = 0; i < num; ++i) {
            linspaced.push_back(static_cast<T>(start + i * delta));
        }
        return linspaced;
    }
}

#endif
