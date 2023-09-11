#include "console.hpp"

// Map from command name to function
std::unordered_map<std::string, CommandFunc> commands;

void Console::Log(const std::string &message)
{
    log_.push_back(message);
    // also print to stdout for debugging
    printf("%s\n", message.c_str());
}

void Console::Clear()
{
    log_.clear();
}

const std::vector<std::string> &Console::GetLog() const
{
    return log_;
}

// Register a command
void registerCommand(const std::string &name, CommandFunc func)
{
    commands[name] = func;
}

// Parse input string into command and arguments
std::vector<std::string> parseInput(const std::string &input)
{
    std::vector<std::string> parts;
    // Split input on spaces
    size_t pos = 0;
    std::string token;
    while ((pos = input.find(" ")) != std::string::npos) {
        token = input.substr(0, pos);
        parts.push_back(token);
        input.erase(0, pos + 1);
    }
    return parts;
}

// Run a command
void runCommand(const std::string &input)
{

    auto parts = parseInput(input);
    auto cmd = parts[0];

    if (commands.count(cmd) == 0)
    {
        console.Log("Unknown command: " + cmd);
        return;
    }

    commands[cmd](parts); // Call command function
}

// Sample commands
void helpCommand(const std::vector<std::string> &args)
{
    console.Log("Available commands:");
	for(const auto &command : commands) {
		console.Log(command.first);
	}
}

void loadCommand(const std::vector<std::string> &args)
{
    std::string mapName = args[1];
    currentWorld = loadMap(mapName);
    inGame = true;
}

void exitCommand(const std::vector<std::string> &args)
{
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void registerDefaultCommands() {
    registerCommand("help", helpCommand);
	registerCommand("load", loadCommand);
	registerCommand("exit", exitCommand);
}