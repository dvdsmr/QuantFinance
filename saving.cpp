#include "saving.h"
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

    void write_table_to_csv(std::string filename, const DataTable& table) {
        // Create an output filestream object
        std::ofstream myFile(filename);

        // Send data to the stream
        for (std::size_t i{ 0 }; i < table.m_numRows; ++i)
        {
            for (std::size_t j{ 0 }; j < table.m_numCols; ++j)
            {
                myFile << table.m_table[i][j];
                if (j < table.m_numCols - 1)
                    myFile << ",";
            }
            myFile << "\n";
        }

        // Close the file
        myFile.close();
    }

    void write_labeledTable_to_csv(std::string filename, const LabeledTable& table) {
        // Create an output filestream object
        std::ofstream myFile(filename);

        // send labels to the stream for the first row
        myFile << table.m_tableName << "," << table.m_rowLabel << "," << table.m_colLabel << "," << table.m_tableLabel << "\n";

        // send xVals to the stream
        myFile << ","; // first entry of table is empty
        for (std::size_t i{ 0 }; i < table.m_numCols; ++i)
        {
            myFile << table.m_colVals[i];
            if (i < table.m_numCols - 1)
                myFile << ",";
        }
        myFile << "\n";

        // Send yVals and table data to the stream
        for (std::size_t i{ 0 }; i < table.m_numRows; ++i)
        {
            myFile << table.m_rowVals[i];
            myFile << ",";
            for (std::size_t j{ 0 }; j < table.m_numCols; ++j)
            {
                myFile << table.m_table[i][j];
                if (j < table.m_numCols - 1)
                    myFile << ",";
            }
            myFile << "\n";
        }

        // Close the file
        myFile.close();
    }

}