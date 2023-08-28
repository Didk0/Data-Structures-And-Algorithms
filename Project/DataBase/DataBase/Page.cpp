#include "Page.h"

Page::Page(size_t max)
	: maxRecordsSize(max)
{}

Page::Page(std::ifstream& in)
{
	in.read((char*)&maxRecordsSize, sizeof(maxRecordsSize));

	size_t len;
	in.read((char*)&len, sizeof(len));
	for (size_t i = 0; i < len; i++)
	{
		recordVec.push_back(Record(in));
	}
}

void Page::addRecord(const Record& record)
{
	if (isFull())
		throw std::overflow_error("Page is full");

	recordVec.push_back(record);
}

void Page::removeRecord(size_t index)
{
	if (index < 0 || index >= recordVec.size())
		throw std::invalid_argument("Record index out of bounds");

	recordVec[index].clear();
}

Record Page::getRecord(size_t index)
{
	if (index < 0 || index >= recordVec.size())
		throw std::exception("Record index out of bounds");

	return recordVec[index];
}

void Page::serialize(std::ofstream& out) const
{
	out.write((const char*)&maxRecordsSize, sizeof(maxRecordsSize));

	size_t len = recordVec.size();
	out.write((const char*)&len, sizeof(len));

	for (size_t i = 0; i < len; i++)
	{
		recordVec[i].serialize(out);
	}
}
