#pragma once
#include <string>
#include <fstream>

/// @brief Pure virtual class, used for the B+ Tree

class Type
{
public:
	virtual Type* clone() const = 0;

	virtual void serialize(std::ofstream& out) const = 0;
	virtual void deserialize(std::ifstream& in) = 0;

	virtual bool operator<(const Type& other) const = 0;
	virtual bool operator<=(const Type& other) const = 0;
	virtual bool operator>(const Type& other) const = 0;
	virtual bool operator>=(const Type& other) const = 0;
	virtual bool operator==(const Type& other) const = 0;
	virtual bool operator!=(const Type& other) const = 0;

	virtual int toInteger() const = 0;
	virtual double toDouble() const = 0;
	virtual std::string toString() const = 0;

	virtual unsigned int getBytes() const = 0;
};

