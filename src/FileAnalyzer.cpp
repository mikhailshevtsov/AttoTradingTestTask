#include "FileAnalyzer.hpp"

#include <fstream>
#include <iostream>
#include <cmath>

namespace Atto
{

bool FileAnalyzer::analyze_file(const std::string& file_name, const std::string& analytics_report_file_name)
{
    std::ifstream fin(file_name);
    if (!fin)
        return false;

    long double mean = 0.0;
    long double s = 0.0;   // sum of squared differences needed for standard deviation calculation
    std::size_t count = 0; // number of elements in the file

    long double number{};
    while (fin >> number)
    {
        ++count;
        long double old_mean = mean;
        long double diff = number - mean;
        mean += diff / count;
        s += (number - old_mean) * diff;
    }
    if (count == 0)
        return false;
    // standard deviation
    long double stddev = count > 1 ? std::sqrt(s / (count - 1)) : 0.0;

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