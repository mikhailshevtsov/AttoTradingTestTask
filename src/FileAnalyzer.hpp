#ifndef ATTO_FILEANALYZER_HPP
#define ATTO_FILEANALYZER_HPP

#include <string>
#include <tuple>
#include <ostream>

namespace Atto
{

class FileAnalyzer
{
public:
    bool analyze_file(const std::string& file_name, const std::string& analytics_report_file_name);
    bool parallel_analyze_file(const std::string& file_name, const std::string& analytics_report_file_name);

private:
    void analyze_chunk(std::ifstream& stream, std::size_t end);

private:
    using percentile_indices = std::tuple<double, std::size_t, std::size_t>;
    percentile_indices calculate_percentile_indices(double percentile, std::size_t count);
    double calculate_percentile(const percentile_indices& indices, double lower_value, double upper_value);

private:
    void output_results(std::ostream& out, double mean, double stddev, double p50, double p99, double p01);
};

};

#endif //ATTO_FILEANALYZER_HPP