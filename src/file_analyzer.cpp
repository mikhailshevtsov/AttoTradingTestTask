#include <iostream>

#include "FileAnalyzer.hpp"

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cout << "Usage: ./file_analyzer <name_of_sorted_file> <name_of_analytics_output_report>" << std::endl;
        return -1;
    }

    Atto::FileAnalyzer file_analyzer;

    std::cout << "Started analyzing a file" << std::endl;

    bool result = file_analyzer.parallel_analyze_file(argv[1], argv[2]);

    if (result)
        std::cout << "File was successfully analyzed. Analytics report was saved at location " << argv[2] << std::endl;
    else
        std::cout << "Failed to analyze a file" << std::endl;

    return 0;
}