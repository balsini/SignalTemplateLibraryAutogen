#include "utility.h"

std::ostream& operator<<(std::ostream& os, const TimeInterval &obj)
{
  os << obj.start << obj.end;
  return os;
}
