#pragma once
#include "BPlusTree.h"
#include "Page.h"
#include "Interval.h"
#include <string>
#include <unordered_map>
#include <stack>
#include <queue>
#include <forward_list>

using std::string;
using std::unordered_map;
using std::stack;
using std::queue;
using std::forward_list;
using std::swap;

/// @brief Class for a table in the database

class Table
{
public:
	Table();
	Table(const string& header, const string& name, const string& firstIndexedCol);
	Table(std::ifstream& in);

	/// @brief Insert a row in the table
	/// @param recordStr - the record that will be inserted in a string format
	void insert(const vector<string>& recordStr);

	/// @brief Filters records of the table by given criteria
	/// @param expression - the WHERE expression
	/// @param orderByWhat - the columns that will be shown on the console
	/// @param distinct - boolean that shows if same records in toPrint will be printed on the console or not
	/// @param toPrint - the columns that will be shown on the console
	/// @return vector rows in string format 
	vector<string> select(const string& expression, const string& orderByWhat, bool distinct, const string& toPrint) const;

	/// @brief Removes records from the table by given criteria
	/// @param expression - the criteria expression in string format
	void remove(const string& expression);

	/// @brief Creates an index on a given column for faster searching in this column
	/// @param indexCol - the column to be indexed
	void createIndex(const string& indexCol);

	/// @brief Gets the ammount of the rows in the table
	/// @return the size of the first column
	size_t size() const 
	{ 
		if (indexedColsRecordsHT.empty()) return 0;
		return indexedColsRecordsHT.at(indexedCols[0]).getSize();
	};

	/// @brief Saves the table to a file with name - the name of the table in binary format
	void serialize() const;

	/// @brief Gets the header of the table (columns, indexed columns etc.)
	/// @return the header in string format
	string getHeaderInfo() const;

	/// @brief Gives the columns that will be printed in the search method
	/// @param toPrint - columns to be printed
	/// @return vector of column names to be printed
	vector<string> getColsNamesToPrint(const string& toPrint) const;

	/// @brief Gets the size of the table
	/// @return the size in bytes
	inline unsigned int getBytes() const { return bytes; }

private:
	/// @brief Creates a Page (new file with name - <name_of_table>_page<currPageNumber>)
	/// @return the newly created page
	Page createPage();

	/// @brief Initializes some of the data structures used in the table
	/// @param header - header of the table
	void setCollections(const string& header);

	/// @brief Converts a string to a Record
	/// @param str - string to be converted
	/// @return the Record converted from the string
	Record toRecord(const string& str) const;

	inline bool isNumber(const char c) const { return c >= '0' && c <= '9'; };

	/// @brief Checks if a string is an Integer object
	/// @param data - the string to be checked
	/// @return true if the string is Integer and false otherwise
	bool isInteger(const string& data) const;

	/// @brief Checks if a string is a Double object
	/// @param data - the string to be checked
	/// @return true if the string is Double and false otherwise
	bool isDouble(const string& data) const;

	/// @brief Checks if a string is a String object
	/// @param data - the string to be checked
	/// @return true if the string is String and false otherwise
	bool isString(const string& data) const;

	/// @brief Checks if a string is a DateTime object
	/// @param data - the string to be checked
	/// @return true if the string is DateTime and false otherwise
	bool isDateTime(const std::string& str) const;

	/// @brief Parses an expression from the search method
	/// @param expression - the expression to be parsed
	/// @return a vector of strings of the parsed expression
	vector<string> parseExpression(const string& expression) const;

	/// @brief Checks if a vector of strings contains only the names of the indexed columns
	/// @param arr - the vector
	/// @return true if the vec contains only the names of the indexed columns and false otherwise
	bool containsOnlyIndexedCols(const vector<string>& arr) const;

	/// @brief Checks if a record satisfies a given search condition
	/// @param record - the given record
	/// @param expression - the search expression
	/// @return true if it satisfies and false otherwise
	bool checkRecordCondition(const Record& record, const vector<string>& expression) const;

	inline bool isLogicalOpOrBracket(const string& str) const
	{
		return str == "(" || str == ")" || str == "AND" || str == "OR" || str == "NOT";
	}

	/// @brief Gets the precedence of a given logical operation
	/// @param arg - the logical operation
	/// @return number depending on the operation
	size_t precedence(const string& arg) const;

	/// @brief Checks the type of a Data object
	/// @param data - the Data in a string format
	/// @return the type of the object in a string format
	string checkRightType(const string& data) const;

	/// @brief Evaluates the result of 2 booleans and logical operation
	/// @param first - the first bool in string format
	/// @param second - the second bool in string format
	/// @param oper - the operation
	void evaluateANDOR(const string& first, string& second, const string& oper) const;

	/// @brief Evaluates the result of boolean and not operation
	/// @param first - the boolean in string format
	/// @param oper - the operation
	void evaluateNOT(string& first, const string& oper) const;

	/// @brief Compares two Data objects from the same type
	/// @param leftArg - the first Data object
	/// @param oper - the comparative operation
	/// @param rightArg - the second Data object
	/// @param record - record to get the one Data from
	/// @return the result of the evaluation
	bool evaluate(const string& leftArg, const string& oper, const Data& rightArg, const Record& record) const;

	/// @brief Unites two sets
	/// @param first - the first set
	/// @param second - the second set
	void unite(const set<RecordPtr>& first, set<RecordPtr>& second) const;

	/// @brief Intersects two sets
	/// @param first - the first set
	/// @param second - the second set
	void intersect(const set<RecordPtr>& first, set<RecordPtr>& second) const;

	/// @brief Gets the complement of a set
	/// @param first - the set
	/// @param colName - name of a indexed column
	void complement(set<RecordPtr>& first, const string& colName) const;

	/// @brief Uses shunting yard algorithm with sets, intersection, union and complement
	/// @param expressionArr - the search expression
	/// @return - the result set
	vector<RecordPtr> getIntervals(const vector<string>& expressionArr) const;

	/// @brief Builds an Interval object
	/// @param colName - the name of the indexed column
	/// @param bound - one of the bounds of the interval
	/// @param oper - the comparation operation
	/// @param type - the type of the Data 
	/// @return the built Interval object
	Interval buildInterval(const string& colName, Data& bound, const string& oper, const string& type) const;

	/// @brief Checks if a vector of record pointers is sorted
	/// @param recordPtrs - the vector
	/// @return true if the vector is sorted and false otherwise
	bool areSorted(const vector<RecordPtr>& recordPtrs) const;

	/// @brief Converts a vector of record pointers to vector of records
	/// @param recordPtrs - the vector of record pointers
	/// @param records - the vector of records
	void transformToRecords(vector<RecordPtr>& recordPtrs, vector<Record>& records) const;

	/// @brief Converts a vector of record pointers to vector of records satisfying given search condition
	/// @param recordPtrs - the vector of record pointers
	/// @param records - the vector of records
	/// @param expression - the search or remove expression
	void checkCondAndTransformToRecords(vector<RecordPtr>& recordPtrs, vector<Record>& records, const vector<string>& expression) const;

	/// @brief Sorts a vector of records using heap sort algorithm
	/// @param records - records to be sorted
	/// @param colName - the name of a indexed col
	void orderBy(vector<Record>& records, const string colName) const;
	void heapSortRecords(vector<Record>& records, const string colName) const;
	void heapifyRecords(vector<Record>& records, int size, int i, size_t colInd) const;
	void heapSortRecordPtrs(vector<RecordPtr>& recordPtrs) const;
	void heapifyRecordPtrs(vector<RecordPtr>& recordPtrs, int size, int i) const;

	/// @brief Converts a string of columns to a vector of strings
	/// @param toPrint - the column names in string format
	/// @return the vector of column names strings
	vector<string> parseColsToPrint(const string& toPrint) const;

	/// @brief Removes repetitive records using a set
	/// @param distinctSet - the set of strings
	/// @param toPrint - columns to be printed in string format
	/// @param result - the distinct strings in a vector
	/// @param selected - the selected records from the search operation
	void makeDistinct(const string& toPrint, vector<string>& result, const vector<Record>& selected) const;

	/// @brief Transfers a vector of records to a vector of strings
	/// @param toPrint - columns to be printed in string format
	/// @param result - the result vector of rows in string format
	/// @param selected - the selected records from the search operation
	void notDistinct(const string& toPrint, vector<string>& result, const vector<Record>& selected) const;

private:
	string name;
	size_t currPageNumber;
	unsigned int bytes;

	vector<string> colNames; // ID, Name, Date, Value, ...  
	vector<string> colTypes; // Int, String, Double, ...		
	vector<string> indexedCols; // ID, ...					
	unordered_map<string, size_t> colNameIndexHT; // (ID,0), (Name,1), ...
	unordered_map<string, BPlusTree> indexedColsRecordsHT; // (ID, Tree1), (Name, Tree2), ...
};

