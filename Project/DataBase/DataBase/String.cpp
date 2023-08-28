#include "String.h"

String::String(const std::string& value)
	: value(value)
{}

String& String::operator=(const std::string& value)
{
	this->value = value;
	return *this;
}

Type* String::clone() const
{
	return new String(*this);
}

void String::serialize(std::ofstream& out) const
{
	size_t len = value.length();
	out.write((const char*)&len, sizeof(len));
	out.write((const char*)&value[0], len);
}

void String::deserialize(std::ifstream& in)
{
	size_t len;
	in.read((char*)&len, sizeof(len));
	value.resize(len);
	in.read((char*)&value[0], len);
}

bool String::operator<(const Type& other) const
{
	return toLower(value) < toLower(other.toString());
}

bool String::operator<=(const Type& other) const
{
	return this->operator<(other) || this->operator==(other);
}

bool String::operator>(const Type& other) const
{
	return toLower(value) > toLower(other.toString());
}

bool String::operator>=(const Type& other) const
{
	return this->operator>(other) || this->operator==(other);
}

bool String::operator==(const Type& other) const
{
	return toLower(value) == toLower(other.toString());
}

bool String::operator!=(const Type& other) const
{
	return !(operator==(other));
}

int String::toInteger() const
{
	return value == "" ? 0 : std::stoi(value);
}

double String::toDouble() const
{
	return value == "" ? 0 : std::stod(value);
}

std::string String::toLower(const std::string& str) const
{
	std::string res;
	for (size_t i = 0; i < str.size(); i++)
	{
		if (isUpperCase(str[i]))
			res.push_back(str[i] + ('a' - 'A'));
		else res.push_back(str[i]);
	}
	return res;
}

std::string String::toString() const
{
	return value;
}
