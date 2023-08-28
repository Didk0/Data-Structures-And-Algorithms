#pragma once
#include <iostream>
#include <fstream>
#include "interface.h"

using std::vector;

class System
{
	using tokens = std::vector<string>;

public:
	System(const System& other) = delete;
	System& operator=(const System& other) = delete;
	~System();

	static System& i();

	void run();

	void printHelp();

private:
	System();

	void processCommand(const string& command, tokens& tokens);
	std::vector<string> splitByDelim(string str, char delimeter);
	void checkIfInvalid(const string& hierName);
	bool isValid(char ch);
	Hierarchy* findHierarchy(const string& hierName);

	void readFromFile(const string& file, const string& hierName);
	void writeToFile(const string& file, const Hierarchy* hier);

private:
	vector<Hierarchy*> fVector;
};

