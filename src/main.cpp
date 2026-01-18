#include <iostream>
#include <string>

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  std::cout << "$ ";

  std::string line;
  while (std::getline(std::cin, line)) {
    // treaating everything to be an error 
    std::cout << "\n" << line << ": command not found" << std::endl;
    std::cout << "$ "; 

  }
}
