#include "DateTime.h"

std::string DateTime::toString() const
{
	return std::to_string(day) + "-" + std::to_string(month) + "-" + std::to_string(year);
}

void DateTime::setValue(const std::string& value)
{
	size_t i = 0;
	std::string d, m, y;
	while (value[i] != '-')
	{
		d += value[i];
		i++;
	}
	i++;

	while (value[i] != '-')
	{
		m += value[i];
		i++;
	}
	i++;

	while (i < value.size())
	{
		y += value[i];
		i++;
	}

	day = std::stoi(d);
	month = std::stoi(m);
	year = std::stoi(y);

	if (month < 1 || month > 12)
		throw std::invalid_argument("Invalid date given!");
	else
	{
		if (month == 2)
		{
			if (day < 1 || day > 28)
				throw std::invalid_argument("Invalid date given!");
		}
		else if (month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12)
		{
			if (day < 1 || day > 31)
				throw std::invalid_argument("Invalid date given!");
		}
		else
		{
			if (day < 1 || day > 30)
				throw std::invalid_argument("Invalid date given!");
		}
	}

	if (year < 0)
		throw std::invalid_argument("Invalid date given!");
}

DateTime::DateTime(const std::string& value)
	: day(1)
	, month(1)
	, year(1)
{
	if (!value.empty())
		setValue(value);
}

DateTime& DateTime::operator=(const DateTime& other)
{
	if (this != &other)
	{
		day = other.day;
		month = other.month;
		year = other.year;
	}
	return *this;
}

Type* DateTime::clone() const
{
	return new DateTime(*this);
}

void DateTime::serialize(std::ofstream& out) const
{
	std::string value = toString();
	size_t len = value.length();
	out.write((const char*)&len, sizeof(len));
	out.write((const char*)&value[0], len);
}

void DateTime::deserialize(std::ifstream& in)
{
	std::string value;
	size_t len;
	in.read((char*)&len, sizeof(len));
	value.resize(len);
	in.read((char*)&value[0], len);
	setValue(value);
}

bool DateTime::operator<(const Type& other) const
{
	size_t i = 0;
	std::string d, m, y;
	std::string otherVal = other.toString();

	while (otherVal[i] != '-')
	{
		d += otherVal[i];
		i++;
	}
	i++;

	while (otherVal[i] != '-')
	{
		m += otherVal[i];
		i++;
	}
	i++;

	while (i < otherVal.size())
	{
		y += otherVal[i];
		i++;
	}

	int otherDay = std::stoi(d);
	int otherMonth = std::stoi(m);
	int otherYear = std::stoi(y);

	if (year < otherYear)
	{
		return true;
	}
	else if (year == otherYear)
	{
		if (month < otherMonth)
		{
			return true;
		}
		else if (month == otherMonth)
		{
			if (day < otherDay)
			{
				return true;
			}
		}
	}

	return false;
}

bool DateTime::operator<=(const Type& other) const
{
	return !this->operator>(other);
}

bool DateTime::operator>(const Type& other) const
{
	size_t i = 0;
	std::string d, m, y;
	std::string otherVal = other.toString();

	while (otherVal[i] != '-')
	{
		d += otherVal[i];
		i++;
	}
	i++;

	while (otherVal[i] != '-')
	{
		m += otherVal[i];
		i++;
	}
	i++;

	while (i < otherVal.size())
	{
		y += otherVal[i];
		i++;
	}

	int otherDay = std::stoi(d);
	int otherMonth = std::stoi(m);
	int otherYear = std::stoi(y);

	if (year > otherYear)
	{
		return true;
	}
	else if (year == otherYear)
	{
		if (month > otherMonth)
		{
			return true;
		}
		else if (month == otherMonth)
		{
			if (day > otherDay)
			{
				return true;
			}
		}
	}

	return false;
}

bool DateTime::operator>=(const Type& other) const
{
	return !this->operator<(other);
}

bool DateTime::operator==(const Type& other) const
{
	size_t i = 0;
	std::string d, m, y;
	std::string otherVal = other.toString();

	while (otherVal[i] != '-')
	{
		d += otherVal[i];
		i++;
	}
	i++;

	while (otherVal[i] != '-')
	{
		m += otherVal[i];
		i++;
	}
	i++;

	while (i < otherVal.size())
	{
		y += otherVal[i];
		i++;
	}

	int otherDay = std::stoi(d);
	int otherMonth = std::stoi(m);
	int otherYear = std::stoi(y);

	return day == otherDay && month == otherMonth && year == otherYear;
}

bool DateTime::operator!=(const Type& other) const
{
	return !this->operator==(other);
}
