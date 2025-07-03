cmake_minimum_required(VERSION 3.16)
project(OrgChartApiTests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(PkgConfig REQUIRED)
find_package(Drogon CONFIG REQUIRED)
find_package(GTest REQUIRED)
find_package(GMock REQUIRED)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../orgChartApi)

# Add test executable
file(GLOB_RECURSE TEST_SOURCES "*.cc" "*.cpp")
add_executable(unit_tests ${TEST_SOURCES})

# Link libraries
target_link_libraries(unit_tests 
    Drogon::Drogon
    GTest::GTest
    GTest::Main
    GMock::GMock
    GMock::Main
    gcov
)

# Enable testing
enable_testing()
add_test(NAME unit_tests COMMAND unit_tests)

# Coverage flags
target_compile_options(unit_tests PRIVATE --coverage)
target_link_options(unit_tests PRIVATE --coverage)
