#ifndef SAVING_H
#define SAVING_H
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
}


#endif
