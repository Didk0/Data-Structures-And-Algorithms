#include "Data.h"

Data::Data()
	: value(nullptr)
{}

Data::Data(int val)
{
	setValue(val);
}

Data::Data(double val)
{
	setValue(val);
}

Data::Data(const std::string& val)
{
	setValue(val);
}

Data::Data(std::ifstream& in)
{
	std::string type;
	size_t len = type.length();
	in.read((char*)&len, sizeof(len));
	type.resize(len);
	in.read((char*)&type[0], len);

	if (type == "Int")
		value = new Integer();
	else if (type == "Double")
		value = new Double();
	else if (type == "String")
		value = new String();
	else if (type == "DateTime")
		value = new DateTime();

	value->deserialize(in);
}

Data::Data(const Data& other)
{
	copy(other);
}

Data& Data::operator=(const Data& other)
{
	if (this != &other)
	{
		clear();
		copy(other);
	}
	return *this;
}

Data::~Data()
{
	clear();
}

void Data::serialize(std::ofstream& out) const
{
	std::string type;

	if (typeid(*value) == typeid(Integer))
		type = "Int";
	else if (typeid(*value) == typeid(Double))
		type = "Double";
	else if (typeid(*value) == typeid(String))
		type = "String";
	else if (typeid(*value) == typeid(DateTime))
		type = "DateTime";

	size_t len = type.length();
	out.write((const char*)&len, sizeof(len));
	out.write((const char*)&type[0], len);

	value->serialize(out);
}

bool Data::operator<(const Data& other) const
{
	return value->operator<(*other.value);
}

bool Data::operator<=(const Data& other) const
{
	return value->operator<(*other.value) || value->operator==(*other.value);
}

bool Data::operator>(const Data& other) const
{
	return value->operator>(*other.value);
}

bool Data::operator>=(const Data& other) const
{
	return value->operator>(*other.value) || value->operator==(*other.value);
}

bool Data::operator==(const Data& other) const
{
	return value->operator==(*other.value);
}

bool Data::operator!=(const Data& other) const
{
	return value->operator!=(*other.value);
}

std::string Data::toString() const
{
	return value->toString();
}

void Data::setValue(int val)
{
	value = new Integer(val);
}

void Data::setValue(double val)
{
	value = new Double(val);
}

void Data::setValue(const std::string& val)
{
	if (isDateTime(val))
		value = new DateTime(val);
	else
		value = new String(val);
}

void Data::copy(const Data& other)
{
	value = other.value == nullptr ? nullptr : other.value->clone();
}

void Data::clear()
{
	delete value;
	value = nullptr;
}

bool Data::isDateTime(const std::string& str) const
{
	std::string day, month, year;

	if (str.size() < 5)
		return false;

	size_t pipes = 0;
	for (size_t i = 0; i < str.size(); i++)
	{
		if (str[i] == '-')
			pipes++;
	}
	if (pipes != 2)
		return false;

	size_t ind = 0;

	while (ind < str.size() && str[ind] != '-')
	{
		day += str[ind];
		ind++;
	}
	ind++;

	if (day.size() < 1 || day.size() > 2)
		return false;

	while (ind < str.size() && str[ind] != '-')
	{
		month += str[ind];
		ind++;
	}
	ind++;

	if (month.size() < 1 || month.size() > 2)
		return false;

	while (ind < str.size())
	{
		year += str[ind];
		ind++;
	}

	if (year.size() < 1 || year.size() > 4)
		return false;

	return true;
}
