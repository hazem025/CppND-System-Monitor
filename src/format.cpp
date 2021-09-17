#include "format.h"

#include <iostream>
#include <string>

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
  long hours = seconds / (3600);
  long min = (seconds / 60) - (hours * 60);
  long sec = (seconds) % 60;

  string hrs = std::to_string(hours);
  string mins = std::to_string(min);
  string sec_string = std::to_string(sec);
  if (sec < 10) sec_string = "0" + sec_string;
  if (hours >= 24) {
    hrs = std::to_string(hours / 24) + " days, ";
    hours = hours % 24;
  }
  if (hours < 10)
    hrs =  "0" + std::to_string(hours);
  else
    hrs = std::to_string(hours);

  if (min < 10) mins = "0" + mins;
  
  return hrs + ":" + mins + ":" + sec_string;
}