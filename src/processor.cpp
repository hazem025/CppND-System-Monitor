#include "processor.h"

#include <linux_parser.h>

#include <iostream>
#include <string>
#include <vector>

using std::string;
using std::vector;
// TODO: Return the aggregate CPU utilization.

//     user    nice   system  idle      iowait irq   softirq  steal  guest
//     guest_nice
float Processor::Utilization() {
  vector<string> cpu_data = LinuxParser::CpuUtilization();

  float idle = std::stof(cpu_data[5]);
  float total = 0;
  for (size_t i = 2; i < cpu_data.size(); ++i) {
    total += std::stof(cpu_data[i]);
  }
  this->cpu_utilization = (total - idle) / total;
  return this->cpu_utilization;
}
