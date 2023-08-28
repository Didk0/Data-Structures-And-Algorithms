#pragma once
#include "Type.h"

/// @brief class deriving Type with double value

class Double : public Type
{
public:
	Double(double value = 0);

	/// @brief Creates a new Double object
	/// @return new Double
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
	virtual double toDouble() const override;

	/// @brief Gives us the size of the object in bytes
	/// @return the size of the value
	virtual unsigned int getBytes() const override { return sizeof(value); }

	/// @brief Turns the value into a string and rounds it to the 3rd digit
	/// @return the string
	virtual std::string toString() const override;

private:
	/// @brief Gives us the value of the object converted to int
	/// @return the value
	virtual int toInteger() const override;

	double value;
	const double EPS = 0.0001;
};