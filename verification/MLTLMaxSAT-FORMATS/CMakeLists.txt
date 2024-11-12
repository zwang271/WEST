# minimum required CMAKE version
CMAKE_MINIMUM_REQUIRED(VERSION 3.7 FATAL_ERROR)
project(Translators)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
# include(ExternalProject)
# compiler must be 17
set(CMAKE_CXX_STANDARD 17)

# required if linking to static library
add_definitions(-DANTLR4CPP_STATIC)

# using /MD flag for antlr4_runtime (for Visual C++ compilers only)
set(ANTLR4_WITH_STATIC_CRT OFF)

# Specify the version of the antlr4 library needed for this project.
# By default the latest version of antlr4 will be used.  You can specify a
# specific, stable version by setting a repository tag value or a link
# to a zip file containing the libary source.
set(ANTLR4_TAG 4.12.0)
set(ANTLR4_ZIP_REPOSITORY https://github.com/antlr/antlr4/archive/refs/tags/4.12.0.zip)

# add external build for antlrcpp
include(ExternalAntlr4Cpp)
# add antrl4cpp artifacts to project environment


message("This is ${ANTLR4_INCLUDE_DIRS}")
message("This is ${CMAKE_BINARY_DIR}")
# set variable pointing to the antlr tool that supports C++
# this is not required if the jar file can be found under PATH environment
include(FetchContent)
FetchContent_Declare(jar
PREFIX "${CMAKE_BINARY_DIR}"
URL "https://github.com/antlr/website-antlr4/raw/gh-pages/download/antlr-4.12.0-complete.jar"
DOWNLOAD_NO_EXTRACT true)





FetchContent_MakeAvailable(jar)

message("This is ${jar_SOURCE_DIR}")

set(ANTLR_EXECUTABLE "${jar_SOURCE_DIR}/antlr-4.12.0-complete.jar")
message("This is ${ANTLR_EXECUTABLE}")
# add macros to generate ANTLR Cpp code from grammar
find_package(ANTLR REQUIRED)

# Call macro to add lexer and grammar to your build dependencies.
# antlr_target(SampleGrammarLexer TLexer.g4 LEXER
            #  PACKAGE antlrcpptest)
# set(ANTLR )

antlr_target(LTLParser LTL.g4 LISTENER VISITOR)
antlr_target(MLTLParser MLTL.g4 LISTENER VISITOR)
# include generated files in project environment
include_directories(
  ${ANTLR4_INCLUDE_DIRS}
  include/
  )
include_directories(${ANTLR_MLTLParser_OUTPUT_DIR})
include_directories(${ANTLR_LTLParser_OUTPUT_DIR})

# add generated grammar to demo binary target

add_library(translator 
src/String2MLTL.cpp 
src/MLTLFormula.cpp
src/LTL2MLTL.cpp
src/Formula.cpp
src/Translators.cpp
${ANTLR_MLTLParser_CXX_OUTPUTS}
${ANTLR_LTLParser_CXX_OUTPUTS})
add_dependencies(translator antlr4_runtime)
add_executable(MLTL2propFast src/MLTL2propFast.cpp
                ${ANTLR_MLTLParser_CXX_OUTPUTS})
target_link_libraries(MLTL2propFast antlr4_static translator)

add_executable(main src/main.cpp
                ${ANTLR_MLTLParser_CXX_OUTPUTS})
target_link_libraries(main antlr4_static translator)

add_executable(MLTL2LiSMT src/MLTL2LiSMT.cpp
                ${ANTLR_MLTLParser_CXX_OUTPUTS})
target_link_libraries(MLTL2LiSMT antlr4_static translator)

add_executable(MLTL2propSlow src/MLTL2propSlow.cpp
                ${ANTLR_MLTLParser_CXX_OUTPUTS})
target_link_libraries(MLTL2propSlow antlr4_static translator)

add_executable(MaxSAT src/MaxSAT.cpp
                ${ANTLR_MLTLParser_CXX_OUTPUTS})
target_link_libraries(MaxSAT antlr4_static translator)

add_executable(benchmarker src/benchmarker.cpp
                ${ANTLR_MLTLParser_CXX_OUTPUTS})
target_link_libraries(benchmarker antlr4_static translator)

add_executable(genMLTLBenchmarks src/genMLTLBenchmarks.cpp
                ${ANTLR_MLTLParser_CXX_OUTPUTS} ${ANTLR_LTLParser_CXX_OUTPUTS})
target_link_libraries(genMLTLBenchmarks antlr4_static translator)