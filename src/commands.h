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

// Dispatch tokens to external command returns true if handeled correctly. 
bool dispatch_external(const std::vector<std::string> &tokens, const std::string &path);

// Query whether a name is a registered builtin
bool is_builtin(const std::string &name);

bool is_external_command(const std::string &name, std::string &out_path);

// Register a set of default builtins (echo, exit, ...)
void register_default_builtins();
} // namespace cmd
