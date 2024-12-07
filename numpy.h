// we replicate useful function from Python's numpy library for use in C++

#ifndef NUMPY_H
#define NUMPY_H

#include <vector>
#include <cmath>
#include <cassert>
#include <numeric>

namespace np
{
    template<typename T, typename N,
        typename = std::enable_if_t<std::is_integral<N>::value>> // allow only integral types for the stepsize
    std::vector<T> linspace(double start, double end, N num) 
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


    template <typename T>
    std::vector<T> abs(const std::vector<T>& vec) 
    {
        std::vector<T> result(vec.size());
        for (std::size_t i = 0; i < vec.size(); ++i) 
        {
            result[i] = std::abs(vec[i]);
        }
        return result;
    }

    template <typename T>
    std::vector<T> add(const std::vector<T>& vec1, const std::vector<T>& vec2, T factor1 = 1.0, T factor2 = 1.0)
    {
        assert(std::size(vec1) == std::size(vec2));
        std::vector<T> result(vec1.size());
        for (std::size_t i = 0; i < vec1.size(); ++i)
        {
            result[i] = factor1*vec1[i] + factor2*vec2[i];
        }
        return result;
    }

    template <typename T>
    std::vector<T> multiply(const std::vector<T>& vec1, const std::vector<T>& vec2)
    {
        assert(std::size(vec1) == std::size(vec2));
        std::vector<T> result(vec1.size());
        for (std::size_t i = 0; i < vec1.size(); ++i)
        {
            result[i] = vec1[i] * vec2[i];
        }
        return result;
    }

    template <typename T>
    std::vector<T> multiply(const std::vector<T>& vec, T factor)
    {
        std::vector<T> result(vec.size());
        for (std::size_t i = 0; i < vec.size(); ++i)
        {
            result[i] = factor * vec[i];
        }
        return result;
    }

    template <typename T>
    std::vector<T> multiply(T factor, const std::vector<T>& vec)
    {
        return multiply(vec, factor);
    }

    template <typename T>
    std::vector<T> minus(const std::vector<T>& vec)
    {
        std::vector<T> result(vec.size());
        for (std::size_t i = 0; i < vec.size(); ++i)
        {
            result[i] = -vec[i];
        }
        return result;
    }

    template <typename T>
    T mean(const std::vector<T>& vec)
    {
        T sum{ std::accumulate(std::begin(vec),std::end(vec),0.0) };
        return sum / static_cast<T>(std::size(vec));
    }

}

#endif
