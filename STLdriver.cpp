#include <STLdriver.h>
#include <STLparser.hh>

STLdriver::STLdriver()
    : trace_scanning (false), trace_parsing (false)
{
}

STLdriver::~STLdriver() {}

int STLdriver::parse(const std::string &f)
{
    file = f;
    scan_begin();
    yy::STLparser parser(*this);
    parser.set_debug_level(trace_parsing);
    int res = parser.parse();
    scan_end();

    return res;
}

void STLdriver::error(const yy::location& l, const std::string& m)
{
    std::cerr << l << ": " << m << std::endl;
}

void STLdriver::error(const std::string& m)
{
    std::cerr << m << std::endl;
}

void STLdriver::append(const std::string &s)
{
    std::cout << s;
}

void STLdriver::appendln(const std::string &s)
{
    append(s);
    std::cout << std::endl;
}
