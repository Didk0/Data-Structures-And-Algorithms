#include "Integer.h"

Integer::Integer(int value)
	: value(value)
{}

Type* Integer::clone() const
{
	return new Integer(*this);
}

void Integer::serialize(std::ofstream& out) const
{
	out.write((const char*)&value, sizeof(value));
}

void Integer::deserialize(std::ifstream& in)
{
	in.read((char*)&value, sizeof(value));
}

bool Integer::operator<(const Type& other) const
{
	return value < other.toInteger();
}

bool Integer::operator<=(const Type& other) const
{
	return this->operator<(other) || this->operator==(other);
}

bool Integer::operator>(const Type& other) const
{
	return value > other.toInteger();
}

bool Integer::operator>=(const Type& other) const
{
	return this->operator>(other) || this->operator==(other);
}

bool Integer::operator==(const Type& other) const
{
	return value == other.toInteger();
}

bool Integer::operator!=(const Type& other) const
{
	return value != other.toInteger();
}

int Integer::toInteger() const
{
	return value;
}

std::string Integer::toString() const
{
	return std::to_string(value);
}

double Integer::toDouble() const
{
	return value;
}
