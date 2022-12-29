#include "processor.h"

#include "linux_parser.h"

float Processor::Utilization() {
  std::vector<long> times = LinuxParser::CpuUtilization();

  // Idle = idle + iowait
  long idle = times[3] + times[4];
  long previous_idle = previous[3] + previous[4];
  long delta_idle = idle - previous_idle;

  // Busy = user + nice + system + irq + softirq + steal
  long busy = times[0] + times[1] + times[2] + times[5] + times[6] + times[7];
  long previous_busy = previous[0] + previous[1] + previous[2] + previous[5] +
                       previous[6] + previous[7];
  long delta_busy = busy - previous_busy;

  this->previous = times;

  // Avoid divide by zero
  if ((delta_busy + delta_idle) == 0) delta_idle++;

  return (float)delta_busy / ((float)delta_busy + (float)delta_idle);
}