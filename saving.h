#ifndef SAVING_H
#define SAVING_H
#include "xyvals.h"
#include <fstream>
#include <vector>

namespace Saving
{
    template <typename T>
    void write_vector_to_file(const std::vector<T>& myVector, std::string filename);
    template <typename T>
    auto read_vector_from_file(std::string filename) -> std::vector<T>;
    void write_xyvals_to_csv(std::string filename, const XYVals& sPath);
    void write_table_to_csv(std::string filename, const DataTable& table);
    void write_labeledTable_to_csv(std::string filename, const LabeledTable& table);
}


#endif
