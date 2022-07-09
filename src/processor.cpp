#include <string>
#include "processor.h"

#include "linux_parser.h"
using std::string;

// DONE: Return the aggregate CPU utilization
float Processor::Utilization() { 
  string line;
  string key;
  string cpuID;
  float cpuPct = 0.0;  
  unsigned long int user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
  unsigned long int Idle, NonIdle, Total;
  std::ifstream filestream(LinuxParser::kProcDirectory + LinuxParser::kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> cpuID >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice) {
        if (cpuID == "cpu") {
          Idle = idle + iowait;
          NonIdle = user + nice + system + irq + softirq + steal;
          Total = Idle + NonIdle;
          cpuPct = 1.0 * (Total - Idle) / Total;
        } 
      }
    }
  }
  return cpuPct;
}