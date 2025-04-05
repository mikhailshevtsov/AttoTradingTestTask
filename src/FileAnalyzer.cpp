#include "FileAnalyzer.hpp"

#include <fstream>
#include <iostream>
#include <cmath>
#include <vector>
#include <thread>
#include <filesystem>

namespace Atto
{

bool FileAnalyzer::analyze_file(const std::string& file_name, const std::string& analytics_report_file_name)
{
    std::ifstream fin(file_name);
    if (!fin.is_open())
        return false;

    long double mean = 0.0;
    long double sum = 0.0; // sum of squared differences needed for standard deviation calculation
    std::size_t count = 0; // number of elements in the file

    long double number = 0.0;
    while (fin >> number)
    {
        ++count;
        long double old_mean = mean;
        long double delta = number - mean;
        mean += delta / count;
        sum += (number - old_mean) * delta;
    }
    if (count == 0)
        return false;
    // standard deviation
    long double stddev = count > 1 ? std::sqrt(sum / (count - 1)) : 0.0;

    // return to the beginning of the file
    fin.clear();
    fin.seekg(0);

    // percentiles
    double p01 = 0.0; // 1st percentile
    double p50 = 0.0; // 50th percentile (median)
    double p99 = 0.0; // 99th percentile

    // calculate positions of percentiles
    auto indices_p01 = calculate_percentile_indices(0.01, count);
    auto indices_p50 = calculate_percentile_indices(0.50, count);
    auto indices_p99 = calculate_percentile_indices(0.99, count);

    double lower_number = 0.0;
    double upper_number = 0.0;
    std::size_t index = 0;
    while (fin >> upper_number && index < count)
    {
        if (index == std::get<2>(indices_p01))
            p01 = calculate_percentile(indices_p01, lower_number, upper_number);
        if (index == std::get<2>(indices_p50))
            p50 = calculate_percentile(indices_p50, lower_number, upper_number);
        if (index == std::get<2>(indices_p99))
        {
            p99 = calculate_percentile(indices_p99, lower_number, upper_number);
            // calculated the last percentile -> exit the loop
            break;
        }

        lower_number = upper_number;
        ++index;
    }

    std::ofstream fout(analytics_report_file_name);
    if (!fout.is_open())
        return false;
    
    output_results(fout, mean, stddev, p50, p99, p01);
    output_results(std::cout, mean, stddev, p50, p99, p01);

    return true;
}

bool FileAnalyzer::parallel_analyze_file(const std::string& file_name, const std::string& analytics_report_file_name)
{
    struct ThreadEntry
    {
        std::jthread thread;
        long double mean = 0.0;
        long double sum = 0.0;
        std::size_t count = 0;
    };

    const std::size_t FILE_SIZE = std::filesystem::file_size(file_name);
    const std::size_t CHUNKS = std::thread::hardware_concurrency();
    const std::size_t CHUNK_SIZE = FILE_SIZE / CHUNKS;
    std::vector<ThreadEntry> entries(CHUNKS);

    for (std::size_t i = 0; i < CHUNKS; ++i)
    {
        std::size_t begin = i * CHUNK_SIZE;
        std::size_t end = (i == CHUNKS - 1) ? FILE_SIZE : begin + CHUNK_SIZE;
        entries[i].thread = std::jthread(
        [i, &entries](std::size_t begin, std::size_t end, const std::string& file_name)
        {
            std::ifstream fin(file_name);
            if (!fin.is_open())
                return false;
            fin.seekg(begin);

            long double number = 0.0;
            long double mean = 0.0;
            long double sum = 0.0; // sum of squared differences needed for standard deviation calculation
            std::size_t count = 0; // number of elements in the chunk

            // skip a boundary number
            if (i != 0)
                fin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            
            while (fin && fin.tellg() < end && (fin >> number))
            {
                ++count;
                long double old_mean = mean;
                long double delta = number - mean;
                mean += delta / count;
                sum += (number - old_mean) * delta;
            }
            entries[i].mean = mean;
            entries[i].sum = sum;
            entries[i].count = count;
            return true;
        }, begin, end, file_name);
    }
    for (auto& entry : entries)
        entry.thread.join();

    // merge results to calculate mean and standard deviation
    long double mean = 0.0;
    long double sum = 0.0;
    std::size_t count = 0;
    for (const auto& entry : entries)
    {
        if (entry.count == 0)
            continue;
        if (count == 0)
        {
            mean = entry.mean;
            sum = entry.sum;
            count = entry.count;
        }
        else
        {
            std::size_t next_count = count + entry.count;
            long double delta = entry.mean - mean;
            mean += (delta * entry.count) / next_count;
            sum += entry.sum + (delta * delta * count * entry.count) / next_count;
            count = next_count;
        }
    }
    if (count == 0)
        return false;
    long double stddev = count > 1 ? std::sqrt(sum / (count - 1)) : 0.0; // standard deviation

    std::ifstream fin(file_name);
    if (!fin.is_open())
        return false;

    // percentiles
    double p01 = 0.0; // 1st percentile
    double p50 = 0.0; // 50th percentile (median)
    double p99 = 0.0; // 99th percentile

    // calculate positions of percentiles
    auto indices_p01 = calculate_percentile_indices(0.01, count);
    auto indices_p50 = calculate_percentile_indices(0.50, count);
    auto indices_p99 = calculate_percentile_indices(0.99, count);

    double lower_number = 0.0;
    double upper_number = 0.0;
    std::size_t index = 0;
    while (fin >> upper_number && index < count)
    {
        if (index == std::get<2>(indices_p01))
            p01 = calculate_percentile(indices_p01, lower_number, upper_number);
        if (index == std::get<2>(indices_p50))
            p50 = calculate_percentile(indices_p50, lower_number, upper_number);
        if (index == std::get<2>(indices_p99))
        {
            p99 = calculate_percentile(indices_p99, lower_number, upper_number);
            // calculated the last percentile -> exit the loop
            break;
        }

        lower_number = upper_number;
        ++index;
    }

    std::ofstream fout(analytics_report_file_name);
    if (!fout.is_open())
        return false;
    
    output_results(fout, mean, stddev, p50, p99, p01);
    output_results(std::cout, mean, stddev, p50, p99, p01);

    return true;
}

FileAnalyzer::percentile_indices FileAnalyzer::calculate_percentile_indices(double percentile, std::size_t count)
{
    double exact_index = percentile * (count - 1);
    std::size_t lower_index = static_cast<std::size_t>(std::floor(exact_index));
    std::size_t upper_index = static_cast<std::size_t>(std::ceil(exact_index));
    return {exact_index, lower_index, upper_index};
}

double FileAnalyzer::calculate_percentile(const percentile_indices& indices, double lower_value, double upper_value)
{
    auto [exact_index, lower_index, upper_index] = indices;
    if (lower_index != upper_index)
    {
        // find linear interpolation between lower_value and upper_value based on exact_index
        double weight = exact_index - lower_index;
        return lower_value * (1.0 - weight) + upper_value * weight;
    }
    else
        return upper_value;
}

void FileAnalyzer::output_results(std::ostream& out, double mean, double stddev, double p50, double p99, double p01)
{
    out << "Mean: " << mean << "\n";
    out << "Standard deviation: " << stddev << "\n";
    out << "Median: " << p50 << "\n";
    out << "99th percentile: " << p99 << "\n";
    out << "1st percentile: " << p01 << std::endl;
}

};