#include "FileSorter.hpp"

#include <fstream>
#include <vector>
#include <ranges>
#include <algorithm>
#include <iomanip>
#include <queue>
#include <filesystem>

namespace Atto
{

bool FileSorter::sort_file(const std::string& unsorted_file_name, const std::string& sorted_file_name)
{
    std::vector<std::string> run_file_names;
    if (!sort_phase(unsorted_file_name, run_file_names))
        return false;
    if (!merge_phase(sorted_file_name, run_file_names))
        return false;
    return true;
}

bool FileSorter::sort_phase(const std::string& unsorted_file_name, std::vector<std::string>& run_file_names)
{
    const std::size_t FILE_SIZE = 1000'000'000; // total file size
    const std::size_t MEMORY_USAGE = 100'000'000; // memory usage limit

    run_file_names.reserve(FILE_SIZE / MEMORY_USAGE); // preallocating buffer for storing temporary file names

    std::ifstream fin(unsorted_file_name);
    if (!fin)
        return false;

    std::vector<double> numbers;
    numbers.reserve(MEMORY_USAGE / sizeof(double)); // preallocating buffer for storing numbers

    double number{};
    while (fin)
    {
        while (numbers.size() < numbers.capacity() && fin >> number)
            numbers.push_back(number);

        std::ranges::sort(numbers);

        // generate temporary file location
        std::string run_file_name;
        do run_file_name = std::filesystem::temp_directory_path() / ("file_sorter_" + std::to_string(rand()));
        while (std::filesystem::exists(run_file_name));
        run_file_names.emplace_back(run_file_name);

        // open file in binary mode
        std::ofstream fout(run_file_name, std::ios::binary);
        if (!fout.is_open())
            return false;

        for (auto number : numbers)
            fout.write(reinterpret_cast<char*>(&number), sizeof(number));

        // clear the buffer for next chunk
        numbers.clear();
    }

    return true;
}

bool FileSorter::merge_phase(const std::string& sorted_file_name, const std::vector<std::string>& run_file_names)
{
    // structure for min-heap
    struct FileEntry
    {
        double number{};
        std::ifstream* stream{}; // file from which the number was read (needed for speed access)
        bool operator>(const FileEntry& other) const noexcept { return number > other.number; }
        FileEntry(double number = 0.0, std::ifstream* stream = nullptr) noexcept : number{number}, stream{stream} {}
    };

    std::vector<std::ifstream> run_files(run_file_names.size());
    std::priority_queue<FileEntry, std::vector<FileEntry>, std::greater<>> min_heap;

    for (std::size_t i = 0; i < run_files.size(); ++i)
    {
        // open temporary file in binary mode
        run_files[i].open(run_file_names[i], std::ios::binary);
        if (!run_files[i].is_open())
            return false;
        
        // read the first number from the file and add it to the min-heap
        double number{};
        run_files[i].read(reinterpret_cast<char*>(&number), sizeof(number));
        if (run_files[i])
            min_heap.emplace(number, &run_files[i]);
    }

    std::ofstream fout(sorted_file_name);
    fout << std::setprecision(6);
    if (!fout.is_open())
        return false;

    // merging
    while (!min_heap.empty())
    {
        FileEntry min = min_heap.top();
        min_heap.pop();

        fout << min.number << "\n";
        
        // read the next number from the file and add it to the min-heap
        double number{};
        min.stream->read(reinterpret_cast<char*>(&number), sizeof(number));
        if (*(min.stream))
            min_heap.emplace(number, min.stream);
    }

    // remove all temporary files
    for (const auto& run_file_name : run_file_names)
        std::filesystem::remove(run_file_name);

    return true;
}

}