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
project(BuildSetupCheck VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23) 

add_executable(hw hw.cpp)