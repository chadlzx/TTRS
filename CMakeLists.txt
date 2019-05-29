#project name
cmake_minimum_required(VERSION 3.10)
PROJECT(BplusTree LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 14)
#head file path
#INCLUDE_DIRECTORIES(.)
#AUX_SOURCE_DIRECTORY(src ../data/one)
#MESSAGE("Add path successfully.")
#SET(VECTOR ${DIR_SRCS})
#SET(BIN_DIR ../bin)
#MESSAGE("Set environmental variables successfully.")
#ADD_EXECUTABLE(test ./debug.cpp)
ADD_EXECUTABLE(BplusTree ./test.cpp ./NewBptree.hpp ./Bptree.hpp ./alloc.h ./exceptions.hpp ./alloc.cpp)
