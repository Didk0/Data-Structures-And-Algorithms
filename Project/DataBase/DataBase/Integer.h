#pragma once
#include "Type.h"

/// @brief class deriving Type with int value

class Integer : public Type
{
public:
	Integer(int value = 0);

	/// @brief Creates a new Integer object
	/// @return new Integer
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
	virtual int toInteger() const override;

	/// @brief Gives us the size of the object in bytes
	/// @return the size of the value
	virtual unsigned int getBytes() const override { return sizeof(value); }

	/// @brief Turns the value into a string
	/// @return the string
	virtual std::string toString() const override;

private:
	/// @brief Gives us the value of the object converted to double
	/// @return the value
	virtual double toDouble() const override;

	int value;
};

