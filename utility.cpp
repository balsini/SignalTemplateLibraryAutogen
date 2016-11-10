#include "utility.h"

std::ostream& operator<<(std::ostream& os, const TimeInterval &obj)
{
  os << obj.start << obj.end;
  return os;
}

void foundConstantBlock(std::string v)
{
  std::cout << "--|--|--|--|--|--) foundConstantBlock [" << v << "]" << std::endl;
}

void foundPortBlock(std::string v)
{
  std::cout << "--|--|--|--|--|--) foundPortBlock [" << v << "]" << std::endl;
}

void foundMainTimeRange(TimeInterval t)
{
  std::cout << "--|--) foundMainTimeRange ["
            << (t.startClosed == "off" ? std::string("(") : std::string("["))
            << t.start << " , "
            << t.end
            << (t.endClosed == "off" ? std::string(")") : std::string("]"))
            << "]" << std::endl;
}

void foundComparisonExpression(LogicalOperator op, std::string v1, std::string v2)
{
  std::cout << "--|--|--) foundComparisonExpression [" << v1 << "][" << v2 << "]" << std::endl;
}
