#include "commands.h"
#include <cstdlib>
#include <iostream>
#include <unordered_map>

namespace {
static std::unordered_map<std::string, cmd::Handler> builtins;
}

void cmd::register_builtin(const std::string &name, Handler h) {
  builtins[name] = std::move(h);
}

bool cmd::dispatch(const std::vector<std::string> &tokens) {
  if (tokens.empty())
    return true;
  auto it = builtins.find(tokens[0]);
  if (it == builtins.end())
    return false;
  std::vector<std::string> args(tokens.begin() + 1, tokens.end());
  it->second(args);
  return true;
}

bool cmd::is_builtin(const std::string &name) {
  return builtins.find(name) != builtins.end();
}

static int builtin_echo(const std::vector<std::string> &args) {
  bool newline = true;
  size_t i = 0;
  if (!args.empty() && args[0] == "-n") {
    newline = false;
    i = 1;
  }
  for (; i < args.size(); ++i) {
    if (i != (newline ? 0 : 1))
      std::cout << ' ';
    std::cout << args[i];
  }
  if (newline)
    std::cout << '\n';
  return 0;
}

static int builtin_exit(const std::vector<std::string> &) {
  std::exit(0);
  return 0; // unreachable
}

void cmd::register_default_builtins() {
  register_builtin("echo", builtin_echo);
  register_builtin("exit", builtin_exit);
}
