#pragma once
#include <ostream>
#include "Integer.h"
#include "String.h"
#include "Double.h"
#include "DateTime.h"

/// @brief Wrapper class for the Type class

class Data
{
public:
	Data();
	/// @brief Constructor with int param
	/// @param val - the value of the object
	Data(int val);
	/// @brief Constructor with double param
	/// @param val - the value of the object
	Data(double val);
	/// @brief Constructor with string param (can be String or DateTime)
	/// @param val - the value of the object
	Data(const std::string& val);

	/// @brief Deserializing constructor
	/// @param in - the file that the object will be read from
	Data(std::ifstream& in);

	Data(const Data& other);
	Data& operator=(const Data& other);
	~Data();

	/// @brief Saves object to file in binary format
	/// @param out - the file
	void serialize(std::ofstream& out) const;

	bool operator<(const Data& other) const;
	bool operator<=(const Data& other) const;
	bool operator>(const Data& other) const;
	bool operator>=(const Data& other) const;
	bool operator==(const Data& other) const;
	bool operator!=(const Data& other) const;

	/// @brief Gives the value into a string format depending of the type of the object
	/// @return the converted value
	std::string toString() const;

	/// @brief Sets the value to a new Integer object
	/// @param val - the given value of the object
	void setValue(int val);

	/// @brief Sets the value to a new Double object
	/// @param val - the given value of the object
	void setValue(double val);

	/// @brief Sets the value to a new String or DateTime object
	/// @param val - the given value of the object
	void setValue(const std::string& val);

	/// @brief Gives the value
	/// @return the value
	Type* getValue() const { return value; }

	/// @brief Gives us the size of the object in bytes
	/// @return the bytes of the value depending of the object type
	unsigned int getBytes() const { return value->getBytes(); }

private:
	/// @brief Helper function for the copy constructor
	/// @param other - the object we copy from
	void copy(const Data& other);

	/// @brief Helper function for the destructor
	void clear();

	/// @brief Checks if a gives string is in DateTime format
	/// @param str - the string to check
	/// @return true if the string is in DateTime format and false otherwise
	bool isDateTime(const std::string& str) const;

private:
	Type* value;
};
