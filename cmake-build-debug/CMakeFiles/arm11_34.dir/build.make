# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

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
CMAKE_COMMAND = /Applications/CLion.app/Contents/bin/cmake/bin/cmake

# The command to remove a file.
RM = /Applications/CLion.app/Contents/bin/cmake/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/Jeepmcbook/Documents/Imperial_Computing_work/Programming_III/arm11_34

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/Jeepmcbook/Documents/Imperial_Computing_work/Programming_III/arm11_34/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/arm11_34.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/arm11_34.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/arm11_34.dir/flags.make

CMakeFiles/arm11_34.dir/src/assemble.c.o: CMakeFiles/arm11_34.dir/flags.make
CMakeFiles/arm11_34.dir/src/assemble.c.o: ../src/assemble.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/Jeepmcbook/Documents/Imperial_Computing_work/Programming_III/arm11_34/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/arm11_34.dir/src/assemble.c.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/arm11_34.dir/src/assemble.c.o   -c /Users/Jeepmcbook/Documents/Imperial_Computing_work/Programming_III/arm11_34/src/assemble.c

CMakeFiles/arm11_34.dir/src/assemble.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/arm11_34.dir/src/assemble.c.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/Jeepmcbook/Documents/Imperial_Computing_work/Programming_III/arm11_34/src/assemble.c > CMakeFiles/arm11_34.dir/src/assemble.c.i

CMakeFiles/arm11_34.dir/src/assemble.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/arm11_34.dir/src/assemble.c.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/Jeepmcbook/Documents/Imperial_Computing_work/Programming_III/arm11_34/src/assemble.c -o CMakeFiles/arm11_34.dir/src/assemble.c.s

CMakeFiles/arm11_34.dir/src/assemble.c.o.requires:

.PHONY : CMakeFiles/arm11_34.dir/src/assemble.c.o.requires

CMakeFiles/arm11_34.dir/src/assemble.c.o.provides: CMakeFiles/arm11_34.dir/src/assemble.c.o.requires
	$(MAKE) -f CMakeFiles/arm11_34.dir/build.make CMakeFiles/arm11_34.dir/src/assemble.c.o.provides.build
.PHONY : CMakeFiles/arm11_34.dir/src/assemble.c.o.provides

CMakeFiles/arm11_34.dir/src/assemble.c.o.provides.build: CMakeFiles/arm11_34.dir/src/assemble.c.o


CMakeFiles/arm11_34.dir/src/emulate.c.o: CMakeFiles/arm11_34.dir/flags.make
CMakeFiles/arm11_34.dir/src/emulate.c.o: ../src/emulate.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/Jeepmcbook/Documents/Imperial_Computing_work/Programming_III/arm11_34/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/arm11_34.dir/src/emulate.c.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/arm11_34.dir/src/emulate.c.o   -c /Users/Jeepmcbook/Documents/Imperial_Computing_work/Programming_III/arm11_34/src/emulate.c

CMakeFiles/arm11_34.dir/src/emulate.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/arm11_34.dir/src/emulate.c.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/Jeepmcbook/Documents/Imperial_Computing_work/Programming_III/arm11_34/src/emulate.c > CMakeFiles/arm11_34.dir/src/emulate.c.i

CMakeFiles/arm11_34.dir/src/emulate.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/arm11_34.dir/src/emulate.c.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/Jeepmcbook/Documents/Imperial_Computing_work/Programming_III/arm11_34/src/emulate.c -o CMakeFiles/arm11_34.dir/src/emulate.c.s

CMakeFiles/arm11_34.dir/src/emulate.c.o.requires:

.PHONY : CMakeFiles/arm11_34.dir/src/emulate.c.o.requires

CMakeFiles/arm11_34.dir/src/emulate.c.o.provides: CMakeFiles/arm11_34.dir/src/emulate.c.o.requires
	$(MAKE) -f CMakeFiles/arm11_34.dir/build.make CMakeFiles/arm11_34.dir/src/emulate.c.o.provides.build
.PHONY : CMakeFiles/arm11_34.dir/src/emulate.c.o.provides

CMakeFiles/arm11_34.dir/src/emulate.c.o.provides.build: CMakeFiles/arm11_34.dir/src/emulate.c.o


CMakeFiles/arm11_34.dir/src/symbolTable.c.o: CMakeFiles/arm11_34.dir/flags.make
CMakeFiles/arm11_34.dir/src/symbolTable.c.o: ../src/symbolTable.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/Jeepmcbook/Documents/Imperial_Computing_work/Programming_III/arm11_34/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object CMakeFiles/arm11_34.dir/src/symbolTable.c.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/arm11_34.dir/src/symbolTable.c.o   -c /Users/Jeepmcbook/Documents/Imperial_Computing_work/Programming_III/arm11_34/src/symbolTable.c

CMakeFiles/arm11_34.dir/src/symbolTable.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/arm11_34.dir/src/symbolTable.c.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/Jeepmcbook/Documents/Imperial_Computing_work/Programming_III/arm11_34/src/symbolTable.c > CMakeFiles/arm11_34.dir/src/symbolTable.c.i

CMakeFiles/arm11_34.dir/src/symbolTable.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/arm11_34.dir/src/symbolTable.c.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/Jeepmcbook/Documents/Imperial_Computing_work/Programming_III/arm11_34/src/symbolTable.c -o CMakeFiles/arm11_34.dir/src/symbolTable.c.s

CMakeFiles/arm11_34.dir/src/symbolTable.c.o.requires:

.PHONY : CMakeFiles/arm11_34.dir/src/symbolTable.c.o.requires

CMakeFiles/arm11_34.dir/src/symbolTable.c.o.provides: CMakeFiles/arm11_34.dir/src/symbolTable.c.o.requires
	$(MAKE) -f CMakeFiles/arm11_34.dir/build.make CMakeFiles/arm11_34.dir/src/symbolTable.c.o.provides.build
.PHONY : CMakeFiles/arm11_34.dir/src/symbolTable.c.o.provides

CMakeFiles/arm11_34.dir/src/symbolTable.c.o.provides.build: CMakeFiles/arm11_34.dir/src/symbolTable.c.o


CMakeFiles/arm11_34.dir/src/utilities.c.o: CMakeFiles/arm11_34.dir/flags.make
CMakeFiles/arm11_34.dir/src/utilities.c.o: ../src/utilities.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/Jeepmcbook/Documents/Imperial_Computing_work/Programming_III/arm11_34/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building C object CMakeFiles/arm11_34.dir/src/utilities.c.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/arm11_34.dir/src/utilities.c.o   -c /Users/Jeepmcbook/Documents/Imperial_Computing_work/Programming_III/arm11_34/src/utilities.c

CMakeFiles/arm11_34.dir/src/utilities.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/arm11_34.dir/src/utilities.c.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/Jeepmcbook/Documents/Imperial_Computing_work/Programming_III/arm11_34/src/utilities.c > CMakeFiles/arm11_34.dir/src/utilities.c.i

CMakeFiles/arm11_34.dir/src/utilities.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/arm11_34.dir/src/utilities.c.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/Jeepmcbook/Documents/Imperial_Computing_work/Programming_III/arm11_34/src/utilities.c -o CMakeFiles/arm11_34.dir/src/utilities.c.s

CMakeFiles/arm11_34.dir/src/utilities.c.o.requires:

.PHONY : CMakeFiles/arm11_34.dir/src/utilities.c.o.requires

CMakeFiles/arm11_34.dir/src/utilities.c.o.provides: CMakeFiles/arm11_34.dir/src/utilities.c.o.requires
	$(MAKE) -f CMakeFiles/arm11_34.dir/build.make CMakeFiles/arm11_34.dir/src/utilities.c.o.provides.build
.PHONY : CMakeFiles/arm11_34.dir/src/utilities.c.o.provides

CMakeFiles/arm11_34.dir/src/utilities.c.o.provides.build: CMakeFiles/arm11_34.dir/src/utilities.c.o


CMakeFiles/arm11_34.dir/src/blink.c.o: CMakeFiles/arm11_34.dir/flags.make
CMakeFiles/arm11_34.dir/src/blink.c.o: ../src/blink.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/Jeepmcbook/Documents/Imperial_Computing_work/Programming_III/arm11_34/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building C object CMakeFiles/arm11_34.dir/src/blink.c.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/arm11_34.dir/src/blink.c.o   -c /Users/Jeepmcbook/Documents/Imperial_Computing_work/Programming_III/arm11_34/src/blink.c

CMakeFiles/arm11_34.dir/src/blink.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/arm11_34.dir/src/blink.c.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/Jeepmcbook/Documents/Imperial_Computing_work/Programming_III/arm11_34/src/blink.c > CMakeFiles/arm11_34.dir/src/blink.c.i

CMakeFiles/arm11_34.dir/src/blink.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/arm11_34.dir/src/blink.c.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/Jeepmcbook/Documents/Imperial_Computing_work/Programming_III/arm11_34/src/blink.c -o CMakeFiles/arm11_34.dir/src/blink.c.s

CMakeFiles/arm11_34.dir/src/blink.c.o.requires:

.PHONY : CMakeFiles/arm11_34.dir/src/blink.c.o.requires

CMakeFiles/arm11_34.dir/src/blink.c.o.provides: CMakeFiles/arm11_34.dir/src/blink.c.o.requires
	$(MAKE) -f CMakeFiles/arm11_34.dir/build.make CMakeFiles/arm11_34.dir/src/blink.c.o.provides.build
.PHONY : CMakeFiles/arm11_34.dir/src/blink.c.o.provides

CMakeFiles/arm11_34.dir/src/blink.c.o.provides.build: CMakeFiles/arm11_34.dir/src/blink.c.o


# Object files for target arm11_34
arm11_34_OBJECTS = \
"CMakeFiles/arm11_34.dir/src/assemble.c.o" \
"CMakeFiles/arm11_34.dir/src/emulate.c.o" \
"CMakeFiles/arm11_34.dir/src/symbolTable.c.o" \
"CMakeFiles/arm11_34.dir/src/utilities.c.o" \
"CMakeFiles/arm11_34.dir/src/blink.c.o"

# External object files for target arm11_34
arm11_34_EXTERNAL_OBJECTS =

arm11_34: CMakeFiles/arm11_34.dir/src/assemble.c.o
arm11_34: CMakeFiles/arm11_34.dir/src/emulate.c.o
arm11_34: CMakeFiles/arm11_34.dir/src/symbolTable.c.o
arm11_34: CMakeFiles/arm11_34.dir/src/utilities.c.o
arm11_34: CMakeFiles/arm11_34.dir/src/blink.c.o
arm11_34: CMakeFiles/arm11_34.dir/build.make
arm11_34: CMakeFiles/arm11_34.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/Jeepmcbook/Documents/Imperial_Computing_work/Programming_III/arm11_34/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Linking C executable arm11_34"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/arm11_34.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/arm11_34.dir/build: arm11_34

.PHONY : CMakeFiles/arm11_34.dir/build

CMakeFiles/arm11_34.dir/requires: CMakeFiles/arm11_34.dir/src/assemble.c.o.requires
CMakeFiles/arm11_34.dir/requires: CMakeFiles/arm11_34.dir/src/emulate.c.o.requires
CMakeFiles/arm11_34.dir/requires: CMakeFiles/arm11_34.dir/src/symbolTable.c.o.requires
CMakeFiles/arm11_34.dir/requires: CMakeFiles/arm11_34.dir/src/utilities.c.o.requires
CMakeFiles/arm11_34.dir/requires: CMakeFiles/arm11_34.dir/src/blink.c.o.requires

.PHONY : CMakeFiles/arm11_34.dir/requires

CMakeFiles/arm11_34.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/arm11_34.dir/cmake_clean.cmake
.PHONY : CMakeFiles/arm11_34.dir/clean

CMakeFiles/arm11_34.dir/depend:
	cd /Users/Jeepmcbook/Documents/Imperial_Computing_work/Programming_III/arm11_34/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/Jeepmcbook/Documents/Imperial_Computing_work/Programming_III/arm11_34 /Users/Jeepmcbook/Documents/Imperial_Computing_work/Programming_III/arm11_34 /Users/Jeepmcbook/Documents/Imperial_Computing_work/Programming_III/arm11_34/cmake-build-debug /Users/Jeepmcbook/Documents/Imperial_Computing_work/Programming_III/arm11_34/cmake-build-debug /Users/Jeepmcbook/Documents/Imperial_Computing_work/Programming_III/arm11_34/cmake-build-debug/CMakeFiles/arm11_34.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/arm11_34.dir/depend

