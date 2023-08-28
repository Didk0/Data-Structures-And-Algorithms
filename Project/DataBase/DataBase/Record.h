#pragma once
#include <vector>
#include "Data.h"

/// @brief A class representing one row in a table

class Record
{
public:
	Record() : bytes(0) {}

	/// @brief Deserializing constructor
	/// @param in - the file that the object will be read from
	Record(std::ifstream& in)
	{
		in.read((char*)&bytes, sizeof(bytes));

		size_t len;
		in.read((char*)&len, sizeof(len));

		for (size_t i = 0; i < len; i++)
			columns.push_back(Data(in));
	}

	/// @brief Gets the element of the table at a given index of the columns vector
	/// @param index - the index of the column
	/// @return the Data object that is a at the index of the column in a table
	Data getColData(size_t index) const
	{
		if (index >= 0 && index < columns.size())
			return columns[index];
		throw std::invalid_argument("Column index out of bounds");
	}

	/// @brief Adds Data object in the columns vector
	/// @param data - the element that will be added
	inline void addColumn(Data data) { columns.push_back(data); bytes += data.getBytes(); }

	/// @brief Gets the ammount of the columns
	/// @return the size of the columns vector
	inline size_t size() const { return columns.size(); }

	/// @brief Saves the record to a file in a binary format
	/// @param out - the file
	void serialize(std::ofstream& out) const
	{
		out.write((const char*)&bytes, sizeof(bytes));

		size_t len = columns.size();
		out.write((const char*)&len, sizeof(len));

		for (size_t i = 0; i < len; i++)
			columns[i].serialize(out);
	}

	/// @brief Clears the record (used to get the right indexe from the RecordPtr)
	inline void clear() { columns.clear(); bytes = 0; };

	/// @brief Check if the record is empty
	/// @return true if the size (bytes) of the record are 0 and false otherwise
	inline bool isEmpty() { return bytes == 0; }

	/// @brief Gets the size of the record
	/// @return the size of the recordin bytes
	unsigned int getBytes() const { return bytes; }

private:
	unsigned int bytes;
	std::vector<Data> columns;
};

