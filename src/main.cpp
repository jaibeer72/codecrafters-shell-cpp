#include "commands.h"
#include <cstdlib>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

// Current Commands
std::vector<std::string> split_tokens(const std::string &s) {
  std::vector<std::string> tokens;
  std::istringstream iss(s);
  std::string tok;
  while (iss >> tok)
    tokens.push_back(tok);
  return tokens;
}

int main() {
  // Flush after every std::cout / std::cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  cmd::register_default_builtins();

  std::cout << "$ ";

  std::string line;
  while (std::getline(std::cin, line)) {

    auto tokens = split_tokens(line);
    // strip any trailing CR (e.g. if input has \r) and tolerate pasted prompt
    for (auto &t : tokens) {
      if (!t.empty() && t.back() == '\r')
        t.pop_back();
    }
    // if the user pasted the prompt like "$ echo", remove leading '$' on the
    // first token
    if (!tokens.empty() && !tokens[0].empty() && tokens[0][0] == '$') {
      tokens[0] = tokens[0].substr(1);
      if (tokens[0].empty() && tokens.size() > 1)
        tokens.erase(tokens.begin());
    }
    if (tokens.empty()) {
      std::cout << "$ ";
      continue;
    }

    if (!cmd::dispatch(tokens)) {
      std::string path; 
      if(!cmd::is_external_command(tokens[0], path)){
        std::cerr << tokens[0] << ": not found\n";
        continue;
      }

      cmd::dispatch_external(tokens, path); 
    }

    std::cout << "$ ";
  }

  return 0;
}
