# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.18

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Disable VCS-based implicit rules.
% : %,v


# Disable VCS-based implicit rules.
% : RCS/%


# Disable VCS-based implicit rules.
% : RCS/%,v


# Disable VCS-based implicit rules.
% : SCCS/s.%


# Disable VCS-based implicit rules.
% : s.%


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/minseok/bwtree

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/minseok/bwtree/build

# Include any dependencies generated for this target.
include googletest/CMakeFiles/bwtree_test.dir/depend.make

# Include the progress variables for this target.
include googletest/CMakeFiles/bwtree_test.dir/progress.make

# Include the compile flags for this target's objects.
include googletest/CMakeFiles/bwtree_test.dir/flags.make

googletest/CMakeFiles/bwtree_test.dir/bwtree_test.cc.o: googletest/CMakeFiles/bwtree_test.dir/flags.make
googletest/CMakeFiles/bwtree_test.dir/bwtree_test.cc.o: ../googletest/bwtree_test.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/minseok/bwtree/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object googletest/CMakeFiles/bwtree_test.dir/bwtree_test.cc.o"
	cd /home/minseok/bwtree/build/googletest && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/bwtree_test.dir/bwtree_test.cc.o -c /home/minseok/bwtree/googletest/bwtree_test.cc

googletest/CMakeFiles/bwtree_test.dir/bwtree_test.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/bwtree_test.dir/bwtree_test.cc.i"
	cd /home/minseok/bwtree/build/googletest && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/minseok/bwtree/googletest/bwtree_test.cc > CMakeFiles/bwtree_test.dir/bwtree_test.cc.i

googletest/CMakeFiles/bwtree_test.dir/bwtree_test.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/bwtree_test.dir/bwtree_test.cc.s"
	cd /home/minseok/bwtree/build/googletest && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/minseok/bwtree/googletest/bwtree_test.cc -o CMakeFiles/bwtree_test.dir/bwtree_test.cc.s

# Object files for target bwtree_test
bwtree_test_OBJECTS = \
"CMakeFiles/bwtree_test.dir/bwtree_test.cc.o"

# External object files for target bwtree_test
bwtree_test_EXTERNAL_OBJECTS =

bin/bwtree_test: googletest/CMakeFiles/bwtree_test.dir/bwtree_test.cc.o
bin/bwtree_test: googletest/CMakeFiles/bwtree_test.dir/build.make
bin/bwtree_test: lib/libbwtree.a
bin/bwtree_test: lib/libgtest_main.a
bin/bwtree_test: lib/libgtest.a
bin/bwtree_test: googletest/CMakeFiles/bwtree_test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/minseok/bwtree/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../bin/bwtree_test"
	cd /home/minseok/bwtree/build/googletest && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/bwtree_test.dir/link.txt --verbose=$(VERBOSE)
	cd /home/minseok/bwtree/build/googletest && /usr/local/bin/cmake -D TEST_TARGET=bwtree_test -D TEST_EXECUTABLE=/home/minseok/bwtree/build/bin/bwtree_test -D TEST_EXECUTOR= -D TEST_WORKING_DIR=/home/minseok/bwtree/build/googletest -D TEST_EXTRA_ARGS= -D TEST_PROPERTIES= -D TEST_PREFIX= -D TEST_SUFFIX= -D NO_PRETTY_TYPES=FALSE -D NO_PRETTY_VALUES=FALSE -D TEST_LIST=bwtree_test_TESTS -D CTEST_FILE=/home/minseok/bwtree/build/googletest/bwtree_test[1]_tests.cmake -D TEST_DISCOVERY_TIMEOUT=5 -D TEST_XML_OUTPUT_DIR= -P /usr/local/share/cmake-3.18/Modules/GoogleTestAddTests.cmake

# Rule to build all files generated by this target.
googletest/CMakeFiles/bwtree_test.dir/build: bin/bwtree_test

.PHONY : googletest/CMakeFiles/bwtree_test.dir/build

googletest/CMakeFiles/bwtree_test.dir/clean:
	cd /home/minseok/bwtree/build/googletest && $(CMAKE_COMMAND) -P CMakeFiles/bwtree_test.dir/cmake_clean.cmake
.PHONY : googletest/CMakeFiles/bwtree_test.dir/clean

googletest/CMakeFiles/bwtree_test.dir/depend:
	cd /home/minseok/bwtree/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/minseok/bwtree /home/minseok/bwtree/googletest /home/minseok/bwtree/build /home/minseok/bwtree/build/googletest /home/minseok/bwtree/build/googletest/CMakeFiles/bwtree_test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : googletest/CMakeFiles/bwtree_test.dir/depend

