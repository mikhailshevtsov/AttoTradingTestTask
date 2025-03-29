cmake_minimum_required(VERSION 3.28)
project(Atto LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

add_executable(file_generator)
target_sources(file_generator
    PRIVATE
    src/file_generator.cpp
    src/FileGenerator.cpp
)

add_executable(file_sorter)
target_sources(file_sorter
    PRIVATE
    src/file_sorter.cpp
    src/FileSorter.cpp
)

add_executable(file_analyzer)
target_sources(file_analyzer
    PRIVATE
    src/file_analyzer.cpp
    src/FileAnalyzer.cpp
)