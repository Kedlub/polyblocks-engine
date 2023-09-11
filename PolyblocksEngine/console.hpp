#ifndef CONSOLE_HPP
#define CONSOLE_HPP
#include <string>
#include <vector>

class Console
{
public:
    void Log(const std::string &message);
    void Clear();
    const std::vector<std::string> &GetLog() const;

private:
    std::vector<std::string> log_;
};

// Command function signature
using CommandFunc = std::function<void(const std::vector<std::string> &args)>;

void registerCommand(const std::string &name, CommandFunc func);
void runCommand(const std::string &input);
void registerDefaultCommands();
#endif // !CONSOLE_HPP