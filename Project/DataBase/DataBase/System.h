#pragma once
#include "Database.h"

class System
{
	using tokens = vector<string>;

public:
	System(const System& other) = delete;
	System& operator=(const System& other) = delete;
	~System() = default;

	static System& i();

	/// @brief Runs the main logic of the program and executes commands
	void run();

	/// @brief Prints help
	void help();

private:
	System() {};

	/// @brief Parses a command into tokens
	/// @param command - the given command as a string
	/// @param tokens - the parsed result
	void processCommand(const string& command, tokens& tokens) const;

	/// @brief Parses a string from the insert command to vector of records in string format
	/// @param toInsert - records to insert
	/// @return the vector of string records
	vector<string> parseRecordsToInsert(const string& toInsert) const;

private:
	Database db;
};

