#pragma once

#include <functional>
#include <string>
#include <vector>

namespace cmd {
using Handler = std::function<int(const std::vector<std::string> &)>;

// Register a builtin handler. The handler receives argv-style args (excluding
// the command name)
void register_builtin(const std::string &name, Handler h);

// Dispatch tokens to a builtin. Returns true if a builtin handled the command.
bool dispatch(const std::vector<std::string> &tokens);

// Query whether a name is a registered builtin
bool is_builtin(const std::string &name);

// Register a set of default builtins (echo, exit, ...)
void register_default_builtins();
} // namespace cmd
