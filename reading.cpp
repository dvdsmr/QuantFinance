#include "reading.h"

namespace Reading
{

    std::vector<std::vector<std::string>> readCSV(const std::string& filename) {
        std::vector<std::vector<std::string>> data;
        std::ifstream file(filename);

        if (!file.is_open()) {
            std::cerr << "Error opening file!" << std::endl;
            return data;
        }

        std::string line;
        while (std::getline(file, line)) {
            std::vector<std::string> row;
            std::stringstream lineStream(line);
            std::string cell;

            // Parse each cell in the line
            while (std::getline(lineStream, cell, ',')) {
                row.push_back(cell);
            }

            data.push_back(row);
        }

        file.close();
        return data;
    }
}
