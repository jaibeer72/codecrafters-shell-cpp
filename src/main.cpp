#include <iostream>
#include <string>

int main() {
  // Flush after every std::cout / std::cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  std::cout << "$ ";

  std::string line;
  while (std::getline(std::cin, line)) {
    // treat everything as an error
    std::cerr << line << ": command not found\n";
    std::cout << "$ ";
  }

  return 0;
}
