#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// constructor
Process::Process(int pid) {
  pid_ = pid;
}

// DONE: Return this process's ID
int Process::Pid() { return pid_; }

// DONE: Return this process's CPU utilization
float Process::CpuUtilization() const{
  long totalTime = LinuxParser::ActiveJiffies(pid_);
  long startTime = LinuxParser::UpTime(pid_);
  long upTime = LinuxParser::UpTime();
  long seconds = upTime - (startTime / sysconf(_SC_CLK_TCK));

  return 1.0 * (totalTime / sysconf(_SC_CLK_TCK)) / seconds;
}

// DONE: Return the command that generated this process
string Process::Command() { return LinuxParser::Command(Pid()); }

// DONE: Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(Pid()); }

// DONE: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(Pid()); }

// DONE: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(Pid()); }

// DONE: Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const { 
  return a.CpuUtilization() > this->CpuUtilization();
}
