#ifndef UTILITY_H
#define UTILITY_H

#include <iostream>

enum Border { INTERVAL_OPEN, INTERVAL_CLOSED };
enum DriverStatus { HEADER, BODY, FOOTER };
enum ComparisonOperator { GEQ, LEQ, GREATER, SMALLER, EQUAL, NEQUAL };
enum LogicalOperator { AND, OR, NOT, COMPARISON };
enum MathOperator { SUM, SUB, MUL, DIV, CONST, SIG, REF };

struct MathOperation {
    MathOperator op;
    MathOperation *a;
    MathOperation *b;
    std::string value;
};

struct ComparisonOperation {
    ComparisonOperator op;
    MathOperation *a;
    MathOperation *b;
};

struct LogicalOperation {
    LogicalOperator op;
    LogicalOperation *a;
    LogicalOperation *b;
    ComparisonOperation *value;
};

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

