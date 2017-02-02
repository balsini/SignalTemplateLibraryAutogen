#include "catch.hpp"

#include <STLdriver.h>

#include <iostream>
#include <fstream>
#include <string>

#if 0
TEST_CASE("Load and try to parse boolean file")
{
  std::cout << "Test folder: " << TEST_FOLDER << std::endl;

  int res = 0;

  STLdriver driver(TEST_FOLDER);

  driver.parsePorts();

  //driver.trace_scanning = true;
  driver.trace_parsing = true;

  res = driver.parse(TEST_FOLDER + std::string("test_stl_01.stl"));

  REQUIRE(res == 0);
}
#else

TEST_CASE("Load and try to parse AND file")
{
  std::cout << "Test folder: " << TEST_FOLDER << std::endl;

  int res = 0;

  STLdriver driver(TEST_FOLDER);

  driver.parsePorts();

  //driver.trace_scanning = true;
  driver.trace_parsing = true;

  res = driver.parse(TEST_FOLDER + std::string("test_stl_02.stl"));

  REQUIRE(res == 0);
}

#endif
