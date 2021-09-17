#include "process.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

using std::string;
using std::to_string;
using std::vector;

Process::Process(int id, std::string user, std::string cmd, float cpu,
                 std::string ram, long int upTime)
    : Pid_(id),
      User_(user),
      Command_(cmd),
      cpu_used(cpu),
      Ram_(ram),
      UpTime_(upTime) {}

// TODO: Return this process's ID
int Process::Pid() { return this->Pid_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() { return this->cpu_used; }

// TODO: Return the command that generated this process
string Process::Command() { return this->Command_; }

// TODO: Return this process's memory utilization
string Process::Ram() { return this->Ram_; }

// TODO: Return the user (name) that generated this process
string Process::User() { return this->User_; }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return this->UpTime_; }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const {
  return this->cpu_used < a.cpu_used;
}