#ifndef UTILITY_H
#define UTILITY_H

#include <iostream>

enum Border { INTERVAL_OPEN, INTERVAL_CLOSED };

class TimeInterval
{
  public:
    TimeInterval(std::string l, Border li, std::string r, Border ri) :
    start(l),
    end(r),
    startBorder(li),
    endBorder(ri)
    {}

    TimeInterval() {}

    std::string start;
    std::string end;
    Border startBorder;
    Border endBorder;
};

std::ostream& operator<<(std::ostream& os, const TimeInterval &obj);

#endif // UTILITY_H

