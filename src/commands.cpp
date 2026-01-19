#include "commands.h"
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>
#include <unordered_map>

namespace {
static std::unordered_map<std::string, cmd::Handler> builtins;
}

static int try_get_exe_path(const std::string &path, const std::string &name, std::string &outPath) {
  size_t start = 0; 

  while (start <= path.size()) {
    // Find next ':' separator (npos means "not found", i.e., last directory)
    size_t pos = path.find(':', start); 
    std::string dir = (pos == std::string::npos) 
                    ? path.substr(start)           
                    : path.substr(start, pos - start); 
    
    if (dir.empty()) dir = "."; 

    std::string full = dir + "/" + name; 


    if (access(full.c_str(), F_OK) == 0 && access(full.c_str(), X_OK) == 0) {
      outPath = full; 
      return 0;  
    }

    if (pos == std::string::npos)
      break;
    
    start = pos + 1;
  }
  
  return -1;  // Not found in any directory
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

bool cmd::dispatch_external(const std::vector<std::string> &tokens, const std::string &path)
{
   std::vector<char*> argv (tokens.size() +1); 
   std::transform(tokens.begin(), tokens.end(), argv.begin(),
  [](const std::string& s){ return const_cast<char*>(s.c_str()); });

  argv.back() = nullptr;

  pid_t pid = fork();
  if(pid < 0) {
    std::cerr << "Fork failed\n"; 
    return false; 
  }
  if(pid == 0) {
    execv(path.c_str(), argv.data());
    std::exit(1); 
  }

  waitpid(pid, nullptr, 0);
  return true; 
}

bool cmd::is_builtin(const std::string &name) {
  return builtins.find(name) != builtins.end();
}

bool cmd::is_external_command(const std::string &name, std::string &out_path)
{
  const char *path_env = std::getenv("PATH");
  if (!path_env)
  {
    std::cerr << name << ": not found\n";
    return false;
  }

  std::string path(path_env); 
  int res_state = try_get_exe_path(path, name , out_path); 
  if(res_state == -1) return false; 

  return true;
}

static int builtin_pwd(const std::vector<std::string> &) {
  char buf[4096];
  if (getcwd(buf, sizeof(buf)) != nullptr) {
    std::cout << buf << '\n';
    return 0;
  } else {
    std::perror("pwd");
    return 1;
  }
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

// specs
/*$ type grep
grep is /usr/bin/grep
$ type invalid_command
invalid_command: not found
$ type echo
echo is a shell builtin
The tester will execute your program with a custom PATH like this:
usage PATH="/usr/bin:/usr/local/bin:$PATH" ./your_program.sh
*/
static int builtin_type(const std::vector<std::string> &args) {
  if (args.empty()) {
    std::cout << "type: missing operand\n";
    return 0;
  }
  const std::string &name = args[0];
  if (cmd::is_builtin(name)) {
    std::cout << name << " is a shell builtin\n";
    return 0;
  }

  const char *path_env = std::getenv("PATH");
  if (!path_env) {
    std::cout << name << ": not found\n";
    return 0;
  }

  std::string path(path_env);
  size_t start = 0;
  while (start <= path.size()) {
    size_t pos = path.find(':', start);
    std::string dir = (pos == std::string::npos)
                          ? path.substr(start)
                          : path.substr(start, pos - start);
    if (dir.empty())
      dir = "."; // empty component -> current dir
    std::string full = dir + "/" + name;

    // check existence and exec permission
    if (access(full.c_str(), F_OK) == 0) {
      if (access(full.c_str(), X_OK) == 0) {
        std::cout << name << " is " << full << '\n';
        return 0;
      }
      // exists but not executable -> skip to next dir
    }

    if (pos == std::string::npos)
      break;
    start = pos + 1;
  }

  std::cout << name << ": not found\n";
  return 0;
}

static int builtin_exit(const std::vector<std::string> &) {
  std::exit(0);
  return 0; // unreachable
}

void cmd::register_default_builtins() {
  register_builtin("echo", builtin_echo);
  register_builtin("exit", builtin_exit);
  register_builtin("type", builtin_type);
  register_builtin("pwd", builtin_pwd);
}
