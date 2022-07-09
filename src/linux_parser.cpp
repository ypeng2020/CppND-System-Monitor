#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include <iostream>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
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

// DONE: An example of how to read data from the filesystem
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
        pids.emplace_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// DONE: Read and return the system memory utilization
// replace colon
float LinuxParser::MemoryUtilization() { 
  string line;
  string key;
  string kb;
  int value;
  int totalMemory;
  int freeMemory;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value >> kb) {
        if (key == "MemTotal") {
          totalMemory = value;
        } else if (key == "MemFree") {
          freeMemory = value;
        }
      }
    }
  }
  return 1.0 * (totalMemory - freeMemory) / totalMemory;
}

// DONE: Read and return the system uptime
long LinuxParser::UpTime() { 
  string line;
  float upSecond;
  float idleSecond;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> upSecond >> idleSecond;
  }
  long upTime = long(upSecond);
  return upTime; 
}

// DONE: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  return LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies(); 
}

// DONE: Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  string line, key;
  vector<string> values;
  std::ifstream filestream(LinuxParser::kProcDirectory + to_string(pid) + LinuxParser::kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    while (linestream >> key) {
      values.emplace_back(key);
    }
  }
  long jiffies = 0;
  if (values.size() > 21) {
    long user = stol(values[13]);
    long kernel = stol(values[14]);
    long children_user = stol(values[15]);
    long children_kernel = stol(values[16]);
    jiffies = user + kernel + children_user + children_kernel;
  }
  return jiffies;
}

// DONE: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  vector<string> values = LinuxParser::CpuUtilization();
  return stol(values[CPUStates::kUser_]) + stol(values[CPUStates::kNice_]) +
         stol(values[CPUStates::kSystem_]) + stol(values[CPUStates::kIRQ_]) +
         stol(values[CPUStates::kSoftIRQ_]) + stol(values[CPUStates::kSteal_]); 
}

// DONE: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> values = LinuxParser::CpuUtilization();
  return stol(values[CPUStates::kIdle_]) + stol(values[CPUStates::kIOwait_]); 
}

// DONE: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  string line;
  string key, value;
  vector<string> CPUs;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> value) {
        if (key != "cpu") {
          CPUs.emplace_back(value);
        }
      }
    }
  }
  return CPUs;
}

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line;
  string key;
  int value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") {
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line;
  string key;
  int value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: Read and return the command associated with a process
string LinuxParser::Command(int pid) { 
  string value, command;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (filestream.is_open()) {
    std::getline(filestream, value);
    }
  // keep the command after last slash  
  std::size_t found = value.find_last_of("/\\");
  command = value.substr(found + 1);
  return command;
}

// DONE: Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  string line, key;
  int value;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmRSS:") {
          return to_string(value / 1024);
        }
      }
    }
  }
  return "0";
}

// DONE: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string line, key, value;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid:") {
          return value;
        }
      }
    }
  }
  return "0"; 
}

// DONE: Read and return the user associated with a process
string LinuxParser::User(int pid) { 
  string line, name, x, id;
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> name >> x >> id) {
        if (id == LinuxParser::Uid(pid)) {
          return name;
        }
      }
    }
  }
  return "n.a"; 
}

// DONE: Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  string line, value;
  vector<string> values;
  long starttime = 0;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >>  value) {
        values.emplace_back(value);
      }
    }
  }
  starttime = stol(values[21]) / sysconf(_SC_CLK_TCK);
  
  return  LinuxParser::UpTime() - starttime;
}
