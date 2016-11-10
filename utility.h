#ifndef UTILITY_H
#define UTILITY_H

#include <iostream>

enum Border { INTERVAL_OPEN, INTERVAL_CLOSED };
enum DriverStatus { HEADER, BODY, FOOTER };
enum ComparisonOperator { GEQ, LEQ, GREATER, SMALLER, EQUAL, NEQUAL };
enum LogicalOperator { AND, OR, NOT, COMPARISON };
enum MathOperator { SUM, SUB, MUL, DIV, CONST, PORT };
enum TemporalOperator { ALWAYS, EVENTUALLY };

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
      end(r)
    {
      if (li == INTERVAL_OPEN) {
        startClosed = "off";
      } else {
        startClosed = "on";
      }

      if (ri == INTERVAL_OPEN) {
        endClosed = "off";
      } else {
        endClosed = "on";
      }
    }

    TimeInterval() {}

    std::string start;
    std::string end;
    std::string startClosed;
    std::string endClosed;
};

std::ostream& operator<<(std::ostream& os, const TimeInterval &obj);

void foundConstantBlock(std::string v);
void foundPortBlock(std::string v);
void foundMainTimeRange(TimeInterval t);
void foundComparisonExpression(LogicalOperator op, std::string v1, std::string v2 = "");

#endif // UTILITY_H

