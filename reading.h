#ifndef READING_H
#define READING_H
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

namespace Reading
{
    std::vector<std::vector<std::string>> readCSV(const std::string& filename);
}

#endif