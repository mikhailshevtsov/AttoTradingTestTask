# Atto Trading Test Task

## FileGenerator

### Algorithm Selection

The FileGenerator class utilizes straightforward approach to generate a large dataset. It sequentially writes floating point values randomly generated with uniform distribution to a file until a target size is reached.

### Optimization Techniques

Random numbers are generated on the fly using modern C++ random number generation library. The program uses std::ostream::tellp() to effectively monitor file size.

### Time and Space Complexity

The algorithm runs in O(N) time, where N is the number of iterations needed to reach the target file size, and O(1) space, as only one number is held in memory at any moment.

## FileSorter

### Algorithm Selection

The FileSorter class implements External Merge Sort, a two-phase algorithm designed for sorting large files that exceed available memory. This approach was chosen for its ability to break the sorting process of large file into sorting of smaller chunks that fit into memory.

### Optimization Techniques

The implementation sorts data in chunks that fit into memory using std::ranges::sort, which applies an efficient sorting strategy. Each sorted chunk is stored in a temporary binary file, reducing disk space usage and enhancing I/O efficiency. During merging, a min-heap (priority queue) ensures that only the smallest element from each run file is processed at a time, reducing memory overhead. Pre-allocating std::vector's minimizes dynamic memory allocations, optimizing memory usage further.

### Time and Space Complexity

Sort Phase:
The standard sorting works in O(M log M) time, where M is the number of elements that fit into memory (inside the individual chunk). Thus overall sorting operates in O(N / M * M log M) = O(N log M) time, where N is the total number of elements, M is the number of elements that fit into memory, N / M - number of chunks. Since each chunk is sorted separately, memory consumption remains at O(M).

Merge Phase:
Merging runs in O(N log (N / M)) time, where N / M is the number of temporary run files, since the operation of inserting and removing an element from min-heap is O(log (N / M)) and it is performed N times (once per element). The priority queue maintains O(N / M) elements at any time.

Overall Complexity:
The combined complexity of sorting and merging results in O(N log M + N log (N / M)) = O(N log N) time in the worst case, while space usage is O(M + N / M) which is approximately O(M) due to N / M << M in our case.


## FileAnalyzer

### Algorithm Selection

The FileAnalyzer class computes key statistical metrics, including mean, standard deviation, and percentiles. Its implementation employs a parallelized version of Welford’s algorithm, because of its efficiency, numerical stability and suitability for large datasets. This method allows for computing mean and standard deviation incrementally in a single pass, avoiding the need to load all values into memory. Percentile values are computed separately using a second pass with index-based interpolation between adjacent data points.

### Optimization Techniques

The analysis is divided into two stages. In the first stage, the file is split into equal-sized chunks, and each chunk is processed by a separate thread. Each thread scans its part of the file and computes local statistics using Welford’s algorithm. Once all threads complete, their partial results are merged into a global result using a numerically stable formula. Percentiles are determined in the second stage by computing precise index positions and interpolating values between adjacent data points. The implementation also employs stream-based file processing to handle large dataset without loading it into memory.


### Time and Space Complexity

Mean and Standard Deviation Calculation:
This phase runs in O(N) time with O(1) space since it requires a single scan of the file while maintaining only a few variables.

Percentile Computation:
Finding percentiles requires O(N) time for an additional scan. Space complexity remains O(1) as only a few values are stored at a time.

Overall Complexity:
Since both phases execute in O(N) time with O(1) space, the overall time and space complexities remain O(N) and O(1) respectively.

## Instructions for building and running

To build and run the programs you must either download the source code archive or clone this repository to local directory with the git clone command.
You will also need to install CMake to successfully build the project.
After that, you need to run the following commands:
```
cd <your_local_directory>
mkdir build && cd build
cmake ..
cmake --build .
```

To run each program you must type:
```
./file_generator <name_of_file>
./file_sorter <name_of_unsorted_file> <name_of_sorted_file>
./file_analyzer <name_of_sorted_file> <name_of_analytics_output_report>
```