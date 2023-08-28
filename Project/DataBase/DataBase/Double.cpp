#include "Double.h"

Double::Double(double value)
	: value(value)
{}

Type* Double::clone() const
{
	return new Double(*this);
}

void Double::serialize(std::ofstream& out) const
{
	out.write((const char*)&value, sizeof(value));
}

void Double::deserialize(std::ifstream& in)
{
	in.read((char*)&value, sizeof(value));
}

bool Double::operator<(const Type& other) const
{
	return (other.toDouble() - value) > ((abs(value) < abs(other.toDouble()) ? abs(other.toDouble()) : abs(value)) * EPS);
}

bool Double::operator<=(const Type& other) const
{
	return this->operator<(other) || this->operator==(other);
}

bool Double::operator>(const Type& other) const
{
	return (value - other.toDouble()) > ((abs(value) < abs(other.toDouble()) ? abs(other.toDouble()) : abs(value)) * EPS);
}

bool Double::operator>=(const Type& other) const
{
	return this->operator>(other) || this->operator==(other);
}

bool Double::operator==(const Type& other) const
{
	return abs(other.toDouble() - value) < EPS;
}

bool Double::operator!=(const Type& other) const
{
	return !(operator==(other));
}

int Double::toInteger() const
{
	return value;
}

std::string Double::toString() const
{
	std::string temp = std::to_string(value);
	std::string res;
	size_t round = 4;

	for (size_t i = 0; i < temp.size(); i++)
	{
		if (temp[i] == '.')
		{
			while (i < temp.size() && round > 0)
			{
				res.push_back(temp[i]);
				i++;
				round--;
			}
			break;
		}
		res.push_back(temp[i]);
	}
	return res;
}

double Double::toDouble() const
{
	return value;
}
