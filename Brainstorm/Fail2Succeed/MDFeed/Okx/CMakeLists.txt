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
project(OkxMdsSubscribe VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(Boost REQUIRED COMPONENTS system)
if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

add_executable(OkxMdsSubscribe sample_subscribe.cpp)
target_include_directories(OkxMdsSubscribe  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../Infrastructure/nlohmann_json/include
    Boost::system pthread
)
target_link_libraries(OkxMdsSubscribe 
    Boost::system pthread ssl crypto
)
get_target_property(GeminiMDFeedTest_INCLUDE_DIRS OkxMdsSubscribe INCLUDE_DIRECTORIES)
message(STATUS "OkxMdsSubscribe include dirs: ${GeminiMDFeedTest_INCLUDE_DIRS}")

