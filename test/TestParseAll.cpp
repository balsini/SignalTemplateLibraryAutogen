#include "catch.hpp"

#include <STLdriver.h>

#include <iostream>
#include <fstream>

TEST_CASE("Load and try to parse the test file")
{
  std::cout << STL_TEST_FILE << std::endl;
  std::cout << TEST_FOLDER << std::endl;

  int res = 0;
  STLdriver driver(STL_TEST_FILE);

  driver.parsePorts();

  driver.trace_scanning = true;
  driver.trace_parsing = true;

  res = driver.parse(TEST_FOLDER);

  std::cout << std::endl << "Parser results: " << res << std::endl;
  REQUIRE(1 == 1);
}
