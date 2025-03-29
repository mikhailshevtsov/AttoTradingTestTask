#include "FileGenerator.hpp"

#include <fstream>
#include <iomanip>

namespace Atto
{

bool FileGenerator::generate_file(const std::string& file_name)
{
    std::ofstream fout(file_name);
    if (!fout.is_open())
        return false;
    fout << std::setprecision(6);
    const std::size_t FILE_SIZE = 1000'000'000; // 1GB file
    while (fout.tellp() < FILE_SIZE)
    {
        double number = sign_dist(gen) * dist(gen); // generate random number and randomly flip its sign
        fout << number << "\n";
    }
    return true;
}

}
