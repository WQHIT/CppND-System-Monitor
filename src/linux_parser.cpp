#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

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
  string os, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> kernel;
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
  float mem_toal, mem_free;
  string line, key;
  int value;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if(stream.is_open()){
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while(linestream >> key >> value) {
        if (key == "MemTotal:") mem_toal = value;
        if (key == "MemFree:") mem_free = value;
      }
    }
  }
  return ((mem_toal - mem_free) / mem_toal); 
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  long uptime = 0;
  string line;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if(stream.is_open()){
     std::getline(stream, line);
     std::istringstream linestream(line);
     linestream >> uptime;
   }
  return uptime; 
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  string line, cpu, user, nice ,system_ ,idle ,iowait , irq ,softirq, steal, guest, guest_nice;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> cpu >> user>> nice >> system_ >>idle >>iowait >> irq >> softirq>>  steal>> guest>> guest_nice;
  }
  return stol(user)+stol(nice)+stol(system_)+stol(idle)+stol(iowait)+stol(irq)+stol(softirq)+stol(steal);
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  string value;
  long utime, stime, cutime, cstime;
  if(stream.is_open()){
    for(int i = 1; i < 23; i++) {
      std::getline(stream, value,' ');
      switch(i){
        case 14: utime  = std::stol(value); break;
        case 15: stime  = std::stol(value); break;
        case 16: cutime = std::stol(value); break;
        case 17: cstime = std::stol(value); break;
        default: break;
      }
    }
  }
  return utime + stime + cutime + cstime;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  string line, cpu, user, nice, system_, idle, iowait, irq, softirq, steal, guest, guest_nice;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> cpu >> user>> nice >> system_ >>idle >>iowait >> irq >> softirq>>  steal>> guest>> guest_nice;
  }
  return stol(user)+stol(nice)+stol(system_)+stol(irq)+stol(softirq)+stol(steal)+stol(guest)+stol(guest_nice);
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return (Jiffies() - ActiveJiffies());}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  vector<string> return_vec;
  string line, cpu, value;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  stream >> cpu;
  while (stream >> value) {
    return_vec.push_back(value);
  }
  return return_vec; 
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  int runningProcesses;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  string line, key, value;
  if(stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while(linestream >> key >> value) {
        if (key == "processes") {
          runningProcesses = stoi(value);
        }
      }
    }  
  }
  return runningProcesses;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  int runningProcesses;
  string line, key, value;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if(stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while(linestream >> key >> value) {
        if (key == "procs_running") {
          std::replace(value.begin(), value.end(), '_', ' ');
          runningProcesses = stoi(value);
        }
      }
    } 
  }
  return runningProcesses;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  std::ifstream stream(LinuxParser::kProcDirectory + to_string(pid) + LinuxParser::kStatFilename);
  string line;
  if (stream.is_open()){
    std::getline(stream, line);
  }
  return line;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  string line, key, value;
  std::ifstream stream(LinuxParser::kProcDirectory + to_string(pid) + LinuxParser::kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmSize:") {
          value = std::to_string(std::stoi(value) / 1024);
          return value;
        }
      }
    }
  }
  return value;
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  string line, key, value;
  std::ifstream stream(LinuxParser::kProcDirectory + to_string(pid) + LinuxParser::kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid:") {
          return value;
        }
      } 
    } 
  }
  return string();
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  string line, key, pass, value;
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> value >> pass >> key) {
        if (key == LinuxParser::Uid(pid)) {
          return value;
        }
      }
    }
  }
  return string();
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  string line, key, value;
  long uptime{0};

  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    for(int i = 1; i <= 22; i++) {
      linestream >> value;
    }
    uptime = std::stol(value) / sysconf(_SC_CLK_TCK);
  }
  return uptime;
}