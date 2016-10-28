#include <iostream>
#include <STLdriver.h>

int main(int argc, char *argv[])
{
    int res = 0;
    STLdriver driver;

    if (argc != 2) {
        std::cout << "Please, provide a valid file path as argument" << std::endl;
        return -1;
    }

    //driver.trace_parsing = true;
    driver.trace_scanning = true;

    res = driver.parse(argv[1]);

    std::cout << "Driver results: " << driver.result << std::endl;
    std::cout << "Parser results: " << res << std::endl;

    return res;
}
