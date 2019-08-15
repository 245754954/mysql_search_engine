# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.13

# Default target executed when no arguments are given to make.
default_target: all

.PHONY : default_target

# Allow only one "make -f Makefile2" at a time, but pass parallelism.
.NOTPARALLEL:


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
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/ftpuser/wiser/wiser/wiser/origin-wiser/wiser/trunk

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ftpuser/wiser/wiser/wiser/origin-wiser/wiser/trunk

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake to regenerate build system..."
	/usr/local/bin/cmake -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache

.PHONY : rebuild_cache/fast

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake cache editor..."
	/usr/local/bin/ccmake -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache

.PHONY : edit_cache/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start /home/ftpuser/wiser/wiser/wiser/origin-wiser/wiser/trunk/CMakeFiles /home/ftpuser/wiser/wiser/wiser/origin-wiser/wiser/trunk/CMakeFiles/progress.marks
	$(MAKE) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start /home/ftpuser/wiser/wiser/wiser/origin-wiser/wiser/trunk/CMakeFiles 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) -f CMakeFiles/Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean

.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named indexengine

# Build rule for target.
indexengine: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 indexengine
.PHONY : indexengine

# fast build rule for target.
indexengine/fast:
	$(MAKE) -f CMakeFiles/indexengine.dir/build.make CMakeFiles/indexengine.dir/build
.PHONY : indexengine/fast

#=============================================================================
# Target rules for targets named searchengine

# Build rule for target.
searchengine: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 searchengine
.PHONY : searchengine

# fast build rule for target.
searchengine/fast:
	$(MAKE) -f CMakeFiles/searchengine.dir/build.make CMakeFiles/searchengine.dir/build
.PHONY : searchengine/fast

aesencrypt.o: aesencrypt.c.o

.PHONY : aesencrypt.o

# target to build an object file
aesencrypt.c.o:
	$(MAKE) -f CMakeFiles/indexengine.dir/build.make CMakeFiles/indexengine.dir/aesencrypt.c.o
	$(MAKE) -f CMakeFiles/searchengine.dir/build.make CMakeFiles/searchengine.dir/aesencrypt.c.o
.PHONY : aesencrypt.c.o

aesencrypt.i: aesencrypt.c.i

.PHONY : aesencrypt.i

# target to preprocess a source file
aesencrypt.c.i:
	$(MAKE) -f CMakeFiles/indexengine.dir/build.make CMakeFiles/indexengine.dir/aesencrypt.c.i
	$(MAKE) -f CMakeFiles/searchengine.dir/build.make CMakeFiles/searchengine.dir/aesencrypt.c.i
.PHONY : aesencrypt.c.i

aesencrypt.s: aesencrypt.c.s

.PHONY : aesencrypt.s

# target to generate assembly for a file
aesencrypt.c.s:
	$(MAKE) -f CMakeFiles/indexengine.dir/build.make CMakeFiles/indexengine.dir/aesencrypt.c.s
	$(MAKE) -f CMakeFiles/searchengine.dir/build.make CMakeFiles/searchengine.dir/aesencrypt.c.s
.PHONY : aesencrypt.c.s

encrypt.o: encrypt.c.o

.PHONY : encrypt.o

# target to build an object file
encrypt.c.o:
	$(MAKE) -f CMakeFiles/indexengine.dir/build.make CMakeFiles/indexengine.dir/encrypt.c.o
	$(MAKE) -f CMakeFiles/searchengine.dir/build.make CMakeFiles/searchengine.dir/encrypt.c.o
.PHONY : encrypt.c.o

encrypt.i: encrypt.c.i

.PHONY : encrypt.i

# target to preprocess a source file
encrypt.c.i:
	$(MAKE) -f CMakeFiles/indexengine.dir/build.make CMakeFiles/indexengine.dir/encrypt.c.i
	$(MAKE) -f CMakeFiles/searchengine.dir/build.make CMakeFiles/searchengine.dir/encrypt.c.i
.PHONY : encrypt.c.i

encrypt.s: encrypt.c.s

.PHONY : encrypt.s

# target to generate assembly for a file
encrypt.c.s:
	$(MAKE) -f CMakeFiles/indexengine.dir/build.make CMakeFiles/indexengine.dir/encrypt.c.s
	$(MAKE) -f CMakeFiles/searchengine.dir/build.make CMakeFiles/searchengine.dir/encrypt.c.s
.PHONY : encrypt.c.s

indexengine.o: indexengine.c.o

.PHONY : indexengine.o

# target to build an object file
indexengine.c.o:
	$(MAKE) -f CMakeFiles/indexengine.dir/build.make CMakeFiles/indexengine.dir/indexengine.c.o
.PHONY : indexengine.c.o

indexengine.i: indexengine.c.i

.PHONY : indexengine.i

# target to preprocess a source file
indexengine.c.i:
	$(MAKE) -f CMakeFiles/indexengine.dir/build.make CMakeFiles/indexengine.dir/indexengine.c.i
.PHONY : indexengine.c.i

indexengine.s: indexengine.c.s

.PHONY : indexengine.s

# target to generate assembly for a file
indexengine.c.s:
	$(MAKE) -f CMakeFiles/indexengine.dir/build.make CMakeFiles/indexengine.dir/indexengine.c.s
.PHONY : indexengine.c.s

mysqldatabase.o: mysqldatabase.c.o

.PHONY : mysqldatabase.o

# target to build an object file
mysqldatabase.c.o:
	$(MAKE) -f CMakeFiles/indexengine.dir/build.make CMakeFiles/indexengine.dir/mysqldatabase.c.o
	$(MAKE) -f CMakeFiles/searchengine.dir/build.make CMakeFiles/searchengine.dir/mysqldatabase.c.o
.PHONY : mysqldatabase.c.o

mysqldatabase.i: mysqldatabase.c.i

.PHONY : mysqldatabase.i

# target to preprocess a source file
mysqldatabase.c.i:
	$(MAKE) -f CMakeFiles/indexengine.dir/build.make CMakeFiles/indexengine.dir/mysqldatabase.c.i
	$(MAKE) -f CMakeFiles/searchengine.dir/build.make CMakeFiles/searchengine.dir/mysqldatabase.c.i
.PHONY : mysqldatabase.c.i

mysqldatabase.s: mysqldatabase.c.s

.PHONY : mysqldatabase.s

# target to generate assembly for a file
mysqldatabase.c.s:
	$(MAKE) -f CMakeFiles/indexengine.dir/build.make CMakeFiles/indexengine.dir/mysqldatabase.c.s
	$(MAKE) -f CMakeFiles/searchengine.dir/build.make CMakeFiles/searchengine.dir/mysqldatabase.c.s
.PHONY : mysqldatabase.c.s

postings.o: postings.c.o

.PHONY : postings.o

# target to build an object file
postings.c.o:
	$(MAKE) -f CMakeFiles/indexengine.dir/build.make CMakeFiles/indexengine.dir/postings.c.o
	$(MAKE) -f CMakeFiles/searchengine.dir/build.make CMakeFiles/searchengine.dir/postings.c.o
.PHONY : postings.c.o

postings.i: postings.c.i

.PHONY : postings.i

# target to preprocess a source file
postings.c.i:
	$(MAKE) -f CMakeFiles/indexengine.dir/build.make CMakeFiles/indexengine.dir/postings.c.i
	$(MAKE) -f CMakeFiles/searchengine.dir/build.make CMakeFiles/searchengine.dir/postings.c.i
.PHONY : postings.c.i

postings.s: postings.c.s

.PHONY : postings.s

# target to generate assembly for a file
postings.c.s:
	$(MAKE) -f CMakeFiles/indexengine.dir/build.make CMakeFiles/indexengine.dir/postings.c.s
	$(MAKE) -f CMakeFiles/searchengine.dir/build.make CMakeFiles/searchengine.dir/postings.c.s
.PHONY : postings.c.s

search.o: search.c.o

.PHONY : search.o

# target to build an object file
search.c.o:
	$(MAKE) -f CMakeFiles/indexengine.dir/build.make CMakeFiles/indexengine.dir/search.c.o
	$(MAKE) -f CMakeFiles/searchengine.dir/build.make CMakeFiles/searchengine.dir/search.c.o
.PHONY : search.c.o

search.i: search.c.i

.PHONY : search.i

# target to preprocess a source file
search.c.i:
	$(MAKE) -f CMakeFiles/indexengine.dir/build.make CMakeFiles/indexengine.dir/search.c.i
	$(MAKE) -f CMakeFiles/searchengine.dir/build.make CMakeFiles/searchengine.dir/search.c.i
.PHONY : search.c.i

search.s: search.c.s

.PHONY : search.s

# target to generate assembly for a file
search.c.s:
	$(MAKE) -f CMakeFiles/indexengine.dir/build.make CMakeFiles/indexengine.dir/search.c.s
	$(MAKE) -f CMakeFiles/searchengine.dir/build.make CMakeFiles/searchengine.dir/search.c.s
.PHONY : search.c.s

searchengine.o: searchengine.c.o

.PHONY : searchengine.o

# target to build an object file
searchengine.c.o:
	$(MAKE) -f CMakeFiles/searchengine.dir/build.make CMakeFiles/searchengine.dir/searchengine.c.o
.PHONY : searchengine.c.o

searchengine.i: searchengine.c.i

.PHONY : searchengine.i

# target to preprocess a source file
searchengine.c.i:
	$(MAKE) -f CMakeFiles/searchengine.dir/build.make CMakeFiles/searchengine.dir/searchengine.c.i
.PHONY : searchengine.c.i

searchengine.s: searchengine.c.s

.PHONY : searchengine.s

# target to generate assembly for a file
searchengine.c.s:
	$(MAKE) -f CMakeFiles/searchengine.dir/build.make CMakeFiles/searchengine.dir/searchengine.c.s
.PHONY : searchengine.c.s

token.o: token.c.o

.PHONY : token.o

# target to build an object file
token.c.o:
	$(MAKE) -f CMakeFiles/indexengine.dir/build.make CMakeFiles/indexengine.dir/token.c.o
	$(MAKE) -f CMakeFiles/searchengine.dir/build.make CMakeFiles/searchengine.dir/token.c.o
.PHONY : token.c.o

token.i: token.c.i

.PHONY : token.i

# target to preprocess a source file
token.c.i:
	$(MAKE) -f CMakeFiles/indexengine.dir/build.make CMakeFiles/indexengine.dir/token.c.i
	$(MAKE) -f CMakeFiles/searchengine.dir/build.make CMakeFiles/searchengine.dir/token.c.i
.PHONY : token.c.i

token.s: token.c.s

.PHONY : token.s

# target to generate assembly for a file
token.c.s:
	$(MAKE) -f CMakeFiles/indexengine.dir/build.make CMakeFiles/indexengine.dir/token.c.s
	$(MAKE) -f CMakeFiles/searchengine.dir/build.make CMakeFiles/searchengine.dir/token.c.s
.PHONY : token.c.s

util.o: util.c.o

.PHONY : util.o

# target to build an object file
util.c.o:
	$(MAKE) -f CMakeFiles/indexengine.dir/build.make CMakeFiles/indexengine.dir/util.c.o
	$(MAKE) -f CMakeFiles/searchengine.dir/build.make CMakeFiles/searchengine.dir/util.c.o
.PHONY : util.c.o

util.i: util.c.i

.PHONY : util.i

# target to preprocess a source file
util.c.i:
	$(MAKE) -f CMakeFiles/indexengine.dir/build.make CMakeFiles/indexengine.dir/util.c.i
	$(MAKE) -f CMakeFiles/searchengine.dir/build.make CMakeFiles/searchengine.dir/util.c.i
.PHONY : util.c.i

util.s: util.c.s

.PHONY : util.s

# target to generate assembly for a file
util.c.s:
	$(MAKE) -f CMakeFiles/indexengine.dir/build.make CMakeFiles/indexengine.dir/util.c.s
	$(MAKE) -f CMakeFiles/searchengine.dir/build.make CMakeFiles/searchengine.dir/util.c.s
.PHONY : util.c.s

wikiload.o: wikiload.c.o

.PHONY : wikiload.o

# target to build an object file
wikiload.c.o:
	$(MAKE) -f CMakeFiles/indexengine.dir/build.make CMakeFiles/indexengine.dir/wikiload.c.o
	$(MAKE) -f CMakeFiles/searchengine.dir/build.make CMakeFiles/searchengine.dir/wikiload.c.o
.PHONY : wikiload.c.o

wikiload.i: wikiload.c.i

.PHONY : wikiload.i

# target to preprocess a source file
wikiload.c.i:
	$(MAKE) -f CMakeFiles/indexengine.dir/build.make CMakeFiles/indexengine.dir/wikiload.c.i
	$(MAKE) -f CMakeFiles/searchengine.dir/build.make CMakeFiles/searchengine.dir/wikiload.c.i
.PHONY : wikiload.c.i

wikiload.s: wikiload.c.s

.PHONY : wikiload.s

# target to generate assembly for a file
wikiload.c.s:
	$(MAKE) -f CMakeFiles/indexengine.dir/build.make CMakeFiles/indexengine.dir/wikiload.c.s
	$(MAKE) -f CMakeFiles/searchengine.dir/build.make CMakeFiles/searchengine.dir/wikiload.c.s
.PHONY : wikiload.c.s

wiser.o: wiser.c.o

.PHONY : wiser.o

# target to build an object file
wiser.c.o:
	$(MAKE) -f CMakeFiles/indexengine.dir/build.make CMakeFiles/indexengine.dir/wiser.c.o
	$(MAKE) -f CMakeFiles/searchengine.dir/build.make CMakeFiles/searchengine.dir/wiser.c.o
.PHONY : wiser.c.o

wiser.i: wiser.c.i

.PHONY : wiser.i

# target to preprocess a source file
wiser.c.i:
	$(MAKE) -f CMakeFiles/indexengine.dir/build.make CMakeFiles/indexengine.dir/wiser.c.i
	$(MAKE) -f CMakeFiles/searchengine.dir/build.make CMakeFiles/searchengine.dir/wiser.c.i
.PHONY : wiser.c.i

wiser.s: wiser.c.s

.PHONY : wiser.s

# target to generate assembly for a file
wiser.c.s:
	$(MAKE) -f CMakeFiles/indexengine.dir/build.make CMakeFiles/indexengine.dir/wiser.c.s
	$(MAKE) -f CMakeFiles/searchengine.dir/build.make CMakeFiles/searchengine.dir/wiser.c.s
.PHONY : wiser.c.s

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... rebuild_cache"
	@echo "... edit_cache"
	@echo "... indexengine"
	@echo "... searchengine"
	@echo "... aesencrypt.o"
	@echo "... aesencrypt.i"
	@echo "... aesencrypt.s"
	@echo "... encrypt.o"
	@echo "... encrypt.i"
	@echo "... encrypt.s"
	@echo "... indexengine.o"
	@echo "... indexengine.i"
	@echo "... indexengine.s"
	@echo "... mysqldatabase.o"
	@echo "... mysqldatabase.i"
	@echo "... mysqldatabase.s"
	@echo "... postings.o"
	@echo "... postings.i"
	@echo "... postings.s"
	@echo "... search.o"
	@echo "... search.i"
	@echo "... search.s"
	@echo "... searchengine.o"
	@echo "... searchengine.i"
	@echo "... searchengine.s"
	@echo "... token.o"
	@echo "... token.i"
	@echo "... token.s"
	@echo "... util.o"
	@echo "... util.i"
	@echo "... util.s"
	@echo "... wikiload.o"
	@echo "... wikiload.i"
	@echo "... wikiload.s"
	@echo "... wiser.o"
	@echo "... wiser.i"
	@echo "... wiser.s"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system

