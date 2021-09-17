#include <iostream>
#include <string>
#include <sstream>
using std::string;
#include <sys/resource.h>

int main() { string line = "82097.40 155580.92";


  std::cout << getrlimit(RLIMIT_AS);
}