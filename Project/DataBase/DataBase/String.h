#pragma once
#include "Type.h"

/// @brief class deriving Type with string value

class String : public Type
{
public:
	String(const std::string& value = "");
	String& operator=(const std::string& value);

	/// @brief Creates a new String object
	/// @return new String
	virtual Type* clone() const override;

	/// @brief Saves object to file in binary format
	/// @param out - the file
	virtual void serialize(std::ofstream& out) const override;

	/// @brief Reads the object from a file in binary format
	/// @param in - the file
	virtual void deserialize(std::ifstream& in) override;

	virtual bool operator<(const Type& other) const override;
	virtual bool operator<=(const Type& other) const override;
	virtual bool operator>(const Type& other) const override;
	virtual bool operator>=(const Type& other) const override;
	virtual bool operator==(const Type& other) const override;
	virtual bool operator!=(const Type& other) const override;

	/// @brief Gives us the value of the object
	/// @return the value
	virtual std::string toString() const override;

	/// @brief Gives us the value of the object converted to int
	/// @return the value
	virtual int toInteger() const override;

	/// @brief Gives us the value of the object converted to double
	/// @return the value
	virtual double toDouble() const override;

	/// @brief Gives us the size of the object in bytes
	/// @return the size of the value
	virtual unsigned int getBytes() const override { return sizeof(value); }

private:

	inline bool isUpperCase(char ch) const { return ch >= 'A' && ch <= 'Z'; }

	/// @brief Turns all upper case letters from str to lower case
	/// @param str - the string to be transformed
	/// @return the transformed string
	std::string toLower(const std::string& str) const;

	std::string value;
};

