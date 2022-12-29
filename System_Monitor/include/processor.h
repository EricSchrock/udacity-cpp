#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "linux_parser.h"

class Processor {
 public:
  Processor() : previous(LinuxParser::CpuUtilization()) {}

  float Utilization();

 private:
  std::vector<long> previous;
};

#endif