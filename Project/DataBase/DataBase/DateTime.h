#pragma once
#include "Type.h"

/// @brief class deriving Type with value in date format

class DateTime : public Type
{
public:
	DateTime(const std::string& value = "");
	DateTime& operator=(const DateTime& other);

	/// @brief Creates a new DateTime object
	/// @return new DateTime
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

	/// @brief Turns the date into a string in day-month-year format
	/// @return the string
	virtual std::string toString() const override;

	/// @brief Gives us the size of the object in bytes
	/// @return the sum of sizes of day, month and year
	virtual unsigned int getBytes() const override { return sizeof(day) + sizeof(month) + sizeof(year); }
private:
	/// @brief Gives the day
	/// @return the day
	virtual int toInteger() const override { return day; };

	/// @brief Gives the day
	/// @return the day
	virtual double toDouble() const override { return day; };

	/// @brief Reads a string and sets values to day, month and year 
	/// @param value - the string to be read
	void setValue(const std::string& value);

	int day;
	int month;
	int year;
};

