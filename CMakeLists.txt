cmake_minimum_required(VERSION 3.1)

project(SignalTemplateLibraryAutogen)

set(CMAKE_BUILD_TYPE Debug)


include_directories(.)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

find_package(BISON)
find_package(FLEX)

BISON_TARGET(STLParser STLparser.yy ${CMAKE_CURRENT_BINARY_DIR}/STLparser.cc COMPILE_FLAGS --report=state)
FLEX_TARGET(STLScanner STLscanner.ll ${CMAKE_CURRENT_BINARY_DIR}/STLscanner.cc)

message("Bison output src files: " ${BISON_STLParser_OUTPUTS})
message("Flex output src files: " ${FLEX_STLScanner_OUTPUTS})

ADD_FLEX_BISON_DEPENDENCY(STLScanner STLParser)

add_executable(${PROJECT_NAME}
  main.cpp
  STLdriver.cpp
  utility.cpp
  ${BISON_STLParser_OUTPUTS}
  ${FLEX_STLScanner_OUTPUTS}
)

target_compile_features(SignalTemplateLibraryAutogen PRIVATE cxx_range_for)
