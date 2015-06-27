cmake_minimum_required(VERSION 2.6)
project(PERSIST C CXX)

set(CMAKE_VERBOSE_MAKEFILE ON)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif(NOT CMAKE_BUILD_TYPE)

if(CMAKE_COMPILER_IS_GNUCXX)
	add_definitions(-fno-strict-aliasing)
	add_definitions(-Wall -Wextra)
	add_definitions(-fshow-column)
        set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -std=c++11)
endif(CMAKE_COMPILER_IS_GNUCXX)

if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
	set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -std=c++11)
endif()

find_package(Threads)

if(WIN32)
	set(Boost_USE_STATIC_LIBS ON)
	add_definitions(-DBOOST_THREAD_USE_LIB)
endif()

find_package(Boost 1.46.0 REQUIRED chrono filesystem system thread)
include_directories(${Boost_INCLUDE_DIRS})

include_directories(${PERSIST_SOURCE_DIR})
include_directories(${PERSIST_SOURCE_DIR}/3rdparty)

set(PERSIST_SOURCES
	Benchmarking.cpp
	Benchmarking.h
	Census.cpp
	Census.h
	config.h	
	DotPrinter.cpp
	DotPrinter.h
	Expression.cpp
	Expression.h
	FenceInsertion.cpp
	FenceInsertion.h
	Foreach.h
	Instruction.h
	Kinds.h
	NaiveParser.cpp
	NaiveParser.h
	Parser.h
	Printer.h
	Program.cpp
	Program.h
	Reduction.cpp
	Reduction.h
	RobustnessChecking.cpp
	RobustnessChecking.h
	SortAndUnique.h
	SpinModelChecker.cpp
	SpinModelChecker.h
	SpinPrinter.cpp
	SpinPrinter.h
	SpinPrinter2.cpp
	SpinPrinter2.h
	State.h
	Thread.cpp
	Thread.h
	Transition.cpp
	Transition.h
)

add_library(persistLib ${PERSIST_SOURCES})
#link_directories(${PERSIST_SOURCE_DIR}/3rdparty/lpsolve)


add_executable(PERSIST Persist.cpp)
target_link_libraries(PERSIST persistLib m boost_regex dl ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES})
#target_link_libraries(PERSIST persistLib liblpsolve55.a m boost_regex dl ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES})
