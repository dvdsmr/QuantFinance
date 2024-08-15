#ifndef SAVING_H
#define SAVING_H
#include "stockpaths.h"
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

    void write_stockpath_to_csv(std::string filename, StockPath sPath) {
        // Make a CSV file with one or more columns of integer values
        // Each column of data is represented by the pair <column name, column data>
        //   as std::pair<std::string, std::vector<int>>
        // The dataset is represented as a vector of these columns
        // Note that all columns should be the same size

        // Create an output filestream object
        std::ofstream myFile(filename);

        // Send data to the stream
        for (int i = 0; i < sPath.m_length; ++i)
        {
            myFile << sPath.m_timeVals[static_cast<std::size_t>(i)]
                    << ","
                    << sPath.m_stockVals[static_cast<std::size_t>(i)]
                    << "\n";
        }

        // Close the file
        myFile.close();
    }

}


#endif
