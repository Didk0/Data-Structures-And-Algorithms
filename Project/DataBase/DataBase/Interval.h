#pragma once
#include "Data.h"

using std::string;

/// @brief A helper class representing an interval [A, B]

class Interval
{
public:
	Interval(const Data& left, const Data& right, const string& col, const string& t, bool emp = false)
		: leftBound(left)
		, rightBound(right)
		, colName(col)
		, type(t)
		, empty(emp)
	{}

	/// @brief Constructor that sets the members of the class from a given string
	/// @param str - the interval in a string format
	Interval(const string& str)
	{
		size_t i = 0;
		while (str[i] != ' ')
			type.push_back(str[i++]);
		i++;

		bool readingStr = false;
		string left;
		while (str[i] != ' ')
		{
			if (str[i] == '"')
			{
				readingStr = true;
				i++;
				while (str[i] != '"')
					left.push_back(str[i++]);
			}
			else
				left.push_back(str[i]);
			i++;
		}
		i++;
		if (readingStr)
		{
			left.insert(left.begin(), '"');
			left.push_back('"');
		}

		string right;
		while (str[i] != ' ')
		{
			if (str[i] == '"')
			{
				i++;
				while (str[i] != '"')
					right.push_back(str[i++]);
			}
			else
				right.push_back(str[i]);
			i++;
		}
		i++;
		if (readingStr)
		{
			right.insert(right.begin(), '"');
			right.push_back('"');
		}		

		if (type == "Int")
		{
			leftBound.setValue(std::stoi(left));
			rightBound.setValue(std::stoi(right));
		}
		else if (type == "Double")
		{
			leftBound.setValue(std::stod(left));
			rightBound.setValue(std::stod(right));
		}
		else if (type == "String")
		{
			leftBound.setValue(left);
			rightBound.setValue(right);
		}

		while (str[i] != ' ')
			colName.push_back(str[i++]);
		i++;

		string isEmpty;
		while (i < str.size())
			isEmpty.push_back(str[i++]);

		empty = isEmpty == "true" ? true : false;
	}

	/// @brief Checks if the Interval is empty (example: [7, 5]) 
	/// @return true if it the Interval empty and false otherwise
	bool isEmpty() const { return empty; }

	/// @brief Converts the Interval to a string format
	/// @return the converted Interval
	string toString() const
	{
		string result;

		result += type + " ";
		result += leftBound.toString() + " " + rightBound.toString() + " ";
		result += colName + " " + (empty == true ? "true" : "false");

		return result;
	}
	string getColName() const { return colName; }
	const Data getLeftBound() const { return leftBound; }
	const Data getRightBound() const { return rightBound; }

private:
	Data leftBound;
	Data rightBound;
	string colName;
	string type;
	bool empty;
};

