#include "Database.h"

void Database::createTable(const string& tableHeader, const string& tableName, const string& tableFirstIndex)
{
	if (tables.find(tableName) != tables.end())
		throw std::exception("Table with this name already exists!");

	Table newTable(tableHeader, tableName, tableFirstIndex);

	tables[tableName] = newTable;
}

void Database::dropTable(const string& tableName)
{
	if (tables.find(tableName) == tables.end())
		throw std::exception("Table with this name doesn't exist!");

	tables.erase(tableName);
}

void Database::listTables() const
{
	if (!tables.empty())
	{
		for (pair<string, Table> pair : tables)
			cout << pair.first << endl;
	}
}

void Database::insertInto(const string& tableName, const vector<string>& recordsToInsert)
{
	if (tables.find(tableName) == tables.end())
		throw std::exception("Table with this name doesn't exist!");

	tables[tableName].insert(recordsToInsert);
}

void Database::selectFrom(const string& tableName, const string& whereExpr, const string& orderBy, bool distinct, const string& toPrint, size_t& selectedCount) const
{
	if (tables.find(tableName) == tables.end())
		throw exception("Table with this name doesn't exist!");

	vector<string> selected = tables.at(tableName).select(whereExpr, orderBy, distinct, toPrint);

	if (!selected.empty())
	{
		unordered_map<string, size_t> colMaxSizes;
		parseSelected(colMaxSizes, tables.at(tableName).getColsNamesToPrint(toPrint), selected);
	}

	selectedCount = selected.size();
}

void Database::removeFrom(const string& tableName, const string& whereExpr)
{
	if (tables.find(tableName) == tables.end())
		throw std::exception("Table with this name doesn't exist!");

	tables[tableName].remove(whereExpr);
}

void Database::createIndex(const string& tableName, const string& indexCol)
{
	if (tables.find(tableName) == tables.end())
		throw std::exception("Table with this name doesn't exist!");

	tables[tableName].createIndex(indexCol);
}

Table Database::getTable(const string& tableName) const
{
	if (tables.find(tableName) == tables.end())
		throw std::exception("Table with this name doesn't exist!");

	return tables.at(tableName);
}

void Database::deserialize(ifstream& in)
{
	size_t len;
	in.read((char*)&len, sizeof(len));

	for (size_t i = 0; i < len; i++)
	{
		size_t size;
		in.read((char*)&size, sizeof(size));
		string tableName;
		tableName.resize(size);
		in.read((char*)&tableName[0], size);

		ifstream tableFile(tableName + ".bin", std::ios::binary);
		if (!tableFile)
			throw exception("Couldn't open file for reading!");

		tables.insert({ tableName, Table(tableFile) });

		tableFile.close();
	}
}

void Database::serialize(ofstream& out) const
{
	size_t len = tables.size();
	out.write((const char*)&len, sizeof(len));

	for (const pair<string, Table>& p : tables)
	{
		size_t size = p.first.length();
		out.write((const char*)&size, sizeof(size));
		out.write((const char*)&p.first[0], size);
		p.second.serialize();
	}
}

void Database::parseSelected(unordered_map<string, size_t>& maxSizesHT, const vector<string>& colNames, const vector<string>& selected) const
{
	for (size_t i = 0; i < colNames.size(); i++)
		maxSizesHT[colNames[i]] = colNames[i].size();

	vector<vector<string>> formatMat;
	formatMat.resize(selected.size());

	for (size_t i = 0; i < selected.size(); i++)
	{
		string currData;
		bool readingString = false;
		size_t ind = 0;

		for (size_t j = 0; j < selected[i].size(); j++)
		{
			if (selected[i][j] == '\"')
				readingString = !readingString;

			if (readingString)
			{
				currData += selected[i][j];
				continue;
			}

			if (selected[i][j] != ' ' || j == selected[i].size() - 1)
				currData += selected[i][j];

			if (selected[i][j] == ' ' || j == selected[i].size() - 1)
			{
				if (maxSizesHT[colNames[ind]] < std::max(currData.size(), colNames[ind].size()))
					maxSizesHT[colNames[ind]] = std::max(currData.size(), colNames[ind].size());

				ind++;
				formatMat[i].push_back(currData);
				currData.clear();
			}
		}
	}

	size_t slashesToPrint = 1;
	cout << '|';
	for (size_t i = 0; i < colNames.size(); i++)
	{
		size_t spacesToPrint = abs((long)maxSizesHT[colNames[i]] - (long)colNames[i].size());
		size_t firstHalf = spacesToPrint / 2;
		for (size_t k = 0; k < firstHalf; k++)
		{
			cout << ' ';
			slashesToPrint++;
		}
		cout << colNames[i];
		slashesToPrint += colNames[i].size() + 1;
		size_t secondHalf = spacesToPrint % 2 == 0 ? firstHalf : firstHalf + 1;
		for (size_t i = 0; i < secondHalf; i++)
		{
			cout << ' ';
			slashesToPrint++;
		}
		cout << '|';
	}

	cout << endl;

	for (size_t i = 0; i < slashesToPrint; i++)
		cout << "-";

	cout << endl;

	for (size_t i = 0; i < formatMat.size(); i++)
	{
		cout << '|';
		for (size_t j = 0; j < formatMat[i].size(); j++)
		{
			size_t spacesToPrint = maxSizesHT[colNames[j]] - formatMat[i][j].size();
			for (size_t k = 0; k < spacesToPrint; k++)
				cout << ' ';
			cout << formatMat[i][j] << '|';
		}
		cout << endl;
	}

	cout << endl;
}
