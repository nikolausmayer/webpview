##
#
# Author: Nikolaus Mayer, 2018 (mayern@cs.uni-freiburg.de)
#
##&
SHELL:=/bin/bash

## Compiler
CXX ?= gcc

## Compiler flags; extended in 'debug'/'release' rules
CXXFLAGS += -fPIC -W -Wall -Wextra -Wpedantic -std=c++11 -pthread -Isrc -Iwebp/src -Wno-misleading-indentation -Wno-shift-negative-value

## Linker flags
LDFLAGS = -pthread -lX11 ./webp/src/.libs/libwebp.a

## Default name for the built executable
TARGET = webpview

## Every *.c/*.cc/*.cpp file is a source file
SOURCES = $(wildcard src/*.c src/*.cc src/*.cpp)
## Every *.h/*.hh/*.hpp file is a header file
HEADERS = $(wildcard *.h *.hh *.hpp)

## Build a *.o object file for every source file
OBJECTS = $(addsuffix .o, $(basename $(SOURCES)))


## Tell the 'make' program that e.g. 'make clean' is not supposed to 
## create a file 'clean'
##
## "Why is it called 'phony'?" -- because it's not a real target. That 
## is, the target name isn't a file that is produced by the commands 
## of that target.
.PHONY: all clean debug release webp


## Default is release build mode
all: release
	
## When in debug mode, don't optimize, and create debug symbols
debug: CXXFLAGS += -O0 -g -D_DEBUG
debug: BUILDMODE ?= DEBUG
debug: webp $(TARGET)
	
## When in release mode, optimize
release: CXXFLAGS += -O3
release: BUILDMODE ?= RELEASE
release: cimg webp $(TARGET)

cimg:
	test -f v.2.2.3.tar.gz || wget https://github.com/dtschump/CImg/archive/v.2.2.3.tar.gz
	test -d CImg-v.2.2.3 || tar xfz v.2.2.3.tar.gz
	test -f src/CImg.h || cp CImg-v.2.2.3/CImg.h src/CImg.h

webp:
	test -f v0.5.1.tar.gz || wget https://github.com/webmproject/libwebp/archive/v0.5.1.tar.gz 
	test -d libwebp-0.5.1 || test -d webp || tar xfz v0.5.1.tar.gz
	test -d webp || mv libwebp-0.5.1 webp
	test -f webp/configure || (cd webp && bash autogen.sh && cd ..)
	test -f webp/Makefile || (cd webp && CFLAGS=-fPIC ./configure --disable-gl --disable-png --disable-jpeg --disable-tiff --disable-gif --disable-wic --disable-shared)
	test -f webp/src/.libs/libwebp.a || (cd webp && make -j && cd ..)


## Remove built object files and the main executable
clean:
	$(info ... deleting built object files and executable  ...)
	-rm src/*.o $(TARGET)

## The main executable depends on all object files of all source files
$(TARGET): $(OBJECTS) webp Makefile
	$(info ... linking $@ ...)
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $@

## Every object file depends on its source. It may also depend on
## potentially all header files, and of course the makefile itself.
%.o: %.c webp Makefile $(HEADERS)
	$(info ... compiling $@ ...)
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) -c $< -o $@

%.o: %.cc webp Makefile $(HEADERS)
	$(info ... compiling $@ ...)
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) -c $< -o $@

%.o: %.cpp webp Makefile $(HEADERS)
	$(info ... compiling $@ ...)
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) -c $< -o $@


