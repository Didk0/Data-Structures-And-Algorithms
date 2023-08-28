#pragma once
#include <unordered_map>
#include "Record.h"

/// @brief A class representing a file with given size. It holds a vector of Records.

class Page
{
public:
	Page(size_t max = 10);

	/// @brief Deserializing constructor
	/// @param in - the file that the object will be read from
	Page(std::ifstream& in);

	/// @brief Adds a record into the records vector
	/// @param record - the record to add
	void addRecord(const Record& record);

	/// @brief Clears a record from the vector at a given index but doesn't remove it from the vector keeping the order of indexes in the vector 
	/// @param index - the index where a record will be cleared
	void removeRecord(size_t index);

	/// @brief Gets the record at a given index from the vector
	/// @param index - the index of the record
	/// @return the record at the index
	Record getRecord(size_t index);
	
	/// @brief Saves the page to a file in a binary format
	/// @param out - the file
	void serialize(std::ofstream& out) const;

	/// @brief Checks if the page (the vector of records) is full
	/// @return true if the size of the vector is the max size of the page and false otherwise
	inline bool isFull() const { return recordVec.size() == maxRecordsSize; }

	/// @brief Checks if the page (the vector of records) is empty
	/// @return true if the vector is empty
	inline bool isEmpty() const { return recordVec.size() == 0; }

	/// @brief Gets the ammount of records in the vector (counting the empty ones)
	/// @return the size of the vector
	inline size_t size() { return recordVec.size(); }

private:
	size_t maxRecordsSize;
	std::vector<Record> recordVec;
};

