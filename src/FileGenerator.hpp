#ifndef ATTO_FILEGENERATOR_HPP
#define ATTO_FILEGENERATOR_HPP

#include <string>
#include <random>
#include <numeric>

namespace Atto
{

class FileGenerator
{
public:
    bool generate_file(const std::string& file_name);

private:
    std::random_device rand;
    std::mt19937 gen{rand()};
    std::uniform_real_distribution<double> dist{
        std::numeric_limits<double>::min(),
        std::numeric_limits<double>::max()
    };
    // since the range contains only positive values their sign must be inverted randomly
    std::uniform_int_distribution<int> sign_dist{-1, 1};
};

};

#endif //ATTO_FILEGENERATOR_HPP