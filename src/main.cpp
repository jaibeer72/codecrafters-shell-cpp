#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <functional>
#include <unordered_map>
#include <cstdlib>

using Handler = std::function<void(const std::vector<std::string> &)>;

std::unordered_map<std::string, Handler> commands;

// Current Commands
std::vector<std::string> split_tokens(const std::string &s)
{
  std::vector<std::string> tokens;
  std::istringstream iss(s);
  std::string tok;
  while (iss >> tok)
    tokens.push_back(tok);
  return tokens;
}

int main()
{
  // Flush after every std::cout / std::cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  commands["echo"] = [](const std::vector<std::string> &args)
  {
    bool newline = true;
    size_t start = 0;
    if (!args.empty() && args[0] == "-n")
    {
      newline = false;
      start = 1;
    }
    for (size_t i = start; i < args.size(); ++i)
    {
      if (i != start)
        std::cout << ' ';
      std::cout << args[i];
    }
    if (newline)
      std::cout << '\n';
  };

  // simple exit builtin
  commands["exit"] = [](const std::vector<std::string> &){ std::exit(0); };

  std::cout << "$ ";

  std::string line;
  while (std::getline(std::cin, line))
  {

    auto tokens = split_tokens(line);
    // strip any trailing CR (e.g. if input has \r) and tolerate pasted prompt
    for (auto &t : tokens) {
      if (!t.empty() && t.back() == '\r') t.pop_back();
    }
    // if the user pasted the prompt like "$ echo", remove leading '$' on the first token
    if (!tokens.empty() && !tokens[0].empty() && tokens[0][0] == '$') {
      tokens[0] = tokens[0].substr(1);
      if (tokens[0].empty() && tokens.size() > 1) tokens.erase(tokens.begin());
    }
    if (tokens.empty())
    {
      std::cout << "$ ";
      continue;
    }
    std::string cmd = tokens[0];
    std::vector<std::string> args(tokens.begin() + 1, tokens.end());

    auto it = commands.find(cmd);
    if (it != commands.end())
    {
      it->second(args);
    }
    else
    {
      std::cerr << cmd << ": command not found\n";
    }
    std::cout << "$ ";
  }

  return 0;
}
