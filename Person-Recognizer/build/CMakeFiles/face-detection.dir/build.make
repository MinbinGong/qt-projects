# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/gongminbin/workspace/repos/Person-Recognizer

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/gongminbin/workspace/repos/Person-Recognizer/build

# Include any dependencies generated for this target.
include CMakeFiles/face-detection.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/face-detection.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/face-detection.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/face-detection.dir/flags.make

CMakeFiles/face-detection.dir/main.cpp.o: CMakeFiles/face-detection.dir/flags.make
CMakeFiles/face-detection.dir/main.cpp.o: ../main.cpp
CMakeFiles/face-detection.dir/main.cpp.o: CMakeFiles/face-detection.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/gongminbin/workspace/repos/Person-Recognizer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/face-detection.dir/main.cpp.o"
	/usr/bin/clang++-17 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/face-detection.dir/main.cpp.o -MF CMakeFiles/face-detection.dir/main.cpp.o.d -o CMakeFiles/face-detection.dir/main.cpp.o -c /home/gongminbin/workspace/repos/Person-Recognizer/main.cpp

CMakeFiles/face-detection.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/face-detection.dir/main.cpp.i"
	/usr/bin/clang++-17 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/gongminbin/workspace/repos/Person-Recognizer/main.cpp > CMakeFiles/face-detection.dir/main.cpp.i

CMakeFiles/face-detection.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/face-detection.dir/main.cpp.s"
	/usr/bin/clang++-17 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/gongminbin/workspace/repos/Person-Recognizer/main.cpp -o CMakeFiles/face-detection.dir/main.cpp.s

# Object files for target face-detection
face__detection_OBJECTS = \
"CMakeFiles/face-detection.dir/main.cpp.o"

# External object files for target face-detection
face__detection_EXTERNAL_OBJECTS =

face-detection: CMakeFiles/face-detection.dir/main.cpp.o
face-detection: CMakeFiles/face-detection.dir/build.make
face-detection: CMakeFiles/face-detection.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/gongminbin/workspace/repos/Person-Recognizer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable face-detection"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/face-detection.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/face-detection.dir/build: face-detection
.PHONY : CMakeFiles/face-detection.dir/build

CMakeFiles/face-detection.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/face-detection.dir/cmake_clean.cmake
.PHONY : CMakeFiles/face-detection.dir/clean

CMakeFiles/face-detection.dir/depend:
	cd /home/gongminbin/workspace/repos/Person-Recognizer/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/gongminbin/workspace/repos/Person-Recognizer /home/gongminbin/workspace/repos/Person-Recognizer /home/gongminbin/workspace/repos/Person-Recognizer/build /home/gongminbin/workspace/repos/Person-Recognizer/build /home/gongminbin/workspace/repos/Person-Recognizer/build/CMakeFiles/face-detection.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/face-detection.dir/depend

