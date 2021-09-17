#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std::filesystem;
using std::ifstream;
using std::istringstream;
using std::stof;
using std::string;
using std::to_string;
using std::vector;

/*  Helper Function decleration. */
static float MemoryFileReading(ifstream& memoryFile);
static long upTimeReadingFile(ifstream& memoryFile);
static long TotalTimeReadingFile(ifstream& memoryFile);
static vector<long> StatFileReading(ifstream& statFile);
static long getRunningProc(ifstream& statFile);
static long getTotalProc(ifstream& statFile);
static vector<string> ReadPIDStatusFile(ifstream& statusFile);
static string getRam(ifstream& status);
static string getUID(ifstream& status);
/**
 * Create A generic Template that let me read a file and return the wanted data
 * from it.
 */
template <class file_stream, class Path_to_File, class call_back,
          class return_value>
void ReadFile(file_stream& file, Path_to_File file_path, return_value* data,
              call_back reading_function) {
  std::filesystem::path path_object(file_path);
  /* Check if file is exists. */
  if (std::filesystem::exists(path_object)) {
    if (file.is_open()) {
      *(data) = (*reading_function)(
          file);  // callback function defined at the end of this file.
    }
  }
}

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
  string os, kernel, version;
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

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  float f32_memory_util = 0.0;
  string Path = kProcDirectory + kMeminfoFilename;
  ifstream file(Path);
  ReadFile(file, Path, &f32_memory_util, MemoryFileReading);
  return f32_memory_util;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  long upTime = 0;
  string Path = kProcDirectory + kUptimeFilename;
  ifstream file(Path);
  ReadFile(file, Path, &upTime, upTimeReadingFile);
  return upTime;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  long total_time = 0;
  string Path = kProcDirectory + kUptimeFilename;
  ifstream file(Path);
  ReadFile(file, Path, &total_time, TotalTimeReadingFile);
  return total_time/sysconf(_SC_CLK_TCK);
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) 
{ 
  string Path = kProcDirectory + std::to_string(pid) + kStatFilename;
  ifstream file(Path);
  string line = "0";
  long active_jiffies = 0;
  vector<string> value;

  while (std::getline(file, line, ' ')) {
    value.push_back(line);
  }
  if (value.size() > 17)
    active_jiffies += std::stol(value[13]) + std::stol(value[14]) + std::stol(value[15]) + std::stol(value[16]);

  return active_jiffies;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return LinuxParser::UpTime()/sysconf(_SC_CLK_TCK); }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  return LinuxParser::Jiffies() - LinuxParser::ActiveJiffies();
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  vector<string> cpu_time;
  string Path = kProcDirectory + kStatFilename;
  ifstream file(Path);
  string line;
  while (std::getline(file, line, ' ')) {
    if (line.find("cpu1") != string::npos) break;
    if (line.find("cpu0") != string::npos) continue;
    cpu_time.push_back(line);
  }
  return cpu_time;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  long total;
  string Path = kProcDirectory + kStatFilename;
  ifstream file(Path);
  ReadFile(file, Path, &total, getTotalProc);
  return total;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  long number_of_proccess;
  string Path = kProcDirectory + kStatFilename;
  ifstream file(Path);
  ReadFile(file, Path, &number_of_proccess, getRunningProc);
  return number_of_proccess;
}

/// proc/[pid]/status in user and memory.

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  string path = kProcDirectory + std::to_string(pid) + kCmdlineFilename;
  ifstream file(path);
  string cmd;
  std::getline(file, cmd);
  if (cmd.length() <= 0) cmd = "-";
  return " " + cmd;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  string ram;
  string Path = kProcDirectory + std::to_string(pid) + kStatusFilename;
  ifstream file(Path);
  ReadFile(file, Path, &ram, getRam);
  return ram;
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  string uid;
  string Path = kProcDirectory + std::to_string(pid) + kStatusFilename;
  ifstream file(Path);
  ReadFile(file, Path, &uid, getUID);
  return uid;
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  string user_id = LinuxParser::Uid(pid);
  string Path = kPasswordPath;
  ifstream file(Path);
  string line;
  string username;

  while (std::getline(file, line)) {
    size_t pos = line.find(":" + user_id + ":" + user_id);
    if (pos != string::npos) {
      username = line.substr(0, pos - 2);
      break;
    }
  }
  return username;
}
//97918 905519104
// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  // 9153
  string Path = kProcDirectory + std::to_string(pid) + kStatFilename;
  ;
  ifstream file(Path);
  string line = "0";
  vector<string> value;
  size_t pos;

  while (std::getline(file, line, ' ')) {
    value.push_back(line);
  }

  if (value.size() == 52) pos = 21;
  else
    pos = 22;

  if (pos >= value.size()) return 0;
  return std::stol(value[pos]);
}

/**********************************    Helper Functions for file reading
 * ***************************************************************/
static float MemoryFileReading(ifstream& memoryFile) {
  string line;

  float MemTotal, MemFree;
  while (std::getline(memoryFile, line)) {
    // Process should be here.
    size_t kb_pos = line.find("kB");
    if (kb_pos != string::npos) line.replace(kb_pos, 2, "");

    size_t first_character_memTotal = line.find("MemTotal:");
    size_t first_character_memFree = line.find("MemAvailable:");

    if (first_character_memTotal != string::npos) {
      line.replace(first_character_memTotal, 9, "");
      MemTotal = std::stof(line) / 1024;  // To convert it to mega.
    } else if (first_character_memFree != string::npos) {
      line.replace(first_character_memFree, 13, "");
      MemFree = std::stof(line) / 1024;
    }
  }
  // string to float conversion.
  return (MemTotal - MemFree) / MemTotal;
}

static long upTimeReadingFile(ifstream& memoryFile) {
  string upTime, line;
  std::getline(memoryFile, line);
  std::istringstream linestream(line);
  linestream >> upTime;
  return (long)std::stof(upTime);
}

static long TotalTimeReadingFile(ifstream& memoryFile) {
  string upTime, line, idle;
  std::getline(memoryFile, line);
  std::istringstream linestream(line);
  linestream >> upTime >> idle;
  return (long)std::stof(upTime) + (long)std::stof(idle);
}

static vector<long> StatFileReading(ifstream& statFile) {
  string line;

  vector<long> number_of_process{0, 0};

  while (std::getline(statFile, line)) {
    size_t proc_line = line.find("processes");
    size_t running_line = line.find("procs_running");
    if (proc_line != string::npos) {
      line.replace(proc_line, 10, "");
      number_of_process[1] = std::stoi(line);
    } else if (running_line != string::npos) {
      line.replace(running_line, 14, "");
      number_of_process[0] = std::stoi(line);
      break;
    }
  }
  return number_of_process;
}

static vector<string> ReadPIDStatusFile(ifstream& statusFile) {
  string line, user_id, ram_consumption;

  vector<string> data{"0", "0"};

  while (std::getline(statusFile, line)) {
    size_t finder = line.find("Uid");
    size_t find_ram = line.find("VmSize");
    // Create line stream.
    istringstream lineStream(line);
    if (finder != string::npos) {
      lineStream >> user_id >> user_id;
      data[0] = user_id;
    } else if (find_ram != string::npos) {
      lineStream >> ram_consumption >> ram_consumption;
      data[1] = ram_consumption;
      break;
    }
  }
  return data;
}

static long getRunningProc(ifstream& statFile) {
  return StatFileReading(statFile)[0];
}
static long getTotalProc(ifstream& statFile) {
  return StatFileReading(statFile)[1];
}
static string getRam(ifstream& status) { return ReadPIDStatusFile(status)[1]; }
static string getUID(ifstream& status) { return ReadPIDStatusFile(status)[0]; }