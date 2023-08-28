#pragma once
#include <iostream>
#include "Table.h"

using std::cout;
using std::exception;
using std::endl;

/// @brief Class for the database

class Database
{
public:
	Database() {};

	/// @brief Creates a new table and adds to the database
	/// @param tableHeader - the header of the table
	/// @param tableName - the name of the table
	/// @param tableFirstIndex - the first indexed column
	void createTable(const string& tableHeader, const string& tableName, const string& tableFirstIndex = "");

	/// @brief Removes a table from the database
	/// @param tableName - the name of the table
	void dropTable(const string& tableName);

	/// @brief Prints the names of all of the tables in the database
	void listTables() const;

	/// @brief Inserts rows into a given table
	/// @param tableName - the name of the table to be inserted in
	/// @param recordsToInsert - vector of records in string format
	void insertInto(const string& tableName, const vector<string>& recordsToInsert);

	/// @brief Selects certain rows from the table by given criteria
	/// @param tableName the name of the table to be selected from
	/// @param whereExpr - the WHERE expression
	/// @param orderBy - the column the record will be ordered by
	/// @param distinct - flag that shows if same records will be printed
	/// @param toPrint - columns to be printed
	/// @param selectedCount - ammount of the selected records
	void selectFrom(const string& tableName, const string& whereExpr, const string& orderBy, bool distinct, const string& toPrint, size_t& selectedCount) const;
	
	/// @brief Removes rows from a given table by a criteria
	/// @param tableName - the name of the table
	/// @param whereExpr - the WHERE expression
	void removeFrom(const string& tableName, const string& whereExpr);

	/// @brief Creates an index on a given column in a table
	/// @param tableName - the name of the table
	/// @param indexCol - the name of the column to be indexed
	void createIndex(const string& tableName, const string& indexCol);

	/// @brief Gets the number of tables in the database
	/// @return the size of the tables
	inline size_t size() const { return tables.size(); }

	/// @brief Checks if a given table is in the database
	/// @param tableName - the name of the table
	/// @return true if the table is in the database and false otherwise
	inline bool contains(const string& tableName) { return tables.find(tableName) != tables.end(); }

	/// @brief Gets a table with a given name
	/// @param tableName - the name of the table
	/// @return the table
	Table getTable(const string& tableName) const;

	/// @brief Reads the database from a file
	/// @param in - the file to be read from
	void deserialize(ifstream& in);

	/// @brief Writes the database to a file in a binary format
	/// @param out - the file to be written in
	void serialize(ofstream& out) const;

private:
	/// @brief Prints the selected rows in the select function
	/// @param maxSizesHT - hash table containing the maximum sizes of every column in the print format
	/// @param colNames - names of the columns to be printed
	/// @param selected - rows to be printed
	void parseSelected(unordered_map<string, size_t>& maxSizesHT, const vector<string>& colNames, const vector<string>& selected) const;

private:
	unordered_map<string, Table> tables;
};

