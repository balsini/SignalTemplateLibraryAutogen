#include <iostream>

#include <STLdriver.h>

int main(int argc, char *argv[])
{
  if (argc != 3) {
    std::cout << "Provide a valid .stl input file path as argument AND the path of the matlab code" << std::endl;
    return -1;
  }

  int res = 0;

  try {
    STLdriver driver(argv[2]);

    driver.parsePorts();

    //driver.trace_scanning = true;
    //driver.trace_parsing = true;

    res = driver.parse(argv[1]);

    std::cout << std::endl << "Parser results: " << res << std::endl;
  } catch (const char *e) {
    std::cout << e << std::endl;
  }

  return res;
}
