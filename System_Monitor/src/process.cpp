#include "process.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

int Process::Pid() { return pid; }

float Process::CpuUtilization() const {
  return LinuxParser::CpuUtilization(pid);
}

string Process::Command() { return LinuxParser::Command(pid); }

string Process::Ram() const { return LinuxParser::Ram(pid); }

string Process::User() { return LinuxParser::User(pid); }

long int Process::UpTime() { return LinuxParser::UpTime(pid); }

bool Process::operator<(Process const& a) const {
  return this->CpuUtilization() >
         a.CpuUtilization();  // Used ">" instead of "<" as an easy way to sort
                              // largest to smallest in system.cpp
}