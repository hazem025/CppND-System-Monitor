#include "system.h"

#include <linux_parser.h>
#include <unistd.h>

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <set>
#include <string>
#include <vector>

#include "process.h"
#include "processor.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

static bool CompProcess(Process a, Process b);

// TODO: Return the system's CPU
Processor& System::Cpu() { return this->cpu_; }

// TODO: Return a container composed of the system's processes
// int id, std::string user, std::string cmd, float cpu, std::string ram, long
// int upTime
vector<Process>& System::Processes() {
  vector<int> pids = LinuxParser::Pids();

  Process temp(0, "0", "0", 0.0, "0", 0);

  this->processes_.push_back(temp);

  this->processes_.resize(pids.size(), temp);

  int i = 0;
  for (int id : pids) {
    // long ram = std::stoi(LinuxParser::Ram(id))/1024;
    float sec = ((float)LinuxParser::UpTime() -
                 LinuxParser::UpTime(id) / sysconf(_SC_CLK_TCK));
    float cpu_load = ((float)LinuxParser::ActiveJiffies(id)/sysconf(_SC_CLK_TCK))/sec;

    Process new_p (id, LinuxParser::User(id), LinuxParser::Command(id),
                    cpu_load,
                    LinuxParser::Ram(id).substr(0, 3),
                    LinuxParser::ActiveJiffies(id)/sysconf(_SC_CLK_TCK));
    this->processes_[i] = new_p;
    ++i;
  }
  // sort processes.
  std::sort(this->processes_.begin(), this->processes_.end(), CompProcess);
  std::reverse(this->processes_.begin(), this->processes_.end());
  return this->processes_;
}

// TODO: Return the system's kernel identifier (string)
std::string System::Kernel() { return LinuxParser::Kernel(); }

// TODO: Return the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// TODO: Return the operating system name
std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

// TODO: Return the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// TODO: Return the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// TODO: Return the number of seconds since the system started running
long int System::UpTime() { return LinuxParser::UpTime(); }

static bool CompProcess(Process a, Process b) { return a < b; }