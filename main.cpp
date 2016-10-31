#include <iostream>

#include <STLdriver.h>

int main(int argc, char *argv[])
{
    int res = 0;
    STLdriver driver(argv[2]);

    if (argc != 3) {
        std::cout << "Please, provide a valid input file path as argument AND a falid output file path" << std::endl;
        return -1;
    }

    //driver.trace_parsing = true;
    driver.trace_scanning = true;

    res = driver.parse(argv[1]);

    std::cout << "Parser results: " << res << std::endl;

    return res;
}
