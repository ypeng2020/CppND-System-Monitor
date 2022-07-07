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

// add constructor
Process::Process(int pid) {
  pid_ = pid;
  long seconds = LinuxParser::UpTime() - LinuxParser::UpTime(pid);
  long totaltime = LinuxParser::ActiveJiffies(pid);
  float utilization_ =  float(totaltime) / float(seconds);   
}

// TODO: Return this process's ID
int Process::Pid() { return pid_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() const {
    return utilization_;
}

// TODO: Return the command that generated this process
// string Process::Command() { return string(); }
string Process::Command() { return LinuxParser::Command(Pid()); }

// TODO: Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(Pid()); }

// TODO: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(Pid()); }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(Pid()); }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const { return a.CpuUtilization() < this->CpuUtilization(); }
// bool Process::operator<(Process const& a) const { return Ram() < a.Ram(); }