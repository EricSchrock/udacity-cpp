#include "format.h"

#include <string>

using std::string;

string Format::ElapsedTime(long seconds) {
  int secs, mins, hours;

  secs = seconds % 60;
  seconds /= 60;
  mins = seconds % 60;
  seconds /= 60;
  hours = seconds;

  return ((hours < 10) ? "0" : "") + std::to_string(hours) + ":" +
         ((mins < 10) ? "0" : "") + std::to_string(mins) + ":" +
         ((secs < 10) ? "0" : "") + std::to_string(secs);
}