# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.12

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/yujie6/Documents/Algorithm/DataStructure/TTRS/BplusTree

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/yujie6/Documents/Algorithm/DataStructure/TTRS/BplusTree/build

# Include any dependencies generated for this target.
include CMakeFiles/bptree.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/bptree.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/bptree.dir/flags.make

CMakeFiles/bptree.dir/test.cpp.o: CMakeFiles/bptree.dir/flags.make
CMakeFiles/bptree.dir/test.cpp.o: ../test.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/yujie6/Documents/Algorithm/DataStructure/TTRS/BplusTree/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/bptree.dir/test.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/bptree.dir/test.cpp.o -c /home/yujie6/Documents/Algorithm/DataStructure/TTRS/BplusTree/test.cpp

CMakeFiles/bptree.dir/test.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/bptree.dir/test.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/yujie6/Documents/Algorithm/DataStructure/TTRS/BplusTree/test.cpp > CMakeFiles/bptree.dir/test.cpp.i

CMakeFiles/bptree.dir/test.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/bptree.dir/test.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/yujie6/Documents/Algorithm/DataStructure/TTRS/BplusTree/test.cpp -o CMakeFiles/bptree.dir/test.cpp.s

# Object files for target bptree
bptree_OBJECTS = \
"CMakeFiles/bptree.dir/test.cpp.o"

# External object files for target bptree
bptree_EXTERNAL_OBJECTS =

bptree: CMakeFiles/bptree.dir/test.cpp.o
bptree: CMakeFiles/bptree.dir/build.make
bptree: CMakeFiles/bptree.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/yujie6/Documents/Algorithm/DataStructure/TTRS/BplusTree/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable bptree"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/bptree.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/bptree.dir/build: bptree

.PHONY : CMakeFiles/bptree.dir/build

CMakeFiles/bptree.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/bptree.dir/cmake_clean.cmake
.PHONY : CMakeFiles/bptree.dir/clean

CMakeFiles/bptree.dir/depend:
	cd /home/yujie6/Documents/Algorithm/DataStructure/TTRS/BplusTree/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/yujie6/Documents/Algorithm/DataStructure/TTRS/BplusTree /home/yujie6/Documents/Algorithm/DataStructure/TTRS/BplusTree /home/yujie6/Documents/Algorithm/DataStructure/TTRS/BplusTree/build /home/yujie6/Documents/Algorithm/DataStructure/TTRS/BplusTree/build /home/yujie6/Documents/Algorithm/DataStructure/TTRS/BplusTree/build/CMakeFiles/bptree.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/bptree.dir/depend
