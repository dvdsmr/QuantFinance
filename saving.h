#ifndef SAVING_H
#define SAVING_H
#include "xyvals.h"
#include <fstream>
#include <vector>

namespace Saving
{
    template <typename T>
    void write_vector_to_file(const std::vector<T>& myVector, std::string filename)
    {
        std::ofstream ofs(filename, std::ios::out | std::ofstream::binary);
        std::ostream_iterator<char> osi{ ofs };
        const char* beginByte = (char*)&myVector[0];

        const char* endByte = (char*)&myVector.back() + sizeof(T);
        std::copy(beginByte, endByte, osi);
    }

    template <typename T>
    std::vector<T> read_vector_from_file(std::string filename)
    {
        std::vector<char> buffer{};
        std::ifstream ifs(filename, std::ios::in | std::ifstream::binary);
        std::istreambuf_iterator<char> iter(ifs);
        std::istreambuf_iterator<char> end{};
        std::copy(iter, end, std::back_inserter(buffer));
        std::vector<T> newVector(buffer.size() / sizeof(T));
        memcpy(&newVector[0], &buffer[0], buffer.size());
        return newVector;
    }

    void write_xyvals_to_csv(std::string filename, const XYVals& sPath) {
        // Create an output filestream object
        std::ofstream myFile(filename);

        // Send data to the stream
        for (std::size_t i{ 0 }; i < sPath.m_length; ++i)
        {
            myFile << sPath.m_xVals[i]
                    << ","
                    << sPath.m_yVals[i]
                    << "\n";
        }

        // Close the file
        myFile.close();
    }

}


#endif
