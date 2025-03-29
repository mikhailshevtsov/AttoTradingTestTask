#include <iostream>

#include "FileSorter.hpp"

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cout << "Usage: ./file_sorter  <name_of_unsorted_file> <name_of_sorted_file>" << std::endl;
        return -1;
    }

    Atto::FileSorter file_sorter;

    std::cout << "Started sorting a file" << std::endl;

    bool result = file_sorter.sort_file(argv[1], argv[2]);

    if (result)
        std::cout << "File was successfully sorted and saved at location " << argv[2] << std::endl;
    else
        std::cout << "Failed to sort a file" << std::endl;

    return 0;
}