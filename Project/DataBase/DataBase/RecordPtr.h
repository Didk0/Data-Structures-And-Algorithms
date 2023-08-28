#pragma once
#include <fstream>

/// @brief Class used to show where a given Record is saved in a Page (the number of the Page and the row in the Page)

class RecordPtr
{
public:
	RecordPtr(int page = 0, int row = 0)
		: pageNum(page)
		, rowNum(row)
	{}

	/// @brief Deserializing constructor
	/// @param in - the file that the object will be read from
	RecordPtr(std::ifstream& in)
	{
		in.read((char*)&pageNum, sizeof(pageNum));
		in.read((char*)&rowNum, sizeof(rowNum));
	}

	inline size_t pageNumber() const { return pageNum; }

	inline size_t rowNumber() const { return rowNum; }

	/// @brief Saves the record pointer to a file in a binary format
	/// @param out - the file
	void serialize(std::ofstream& out) const
	{
		out.write((const char*)&pageNum, sizeof(pageNum));
		out.write((const char*)&rowNum, sizeof(rowNum));
	}

	bool operator<(const RecordPtr& other) const
	{
		if (pageNum < other.pageNum)
			return true;
		else if (pageNum == other.pageNum)
		{
			if (rowNum < other.rowNum)
				return true;
		}

		return false;
	}

	bool operator>(const RecordPtr& other) const
	{
		if (pageNum > other.pageNum)
			return true;
		else if (pageNum == other.pageNum)
		{
			if (rowNum > other.rowNum)
				return true;
		}

		return false;
	}

	bool operator==(const RecordPtr& other) const
	{
		return pageNum == other.pageNum && rowNum == other.rowNum;
	}

private:
	int pageNum, rowNum;
};

