cmake_minimum_required(VERSION 3.1)

project(SignalTemplateLibraryAutogen)

set(CMAKE_BUILD_TYPE Debug)

include_directories(.)
include_directories(test)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

find_package(BISON)
find_package(FLEX)

BISON_TARGET(STLParser STLparser.yy ${CMAKE_CURRENT_BINARY_DIR}/STLparser.cc COMPILE_FLAGS "--report=all --graph")
FLEX_TARGET(STLScanner STLscanner.ll ${CMAKE_CURRENT_BINARY_DIR}/STLscanner.cc)

ADD_FLEX_BISON_DEPENDENCY(STLScanner STLParser)

add_executable(${PROJECT_NAME}
  main.cpp
  STLdriver.cpp
  utility.cpp
  ${BISON_STLParser_OUTPUTS}
  ${FLEX_STLScanner_OUTPUTS}
)

target_compile_features(SignalTemplateLibraryAutogen PRIVATE cxx_range_for)



enable_testing(true)

set(TEST_FOLDER \"${CMAKE_CURRENT_SOURCE_DIR}/test/\")
add_definitions(-DTEST_FOLDER=${TEST_FOLDER})

set (TEST_LIBRARY ${PROJECT_NAME}_test)

# Create Catch library to decrease compile times
add_library (${TEST_LIBRARY} "STATIC" test/TestMain.cpp test/catch.hpp)

# Define a macro to simplify tests creation
macro (create_test required_arg1 required_arg2)
    message(${required_arg1})
    message(${required_arg2})
    add_executable (${required_arg1}
        ${required_arg2}
        STLdriver.cpp
        utility.cpp
        ${BISON_STLParser_OUTPUTS}
        ${FLEX_STLScanner_OUTPUTS})
    target_compile_features (${required_arg1} PRIVATE cxx_range_for)
    target_link_libraries (${required_arg1} ${TEST_LIBRARY})
    add_test (NAME ${required_arg1} COMMAND ${required_arg1})
endmacro (create_test)

create_test (TestParseAll test/TestParseAll.cpp)
