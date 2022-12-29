#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

float LinuxParser::MemoryUtilization() {
  std::ifstream filestream(LinuxParser::kProcDirectory +
                           LinuxParser::kMeminfoFilename);

  if (filestream.is_open()) {
    string line;
    string total;
    string free;
    string throwaway;

    std::getline(filestream, line);
    std::istringstream line1stream(line);

    line1stream >> throwaway >> total;

    std::getline(filestream, line);
    std::istringstream line2stream(line);

    line2stream >> throwaway >> free;

    return 1.0 - ((float)stoi(free) / (float)stoi(total));
  }

  return 0.0;
}

long LinuxParser::UpTime() {
  std::ifstream filestream(LinuxParser::kProcDirectory +
                           LinuxParser::kUptimeFilename);

  if (filestream.is_open()) {
    string line;
    string uptime;

    std::getline(filestream, line);
    std::istringstream linestream(line);

    linestream >> uptime;

    return stoi(uptime);
  }

  return 0;
}

vector<long> LinuxParser::CpuUtilization() {
  vector<long> times;

  std::ifstream filestream(LinuxParser::kProcDirectory +
                           LinuxParser::kStatFilename);

  if (filestream.is_open()) {
    string line;
    string time;

    std::getline(filestream, line);
    std::istringstream linestream(line);

    while (linestream) {
      linestream >> time;

      try {
        times.emplace_back(stoi(time));
      } catch (std::invalid_argument const&) {
      }
    }
  }

  return times;
}

static int ProcessCountHelper(string desired_key) {
  std::ifstream filestream(LinuxParser::kProcDirectory +
                           LinuxParser::kStatFilename);

  if (filestream.is_open()) {
    string line;

    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);

      string key;
      string value;

      linestream >> key >> value;

      if (key == desired_key) {
        return stoi(value);
      }
    }
  }

  return 0;
}

int LinuxParser::TotalProcesses() { return ProcessCountHelper("processes"); }

int LinuxParser::RunningProcesses() {
  return ProcessCountHelper("procs_running");
}

bool LinuxParser::Running(int pid) {
  std::ifstream filestream(LinuxParser::kProcDirectory + "/" +
                           std::to_string(pid) + LinuxParser::kStatusFilename);

  if (filestream.is_open()) {
    string line;

    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);

      string key;
      string value;

      linestream >> key >> value;

      if (key == "State:") {
        return (value == "R");
      }
    }
  }

  return false;
}

string LinuxParser::Command(int pid) {
  string command;

  std::ifstream filestream(LinuxParser::kProcDirectory + "/" +
                           std::to_string(pid) + LinuxParser::kCmdlineFilename);

  if (filestream.is_open()) {
    std::getline(filestream, command);
  }

  return command;
}

string LinuxParser::Ram(int pid [[maybe_unused]]) {
  std::ifstream filestream(LinuxParser::kProcDirectory + "/" +
                           std::to_string(pid) + LinuxParser::kStatusFilename);

  if (filestream.is_open()) {
    string line;

    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);

      string key;
      string value;

      linestream >> key >> value;

      if (key == "VmSize:") {
        return to_string(stoi(value) / 1024);  // convert from kB to MB
      }
    }
  }

  return "";
}

string LinuxParser::Uid(int pid) {
  std::ifstream filestream(LinuxParser::kProcDirectory + "/" +
                           std::to_string(pid) + LinuxParser::kStatusFilename);

  if (filestream.is_open()) {
    string line;

    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);

      string key, value;

      linestream >> key >> value;

      if (key == "Uid:") {
        return value;
      }
    }
  }

  return "";
}

string LinuxParser::User(int pid) {
  string target_uid = LinuxParser::Uid(pid);

  std::ifstream filestream(LinuxParser::kPasswordPath);

  if (filestream.is_open()) {
    string line;

    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');

      std::istringstream linestream(line);

      string user, x, uid;

      linestream >> user >> x >> uid;

      if (uid == target_uid) {
        return user;
      }
    }
  }

  return "";
}

long LinuxParser::UpTime(int pid) {
  string process_start;

  std::ifstream filestream(LinuxParser::kProcDirectory + "/" +
                           std::to_string(pid) + LinuxParser::kStatFilename);

  if (filestream.is_open()) {
    string line;
    std::getline(filestream, line);

    std::istringstream linestream(line);

    for (int i = 0; i < 22; i++) linestream >> process_start;
  }

  long system_uptime = LinuxParser::UpTime();

  long proc_start_time;

  // Sometimes stoi(process_start) fails because process_start is not a number
  try {
    proc_start_time = stoi(process_start) / sysconf(_SC_CLK_TCK);
  } catch (std::invalid_argument const&) {
    proc_start_time = system_uptime;
  }

  return system_uptime - proc_start_time;
}

float LinuxParser::CpuUtilization(int pid) {
  long uptime = LinuxParser::UpTime(pid);

  string time;
  long process_time = 0;

  std::ifstream filestream(LinuxParser::kProcDirectory + "/" +
                           std::to_string(pid) + LinuxParser::kStatFilename);

  if (filestream.is_open()) {
    string line;
    std::getline(filestream, line);

    std::istringstream linestream(line);

    for (int i = 0; i < 15; i++) {
      linestream >> time;

      if (i >= 13) {
        process_time += stoi(time) / sysconf(_SC_CLK_TCK);
      }
    }
  }

  int cpu_count = 0;

  std::ifstream filestream2(LinuxParser::kProcDirectory +
                            LinuxParser::kCpuinfoFilename);

  if (filestream2.is_open()) {
    string line;

    while (std::getline(filestream2, line)) {
      std::istringstream linestream(line);

      string key;

      linestream >> key;

      if (key == "processor") {
        cpu_count++;
      }
    }
  }

  // Avoid dividing by zero
  if (uptime == 0) uptime++;
  if (cpu_count == 0) cpu_count++;

  return ((float)process_time / (float)uptime) / cpu_count;
}
