#ifndef ATTO_FILESORTER_HPP
#define ATTO_FILESORTER_HPP

#include <string>
#include <vector>

namespace Atto
{

class FileSorter
{
public:
    bool sort_file(const std::string& unsorted_file_name, const std::string& sorted_file_name);
    
private:
    bool sort_phase(const std::string& unsorted_file_name, std::vector<std::string>& run_file_names);
    bool merge_phase(const std::string& sorted_file_name, const std::vector<std::string>& run_file_names);
};

}

#endif //ATTO_FILESORTER_HPP