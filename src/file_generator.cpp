#include <iostream>

#include "FileGenerator.hpp"

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: ./file_generator <name_of_file>" << std::endl;
        return -1;
    }

    Atto::FileGenerator file_generator;

    std::cout << "Started generating a file" << std::endl;

    bool result = file_generator.generate_file(argv[1]);
    if (result)
        std::cout << "File was successfully generated at location " << argv[1] << std::endl;
    else
        std::cout << "Failed to generate a file" << std::endl;

    return 0;
}