#pragma once
#include "Tree.h"
#include <unordered_map>

using std::string;

class System
{
	using tokens = std::vector<string>;

public:
	System(const System& other) = delete;
	System& operator=(const System& other) = delete;
	~System() = default;

	static System& i();

	void run();

	void printHelp();

private:
	System() = default;

	void processCommand(const string& command, tokens& tokens);

private:
	std::unordered_map<string, Tree> trees;
};

