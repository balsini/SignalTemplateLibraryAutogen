#include "utility.h"

std::ostream& operator<<(std::ostream& os, const TimeInterval &obj)
{
  os << obj.start << obj.end;
  return os;
}

BooleanValue::BooleanValue(bool v) :
  _v(v)
{
  std::cout << "--|--|--|--|--|--|--) Found boolean value [";
  if (v)
    std::cout << "TRUE";
  else
    std::cout << "FALSE";
  std::cout << "]" << std::endl;
}

blockPortMapping BooleanValue::generate()
{
  blockPortMapping bpm;

  std::cout << "TODO - Generating BooleanValue [";
  if (_v)
    std::cout << "TRUE";
  else
    std::cout << "FALSE";
  std::cout << "]" << std::endl;

  return bpm;
}
