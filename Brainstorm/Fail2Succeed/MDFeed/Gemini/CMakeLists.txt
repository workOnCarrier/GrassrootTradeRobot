# mkdir build
# cd build
# cmake ..
# cmake --build .. / make
cmake_minimum_required(VERSION 3.22)

# project(BuildSetupCheck VERSION 1.0 LANGUAGES CXX, Python)
## CMake Error at CMakeLists.txt:7 (project):
#  No CMAKE_Python_COMPILER could be found.

#  Tell CMake where to find the compiler by setting the CMake cache entry
#  CMAKE_Python_COMPILER to the full path to the compiler, or to the compiler
#  name if it is in the PATH.
project(GeminiMDFeedTest VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(Boost REQUIRED COMPONENTS system)
if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

add_executable(GeminiMDFeedTest sample_subscribe.cpp)
target_include_directories(GeminiMDFeedTest  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../Infrastructure/nlohmann_json/include
    Boost::system pthread
)
target_link_libraries(GeminiMDFeedTest 
    Boost::system pthread ssl crypto
)
get_target_property(GeminiMDFeedTest_INCLUDE_DIRS GeminiMDFeedTest INCLUDE_DIRECTORIES)
message(STATUS "GeminiMDFeedTest include dirs: ${GeminiMDFeedTest_INCLUDE_DIRS}")


add_executable(GeminiOrderBook book_snapshot_delta_subscribe.cpp)
target_include_directories(GeminiOrderBook   PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../Infrastructure/nlohmann_json/include
    Boost::system pthread
)
target_link_libraries(GeminiOrderBook  
    Boost::system pthread ssl crypto
)
get_target_property(GeminiOrderBook_INCLUDE_DIRS GeminiMDFeedTest INCLUDE_DIRECTORIES)
message(STATUS "GeminiOrderBook  include dirs: ${GeminiMDFeedTest_INCLUDE_DIRS}")


add_executable(GeminiParse parse_msg.cpp)
target_include_directories(GeminiParse    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../Infrastructure/nlohmann_json/include
    Boost::system pthread
)
target_link_libraries(GeminiParse      
    Boost::system pthread ssl crypto
)
get_target_property(GeminiParse_INCLUDE_DIRS GeminiMDFeedTest INCLUDE_DIRECTORIES)
message(STATUS "GeminiParse include dirs: ${GeminiMDFeedTest_INCLUDE_DIRS}")

# Set the options for the curl submodule before calling add_subdirectory.
# The options "BUILD_CURL_EXE" and "CURL_STATIC_LIBS" are important.
#
# CURL_STATIC_LIBS ON: This is very important. It tells curl to build the static
#                      library, which is often the easiest way to link it.
# BUILD_CURL_EXE OFF:  This prevents the 'curl' executable from being built,
#                      avoiding the error you saw.
set(CURL_STATIC_LIBS ON)
set(BUILD_CURL_EXE OFF)
set(CURL_USE_LIBPSL OFF)
add_executable(GeminiMarketData market_data.cpp)
add_subdirectory(../../../../Infrastructure/curl curlbuild)
target_include_directories(GeminiMarketData     PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../Infrastructure/nlohmann_json/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../Infrastructure/curl/include
    Boost::system pthread
)
target_link_libraries(GeminiMarketData PRIVATE
    Boost::system pthread ssl crypto libcurl
)
get_target_property(GeminiMarketData_INCLUDE_DIRS GeminiMDFeedTest INCLUDE_DIRECTORIES)
message(STATUS "GeminiMarketData include dirs: ${GeminiMDFeedTest_INCLUDE_DIRS}")


