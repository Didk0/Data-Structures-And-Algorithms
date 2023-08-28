#include "Table.h"

Table::Table()
	: name("")
	, currPageNumber(0)
	, bytes(0)
{}

Table::Table(const string& header, const string& name, const string& firstIndexedCol)
	: name(name)
	, currPageNumber(0)
	, bytes(0)
{
	setCollections(header);
	if (!firstIndexedCol.empty())

		indexedCols.push_back(firstIndexedCol);
	else
		indexedCols.push_back(colNames[0]);
	indexedColsRecordsHT.insert({ indexedCols[0], BPlusTree() });
	createPage();
	serialize();
}

Table::Table(std::ifstream& in)
{
	size_t len;
	in.read((char*)&len, sizeof(len));
	name.resize(len);
	in.read((char*)&name[0], len);

	in.read((char*)&currPageNumber, sizeof(currPageNumber));
	in.read((char*)&bytes, sizeof(bytes));

	in.read((char*)&len, sizeof(len));
	colNames.reserve(len);
	for (size_t i = 0; i < len; i++)
	{
		size_t size;
		in.read((char*)&size, sizeof(size));

		string colName;
		colName.resize(size);
		in.read((char*)&colName[0], size);
		colNames.push_back(colName);
	}

	in.read((char*)&len, sizeof(len));
	colTypes.reserve(len);
	for (size_t i = 0; i < len; i++)
	{
		size_t size;
		in.read((char*)&size, sizeof(size));

		string colType;
		colType.resize(size);
		in.read((char*)&colType[0], size);
		colTypes.push_back(colType);
	}

	in.read((char*)&len, sizeof(len));
	indexedCols.reserve(len);
	for (size_t i = 0; i < len; i++)
	{
		size_t size;
		in.read((char*)&size, sizeof(size));

		string colName;
		colName.resize(size);
		in.read((char*)&colName[0], size);
		indexedCols.push_back(colName);
	}

	in.read((char*)&len, sizeof(len));
	for (size_t i = 0; i < len; i++)
	{
		size_t size;
		in.read((char*)&size, sizeof(size));
		string first;
		first.resize(size);
		in.read((char*)&first[0], size);

		size_t second;
		in.read((char*)&second, sizeof(second));

		colNameIndexHT.insert({ first,second });
	}

	in.read((char*)&len, sizeof(len));
	for (size_t i = 0; i < len; i++)
	{
		size_t size;
		in.read((char*)&size, sizeof(size));
		string colName;
		colName.resize(size);
		in.read((char*)&colName[0], size);

		BPlusTree column(in);
		indexedColsRecordsHT[colName] = column;
	}
}

void Table::insert(const vector<string>& recordStr)
{
	for (size_t i = 0; i < recordStr.size(); i++)
	{
		Record record = toRecord(recordStr[i]);

		ifstream in(name + "_page" + std::to_string(currPageNumber) + ".bin", std::ios::binary);
		if (!in)
			throw std::exception("Couldn't open page for reading!");

		Page page(in);
		in.close();

		if (page.isFull())
			page = createPage();

		page.addRecord(record);
		bytes += record.getBytes();

		ofstream out(name + "_page" + std::to_string(currPageNumber) + ".bin", std::ios::binary);
		if (!out)
			throw std::exception("Couldn't open page for writing!");

		page.serialize(out);

		out.close();

		for (size_t i = 0; i < indexedCols.size(); i++)
		{
			indexedColsRecordsHT[indexedCols[i]]
				.insert({ record.getColData(colNameIndexHT[indexedCols[i]]), RecordPtr(currPageNumber, page.size() - 1) });
		}
	}
}

vector<string> Table::select(const string& expression, const string& orderByWhat, bool distinct, const string& toPrint) const
{
	vector<string> result;

	if (indexedColsRecordsHT.at(indexedCols[0]).isEmpty())
		return result;

	vector<string> expressionArr = parseExpression(expression);
	vector<Record> selected;
	vector<RecordPtr> recordPtrs;

	if (containsOnlyIndexedCols(expressionArr))
	{
		recordPtrs = getIntervals(expressionArr);
		if (recordPtrs.empty()) return result;
		transformToRecords(recordPtrs, selected);
	}
	else
	{
		recordPtrs = indexedColsRecordsHT.at(indexedCols[0])
			.getElementsInRange(indexedColsRecordsHT.at(indexedCols[0]).min(), indexedColsRecordsHT.at(indexedCols[0]).max());
		if (recordPtrs.empty()) return result;
		checkCondAndTransformToRecords(recordPtrs, selected, expressionArr);
	}

	if (orderByWhat != "")
		orderBy(selected, orderByWhat);

	if (distinct)
		makeDistinct(toPrint, result, selected);
	else
		notDistinct(toPrint, result, selected);

	return result;
}

void Table::remove(const string& expression)
{
	if (indexedColsRecordsHT.at(indexedCols[0]).isEmpty())
		return;

	vector<string> expressionArr = parseExpression(expression);

	vector<RecordPtr> recordPtrs = indexedColsRecordsHT.at(indexedCols[0])
		.getElementsInRange(indexedColsRecordsHT.at(indexedCols[0]).min(), indexedColsRecordsHT.at(indexedCols[0]).max());

	if (!areSorted(recordPtrs))
		heapSortRecordPtrs(recordPtrs);

	for (size_t i = 0; i < recordPtrs.size(); i++)
	{
		size_t num = recordPtrs[i].pageNumber();
		ifstream in(name + "_page" + std::to_string(recordPtrs[i].pageNumber()) + ".bin", std::ios::binary);
		size_t ind = i;
		if (!in)
			throw std::exception("Couldn't open page for reading!");

		Page page(in);

		if (i < recordPtrs.size() - 1 && recordPtrs[i].pageNumber() == recordPtrs[i + 1].pageNumber())
		{
			while (i < recordPtrs.size() - 1 && recordPtrs[i].pageNumber() == recordPtrs[i + 1].pageNumber())
			{
				RecordPtr recPtr = recordPtrs[i];
				Record record = page.getRecord(recordPtrs[i].rowNumber());
				if (!record.isEmpty() && (expression.empty() || checkRecordCondition(record, expressionArr)))
				{
					for (size_t k = 0; k < indexedCols.size(); k++)
					{
						indexedColsRecordsHT[indexedCols[k]]
							.remove({ record.getColData(colNameIndexHT[indexedCols[k]]), recPtr });
					}
					page.removeRecord(recordPtrs[i].rowNumber());
					bytes -= record.getBytes();
				}
				i++;
			}
			if (i < recordPtrs.size() && recordPtrs[i].pageNumber() == num)
			{
				RecordPtr recPtr = recordPtrs[i];
				Record record = page.getRecord(recordPtrs[i].rowNumber());
				if (!record.isEmpty() && (expression.empty() || checkRecordCondition(record, expressionArr)))
				{
					for (size_t k = 0; k < indexedCols.size(); k++)
					{
						indexedColsRecordsHT[indexedCols[k]]
							.remove({ record.getColData(colNameIndexHT[indexedCols[k]]), recPtr });
					}
					page.removeRecord(recordPtrs[i].rowNumber());
					bytes -= record.getBytes();
				}
			}
		}
		else if (i < recordPtrs.size() - 1 && recordPtrs[i].pageNumber() != recordPtrs[i + 1].pageNumber())
		{
			RecordPtr recPtr = recordPtrs[i];
			Record record = page.getRecord(recordPtrs[i].rowNumber());
			if (!record.isEmpty() && (expression.empty() || checkRecordCondition(record, expressionArr)))
			{
				for (size_t k = 0; k < indexedCols.size(); k++)
				{
					indexedColsRecordsHT[indexedCols[k]]
						.remove({ record.getColData(colNameIndexHT[indexedCols[k]]), recPtr });
				}
				bytes -= record.getBytes();
				page.removeRecord(recordPtrs[i].rowNumber());
			}
		}
		else if (i == recordPtrs.size() - 1)
		{
			RecordPtr recPtr = recordPtrs[i];
			Record record = page.getRecord(recordPtrs[i].rowNumber());
			if (!record.isEmpty() && (expression.empty() || checkRecordCondition(record, expressionArr)))
			{
				for (size_t k = 0; k < indexedCols.size(); k++)
				{
					indexedColsRecordsHT[indexedCols[k]]
						.remove({ record.getColData(colNameIndexHT[indexedCols[k]]), recPtr });
				}
				bytes -= record.getBytes();
				page.removeRecord(recordPtrs[i].rowNumber());
			}
		}

		in.close();

		std::ofstream out(name + "_page" + std::to_string(recordPtrs[ind].pageNumber()) + ".bin", std::ios::binary);
		if (!out)
			throw std::exception("Couldn't open page for writing!");

		page.serialize(out);

		out.close();
	}
	//serialize();
}

void Table::createIndex(const string& indexCol)
{
	if (colNameIndexHT.find(indexCol) != colNameIndexHT.end())
		throw std::invalid_argument("Column doesn't exist!");

	if (indexedColsRecordsHT.find(indexCol) != indexedColsRecordsHT.end())
		throw std::invalid_argument("This column already has an index!");

	indexedCols.push_back(indexCol);

	vector<RecordPtr> recordPtrs;
	if (!indexedColsRecordsHT.at(indexedCols[0]).isEmpty())
		recordPtrs = indexedColsRecordsHT.at(indexedCols[0])
		.getElementsInRange(indexedColsRecordsHT.at(indexedCols[0]).min(), indexedColsRecordsHT.at(indexedCols[0]).max());

	vector<Record> records;
	transformToRecords(recordPtrs, records);

	for (size_t i = 0; i < records.size(); i++)
	{
		indexedColsRecordsHT[indexCol].insert({ records[i].getColData(colNameIndexHT.at(indexCol)), recordPtrs[i] });
	}
}

void Table::serialize() const
{
	std::ofstream out(name + ".bin", std::ios::binary);

	size_t len = name.length();
	out.write((const char*)&len, sizeof(len));
	out.write((const char*)&name[0], len);

	out.write((const char*)&currPageNumber, sizeof(currPageNumber));
	out.write((const char*)&bytes, sizeof(bytes));

	len = colNames.size();
	out.write((const char*)&len, sizeof(len));
	for (size_t i = 0; i < len; i++)
	{
		size_t size = colNames[i].length();
		out.write((const char*)&size, sizeof(size));
		out.write((const char*)&colNames[i][0], size);
	}

	len = colTypes.size();
	out.write((const char*)&len, sizeof(len));
	for (size_t i = 0; i < len; i++)
	{
		size_t size = colTypes[i].length();
		out.write((const char*)&size, sizeof(size));
		out.write((const char*)&colTypes[i][0], size);
	}

	len = indexedCols.size();
	out.write((const char*)&len, sizeof(len));
	for (size_t i = 0; i < len; i++)
	{
		size_t size = indexedCols[i].length();
		out.write((const char*)&size, sizeof(size));
		out.write((const char*)&indexedCols[i][0], size);
	}

	len = colNameIndexHT.size();
	out.write((const char*)&len, sizeof(len));
	for (const pair<string, size_t>& p : colNameIndexHT)
	{
		size_t size = p.first.length();
		out.write((const char*)&size, sizeof(size));
		out.write((const char*)&p.first[0], size);

		out.write((const char*)&p.second, sizeof(p.second));
	}

	len = indexedColsRecordsHT.size();
	out.write((const char*)&len, sizeof(len));
	for (const pair<string, BPlusTree>& p : indexedColsRecordsHT)
	{
		size_t size = p.first.length();
		out.write((const char*)&size, sizeof(size));
		out.write((const char*)&p.first[0], size);

		p.second.serialize(out);
	}

	out.close();
}

string Table::getHeaderInfo() const
{
	string res;

	res.push_back('(');

	for (size_t i = 0; i < colNames.size(); i++)
	{
		res += colNames[i];
		res.push_back(':');
		res += colTypes[i];

		for (size_t j = 0; j < indexedCols.size(); j++)
		{
			if (colNames[i] == indexedCols[j])
			{
				res += ", ";
				res += "Indexed";
			}
		}

		if (i != colNames.size() - 1) res += "; ";
	}

	res.push_back(')');

	return res;
}

vector<string> Table::getColsNamesToPrint(const string& toPrint) const
{
	vector<string> result;

	if (toPrint == "*")
	{
		return colNames;
	}
	else
		result = parseColsToPrint(toPrint);

	return result;
}

Page Table::createPage()
{
	currPageNumber++;
	Page p;
	std::ofstream out(name + "_page" + std::to_string(currPageNumber) + ".bin", std::ios::binary);
	if (!out)
		throw std::exception("Couldn't open page for writing!");
	p.serialize(out);
	out.close();
	return p;
}

void Table::setCollections(const string& header)
{
	string currColName;
	string currColType;

	bool buildColName = true;
	bool buildColType = false;

	// header example: (ID:Int, Name:String, Value:Int)

	size_t ind = 0;

	for (size_t i = 1; i < header.size() - 1; i++)
	{
		if (header[i] == ' ')
			continue;
		else if (header[i] == ':' || header[i] == ',')
		{
			if (header[i] == ',')
			{
				colNames.push_back(currColName);
				colTypes.push_back(currColType);
				colNameIndexHT[currColName] = ind++;

				currColName.clear();
				currColType.clear();
			}

			buildColName = !buildColName;
			buildColType = !buildColType;
			continue;
		}

		if (buildColName)
			currColName += header[i];
		else if (buildColType)
			currColType += header[i];
	}

	colNames.push_back(currColName);
	colTypes.push_back(currColType);
	colNameIndexHT[currColName] = ind++;
}

Record Table::toRecord(const string& str) const
{
	Record res;

	size_t ind = 0;
	string currData;
	bool readingString = false;

	// str example: (1, "Test", 10)

	for (size_t i = 0; i < str.size(); i++)
	{
		if (str[i] == '\"')
			readingString = !readingString;

		if (readingString)
		{
			currData += str[i];
			continue;
		}

		if (str[i] == '(' || str[i] == ' ')
			continue;

		if (str[i] == ',' || str[i] == ')')
		{
			if (isInteger(currData))
			{
				if (colTypes[ind] != "Int")
					throw std::invalid_argument("Invalid row given!");
				Data d(std::stoi(currData));
				res.addColumn(d);
			}
			else if (isDouble(currData))
			{
				if (colTypes[ind] != "Double")
					throw std::invalid_argument("Invalid row given!");
				Data d(std::stod(currData));
				res.addColumn(d);
			}
			else if (isString(currData))
			{
				if (colTypes[ind] != "String")
					throw std::invalid_argument("Invalid row given!");
				Data d(currData);
				res.addColumn(d);
			}
			else if (isDateTime(currData))
			{
				if (colTypes[ind] != "DateTime")
					throw std::invalid_argument("Invalid row given!");
				Data d(currData);
				res.addColumn(d);
			}
			else
				throw std::invalid_argument("Invalid row given!");
			ind++;
			currData.clear();
		}
		else
			currData += str[i];
	}

	return res;
}

bool Table::isInteger(const string& data) const
{
	size_t i = 0;
	if (data[i] == '-')
	{
		i++;
		if (i == data.size()) return false;
	}
	for (i; i < data.size(); i++)
		if (!isNumber(data[i])) return false;

	return true;
}

bool Table::isDouble(const string& data) const
{
	size_t i = 0;
	if (data[i] == '-')
	{
		i++;
		if (i == data.size()) return false;
	}
	size_t dotsCnt = 0;
	for (i; i < data.size(); i++)
	{
		if (data[i] == '.')
			dotsCnt++;
		else
			if (!isNumber(data[i]))
				return false;
	}
	if (dotsCnt != 1)
		return false;

	return true;
}

bool Table::isString(const string& data) const
{
	return data[0] == '\"' && data[data.size() - 1] == '\"';
}

bool Table::isDateTime(const std::string& str) const
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

vector<string> Table::parseExpression(const string& expression) const
{
	vector<string> res;
	string currWord;

	for (size_t i = 0; i < expression.size(); i++)
	{
		if (expression[i] == '(')
		{
			res.push_back("(");
			continue;
		}

		if (expression[i] != ' ' && expression[i] != ')')
		{
			currWord += expression[i];
		}

		if ((expression[i] == ' ' || expression[i] == ')' || i == (expression.size() - 1)) && !currWord.empty())
		{
			res.push_back(currWord);
			currWord.clear();
		}

		if (expression[i] == ')')
		{
			res.push_back(")");
			continue;
		}
	}

	return res;
}

bool Table::containsOnlyIndexedCols(const vector<string>& arr) const
{
	if (arr.size() == 0)
		return false;

	for (size_t i = 0; i < arr.size(); i++)
	{
		if (arr[i] == "AND" || arr[i] == "OR" || arr[i] == "NOT"
			|| arr[i] == "<" || arr[i] == ">" || arr[i] == "<="
			|| arr[i] == ">=" || arr[i] == "!=" || arr[i] == "==")
			continue;

		bool isColName = false;
		bool isIndexedCol = false;

		for (size_t j = 0; j < colNames.size(); j++)
		{
			if (arr[i] == colNames[j])
			{
				isColName = true;
				break;
			}
		}

		if (isColName)
		{
			for (size_t j = 0; j < indexedCols.size(); j++)
			{
				if (arr[i] == indexedCols[j])
				{
					isIndexedCol = true;
					break;
				}
			}
			if (!isIndexedCol) return false;
		}
	}
	return true;
}

bool Table::checkRecordCondition(const Record& record, const vector<string>& expression) const
{
	stack<string> evaluationStack;
	queue<string> postfixQueue;

	// turning into postfix expression
	for (size_t i = 0; i < expression.size(); i++)
	{
		if (isLogicalOpOrBracket(expression[i]))
		{
			if (expression[i] == "(")
			{
				evaluationStack.push(expression[i]);
			}
			else if (expression[i] == ")")
			{
				if (evaluationStack.empty())
					throw std::exception("Invalid expression!");

				while (evaluationStack.top() != "(")
				{
					if (evaluationStack.top() == "AND" || evaluationStack.top() == "OR" || evaluationStack.top() == "NOT")
						postfixQueue.push(evaluationStack.top());

					evaluationStack.pop();
				}
				evaluationStack.pop();
			}
			else if (expression[i] == "AND" || expression[i] == "OR" || expression[i] == "NOT")
			{
				while (!evaluationStack.empty() && precedence(evaluationStack.top()) > precedence(expression[i]))
				{
					postfixQueue.push(evaluationStack.top());
					evaluationStack.pop();
				}

				evaluationStack.push(expression[i]);
			}
		}
		else
		{
			string leftArg = expression[i];
			string oper = expression[i + 1];
			string type = checkRightType(expression[i + 2]);
			Data rightArg;
			if (type == "")
			{
				throw std::exception("Invalid type!");
			}
			else if (type == "Int")
			{
				rightArg.setValue(std::stoi(expression[i + 2]));
			}
			else if (type == "Double")
			{
				rightArg.setValue(std::stod(expression[i + 2]));
			}
			else if (type == "String")
			{
				rightArg.setValue(expression[i + 2]);
			}
			else if (type == "DateTime")
			{
				rightArg.setValue(expression[i + 2]);
			}

			if (evaluate(leftArg, oper, rightArg, record))
				postfixQueue.push("true");
			else
				postfixQueue.push("false");

			i += 2;
		}
	}

	while (!evaluationStack.empty())
	{
		postfixQueue.push(evaluationStack.top());
		evaluationStack.pop();
	}

	stack<string> resultStack;

	// getting the result
	while (!postfixQueue.empty())
	{
		if (postfixQueue.front() == "true" || postfixQueue.front() == "false")
		{
			resultStack.push(postfixQueue.front());
			postfixQueue.pop();
		}
		else if (postfixQueue.front() == "AND" || postfixQueue.front() == "OR")
		{
			if (resultStack.size() >= 2)
			{
				string first = resultStack.top();
				resultStack.pop();

				string second = resultStack.top();
				resultStack.pop();

				evaluateANDOR(first, second, postfixQueue.front());
				resultStack.push(second);

				postfixQueue.pop();
			}
			else
				throw std::exception("Not enough arguments for binary expression!");
		}
		else if (postfixQueue.front() == "NOT")
		{
			if (resultStack.size() >= 1)
			{
				string first = resultStack.top();
				resultStack.pop();

				evaluateNOT(first, postfixQueue.front());
				resultStack.push(first);

				postfixQueue.pop();
			}
			else
				throw std::exception("Not enough arguments for unary expression!");
		}
	}

	if (resultStack.size() != 1 || (resultStack.top() != "true" && resultStack.top() != "false"))
		throw std::exception("Invalid expression!");

	return resultStack.top() == "true" ? true : false;
}

size_t Table::precedence(const string& arg) const
{
	if (arg == "NOT")
		return 3;
	else if (arg == "AND")
		return 2;
	else if (arg == "OR")
		return 1;

	return 0;
}

string Table::checkRightType(const string& data) const
{
	if (isInteger(data))
	{
		return "Int";
	}
	else if (isDouble(data))
	{
		return "Double";
	}
	else if (isString(data))
	{
		return "String";
	}
	else if (isDateTime(data))
	{
		return "DateTime";
	}

	return "";
}

void Table::evaluateANDOR(const string& first, string& second, const string& oper) const
{
	bool leftArg = first == "true" ? true : false;
	bool rightArg = second == "true" ? true : false;
	if (oper == "AND")
	{
		rightArg = leftArg && rightArg;
	}
	else if (oper == "OR")
	{
		rightArg = leftArg || rightArg;
	}

	rightArg == true ? second = "true" : second = "false";
}

void Table::evaluateNOT(string& first, const string& oper) const
{
	bool arg = first == "true" ? true : false;
	if (oper == "NOT")
		arg = !arg;

	first = arg == true ? "true" : "false";
}

bool Table::evaluate(const string& leftArg, const string& oper, const Data& rightArg, const Record& record) const
{
	if (colNameIndexHT.find(leftArg) == colNameIndexHT.end())
		throw std::exception("Given column doesn't exist!");

	Data recData = record.getColData(colNameIndexHT.at(leftArg));

	if (oper == ">")
		return recData > rightArg;
	else if (oper == "<")
		return recData < rightArg;
	else if (oper == ">=")
		return recData >= rightArg;
	else if (oper == "<=")
		return recData <= rightArg;
	else if (oper == "==")
		return recData == rightArg;
	else if (oper == "!=")
		return recData != rightArg;

	return false;
}

void Table::unite(const set<RecordPtr>& first, set<RecordPtr>& second) const
{
	for (const RecordPtr& elem : first)
		second.insert(elem);
}

void Table::intersect(const set<RecordPtr>& first, set<RecordPtr>& second) const
{
	for (set<RecordPtr>::iterator it = second.begin(); it != second.end();)
	{
		if (first.find(*it) == first.end())
			it = second.erase(it);
		else
			++it;
	}
}

void Table::complement(set<RecordPtr>& first, const string& colName) const
{
	set<RecordPtr> all = indexedColsRecordsHT.at(colName)
		.getElementsInRangeInSet(indexedColsRecordsHT.at(colName).min(), indexedColsRecordsHT.at(colName).max());

	for (const RecordPtr& elem : all)
	{
		if (first.find(elem) == first.end())
			first.insert(elem);
		else
			first.erase(elem);
	}
}

vector<RecordPtr> Table::getIntervals(const vector<string>& expressionArr) const
{
	vector<RecordPtr> result;
	stack<string> evaluationStack;
	queue<string> postfixQueue;

	// constructing the intervals and turning into postfix expression
	for (size_t i = 0; i < expressionArr.size(); i++)
	{
		if (isLogicalOpOrBracket(expressionArr[i]))
		{
			if (expressionArr[i] == "(")
			{
				evaluationStack.push(expressionArr[i]);
			}
			else if (expressionArr[i] == ")")
			{
				if (evaluationStack.empty())
					throw std::exception("Invalid expression!");

				while (evaluationStack.top() != "(")
				{
					if (evaluationStack.top() == "AND" || evaluationStack.top() == "OR" || evaluationStack.top() == "NOT")
						postfixQueue.push(evaluationStack.top());

					evaluationStack.pop();
				}
				evaluationStack.pop();
			}
			else if (expressionArr[i] == "AND" || expressionArr[i] == "OR" || expressionArr[i] == "NOT")
			{
				while (!evaluationStack.empty() && precedence(evaluationStack.top()) > precedence(expressionArr[i]))
				{
					postfixQueue.push(evaluationStack.top());
					evaluationStack.pop();
				}

				evaluationStack.push(expressionArr[i]);
			}
		}
		else
		{
			string colName = expressionArr[i];
			string oper = expressionArr[i + 1];
			string type = checkRightType(expressionArr[i + 2]);
			Data bound;
			if (type == "")
			{
				throw std::exception("Invalid type!");
			}
			else if (type == "Int")
			{
				bound.setValue(std::stoi(expressionArr[i + 2]));
			}
			else if (type == "Double")
			{
				bound.setValue(std::stod(expressionArr[i + 2]));
			}
			else if (type == "String")
			{
				bound.setValue(expressionArr[i + 2]);
			}
			else if (type == "DateTime")
			{
				bound.setValue(expressionArr[i + 2]);
			}
			if (oper == "!=")
			{
				Data temp = bound;
				Interval interval1 = buildInterval(colName, bound, "<", type);
				Interval interval2 = buildInterval(colName, temp, ">", type);
				postfixQueue.push(interval1.toString());
				postfixQueue.push(interval2.toString());
				postfixQueue.push("OR");
			}
			else
			{
				Interval interval = buildInterval(colName, bound, oper, type);
				postfixQueue.push(interval.toString());
			}


			i += 2;
		}
	}

	while (!evaluationStack.empty())
	{
		postfixQueue.push(evaluationStack.top());
		evaluationStack.pop();
	}

	stack<pair<string, set<RecordPtr>>> resultStack;

	size_t count = 1;

	// getting the result
	while (!postfixQueue.empty())
	{
		if (postfixQueue.front() != "AND" && postfixQueue.front() != "OR" && postfixQueue.front() != "NOT")
		{
			Interval interval(postfixQueue.front());
			string colName = interval.getColName();
			set<RecordPtr> set;
			if (!interval.isEmpty())
			{
				set = indexedColsRecordsHT.at(colName).getElementsInRangeInSet(interval.getLeftBound(), interval.getRightBound());
			}
			resultStack.push({ colName, set });
			postfixQueue.pop();
		}
		else if (postfixQueue.front() == "AND")
		{
			if (resultStack.size() >= 2)
			{
				set<RecordPtr> firstSet = resultStack.top().second;
				resultStack.pop();

				string colName = resultStack.top().first;
				set<RecordPtr> secondSet = resultStack.top().second;
				resultStack.pop();

				intersect(firstSet, secondSet);

				resultStack.push({ colName, secondSet });

				postfixQueue.pop();
			}
			else
				throw std::exception("Not enough arguments for binary expression!");
		}
		else if (postfixQueue.front() == "OR")
		{
			if (resultStack.size() >= 2)
			{
				set<RecordPtr> firstSet = resultStack.top().second;
				resultStack.pop();

				string colName = resultStack.top().first;
				set<RecordPtr> secondSet = resultStack.top().second;
				resultStack.pop();

				unite(firstSet, secondSet);

				resultStack.push({ colName, secondSet });

				postfixQueue.pop();
			}
			else
				throw std::exception("Not enough arguments for binary expression!");
		}
		else if (postfixQueue.front() == "NOT")
		{
			if (evaluationStack.size() >= 1)
			{
				string colName = resultStack.top().first;
				set<RecordPtr> firstSet = resultStack.top().second;
				resultStack.pop();

				complement(firstSet, colName);

				resultStack.push({ colName, firstSet });

				postfixQueue.pop();
			}
			else
				throw std::exception("Not enough arguments for unary expression!");
		}
	}

	if (resultStack.size() != 1)
		throw std::exception("Invalid expression!");

	for (const RecordPtr& el : resultStack.top().second)
		result.push_back(el);

	return result;
}

Interval Table::buildInterval(const string& colName, Data& bound, const string& oper, const string& type) const
{
	if (oper == ">")
	{
		Data rightBound = indexedColsRecordsHT.at(colName).max();
		bound = indexedColsRecordsHT.at(colName).getNextGreaterThan(bound);
		if (!bound.getValue())
		{
			return Interval(rightBound, rightBound, colName, type, true);
		}
		Interval interval(bound, rightBound, colName, type, false);
		return interval;
	}
	else if (oper == ">=")
	{
		if (bound < indexedColsRecordsHT.at(colName).min())
		{
			return Interval(bound, bound, colName, type, true);
		}
		Data rightBound = indexedColsRecordsHT.at(colName).max();
		Interval interval(bound, rightBound, colName, type, false);
		return interval;
	}
	else if (oper == "<")
	{
		Data leftBound = indexedColsRecordsHT.at(colName).min();
		bound = indexedColsRecordsHT.at(colName).getPrevLesserThan(bound);
		if (!bound.getValue())
		{
			return Interval(leftBound, leftBound, colName, type, true);
		}
		Interval interval(leftBound, bound, colName, type, false);
		return interval;
	}
	else if (oper == "<=")
	{
		if (bound > indexedColsRecordsHT.at(colName).max())
		{
			return Interval(bound, bound, colName, type, true);
		}
		Data leftBound = indexedColsRecordsHT.at(colName).min();
		Interval interval(leftBound, bound, colName, type, false);
		return interval;
	}
	else if (oper == "==")
	{
		if (!bound.getValue())
		{
			return Interval(Data(), Data(), colName, type, true);
		}
		return Interval(bound, bound, colName, type, false);
	}
	else
		throw std::invalid_argument("Invalid operation given!");
}

bool Table::areSorted(const vector<RecordPtr>& recordPtrs) const
{
	for (size_t i = 0; i < recordPtrs.size() - 1; i++)
	{
		if (recordPtrs[i] > recordPtrs[i + 1])
			return false;
	}

	return true;
}

void Table::transformToRecords(vector<RecordPtr>& recordPtrs, vector<Record>& records) const
{
	if (recordPtrs.empty())
		return;

	if (!areSorted(recordPtrs))
		heapSortRecordPtrs(recordPtrs);

	for (size_t i = 0; i < recordPtrs.size(); i++)
	{
		size_t num = recordPtrs[i].pageNumber();
		ifstream in(name + "_page" + std::to_string(recordPtrs[i].pageNumber()) + ".bin", std::ios::binary);
		if (!in)
			throw std::exception("Couldn't open page for reading!");

		Page page(in);

		if (i < recordPtrs.size() - 1 && recordPtrs[i].pageNumber() == recordPtrs[i + 1].pageNumber())
		{
			while (i < recordPtrs.size() - 1 && recordPtrs[i].pageNumber() == recordPtrs[i + 1].pageNumber())
			{
				records.push_back(page.getRecord(recordPtrs[i].rowNumber()));
				i++;
			}
			if (i < recordPtrs.size() && recordPtrs[i].pageNumber() == num)
			{
				records.push_back(page.getRecord(recordPtrs[i].rowNumber()));
			}
		}
		else if (i < recordPtrs.size() - 1 && recordPtrs[i].pageNumber() != recordPtrs[i + 1].pageNumber())
		{
			records.push_back(page.getRecord(recordPtrs[i].rowNumber()));
		}
		else if (i == recordPtrs.size() - 1)
		{
			records.push_back(page.getRecord(recordPtrs[i].rowNumber()));
		}

		in.close();
	}
}

void Table::checkCondAndTransformToRecords(vector<RecordPtr>& recordPtrs, vector<Record>& records, const vector<string>& expression) const
{
	if (recordPtrs.empty())
		return;

	if (!areSorted(recordPtrs))
		heapSortRecordPtrs(recordPtrs);

	for (size_t i = 0; i < recordPtrs.size(); i++)
	{
		size_t num = recordPtrs[i].pageNumber();
		ifstream in(name + "_page" + std::to_string(recordPtrs[i].pageNumber()) + ".bin", std::ios::binary);
		if (!in)
			throw std::exception("Couldn't open page for reading!");

		Page page(in);

		if (i < recordPtrs.size() - 1 && recordPtrs[i].pageNumber() == recordPtrs[i + 1].pageNumber())
		{
			while (i < recordPtrs.size() - 1 && recordPtrs[i].pageNumber() == recordPtrs[i + 1].pageNumber())
			{
				if (expression.empty() || checkRecordCondition(page.getRecord(recordPtrs[i].rowNumber()), expression))
					records.push_back(page.getRecord(recordPtrs[i].rowNumber()));
				i++;
			}
			if (i < recordPtrs.size() && recordPtrs[i].pageNumber() == num)
			{
				if (expression.empty() || checkRecordCondition(page.getRecord(recordPtrs[i].rowNumber()), expression))
					records.push_back(page.getRecord(recordPtrs[i].rowNumber()));
			}
		}
		else if (i < recordPtrs.size() - 1 && recordPtrs[i].pageNumber() != recordPtrs[i + 1].pageNumber())
		{
			if (expression.empty() || checkRecordCondition(page.getRecord(recordPtrs[i].rowNumber()), expression))
				records.push_back(page.getRecord(recordPtrs[i].rowNumber()));
		}
		else if (i == recordPtrs.size() - 1)
		{
			if (expression.empty() || checkRecordCondition(page.getRecord(recordPtrs[i].rowNumber()), expression))
				records.push_back(page.getRecord(recordPtrs[i].rowNumber()));
		}

		in.close();
	}
}

void Table::orderBy(vector<Record>& records, const string colName) const
{
	if (colNameIndexHT.find(colName) == colNameIndexHT.end())
		throw std::invalid_argument("Column doesn't exist in the table!");

	heapSortRecords(records, colName);
}

void Table::heapSortRecords(vector<Record>& records, const string colName) const
{
	int size = records.size();
	for (int i = size / 2 - 1; i >= 0; i--)
		heapifyRecords(records, size, i, colNameIndexHT.at(colName));

	for (int i = size - 1; i > 0; i--) {
		swap(records[0], records[i]);
		heapifyRecords(records, i, 0, colNameIndexHT.at(colName));
	}
}

void Table::heapifyRecords(vector<Record>& records, int size, int i, size_t colInd) const
{
	int largest = i;
	int left = 2 * i + 1;
	int right = 2 * i + 2;

	if (left < size && records[left].getColData(colInd) > records[largest].getColData(colInd))
		largest = left;

	if (right < size && records[right].getColData(colInd) > records[largest].getColData(colInd))
		largest = right;

	if (largest != i) {
		swap(records[i], records[largest]);
		heapifyRecords(records, size, largest, colInd);
	}
}

void Table::heapSortRecordPtrs(vector<RecordPtr>& recordPtrs) const
{
	int size = recordPtrs.size();
	for (int i = size / 2 - 1; i >= 0; i--)
		heapifyRecordPtrs(recordPtrs, size, i);

	for (int i = size - 1; i > 0; i--) {
		swap(recordPtrs[0], recordPtrs[i]);
		heapifyRecordPtrs(recordPtrs, i, 0);
	}
}

void Table::heapifyRecordPtrs(vector<RecordPtr>& recordPtrs, int size, int i) const
{
	int largest = i;
	int left = 2 * i + 1;
	int right = 2 * i + 2;

	if (left < size && recordPtrs[left] > recordPtrs[largest])
		largest = left;

	if (right < size && recordPtrs[right] > recordPtrs[largest])
		largest = right;

	if (largest != i) {
		swap(recordPtrs[i], recordPtrs[largest]);
		heapifyRecordPtrs(recordPtrs, size, largest);
	}
}

vector<string> Table::parseColsToPrint(const string& toPrint) const
{
	vector<string> res;
	string currCol;

	for (size_t i = 0; i < toPrint.size(); i++)
	{
		if (toPrint[i] != ' ')
			currCol += toPrint[i];
		else
		{
			res.push_back(currCol);
			currCol.clear();
		}

		if (i == toPrint.size() - 1)
			res.push_back(currCol);
	}

	return res;
}

void Table::makeDistinct(const string& toPrint, vector<string>& result, const vector<Record>& selected) const
{
	if (toPrint == "*")
	{
		for (size_t i = 0; i < selected.size(); i++)
		{
			string currRow;
			for (size_t j = 0; j < selected[i].size(); j++)
				currRow += selected[i].getColData(j).toString() + ' ';

			currRow.pop_back();
			bool found = false;
			for (size_t i = 0; i < result.size(); i++)
			{
				if (result[i] == currRow)
				{
					found = true;
					break;
				}
			}
			if (!found)
				result.push_back(currRow);
		}
	}
	else
	{
		vector<string> colsToPrint = parseColsToPrint(toPrint);

		for (size_t i = 0; i < selected.size(); i++)
		{
			string currRow;
			for (size_t j = 0; j < colsToPrint.size(); j++)
			{
				if (colNameIndexHT.find(colsToPrint[j]) == colNameIndexHT.end())
					throw std::invalid_argument("Invalid column name!");

				currRow += selected[i].getColData(colNameIndexHT.at(colsToPrint[j])).toString() + ' ';
			}
			currRow.pop_back();
			bool found = false;
			for (size_t i = 0; i < result.size(); i++)
			{
				if (result[i] == currRow)
				{
					found = true;
					break;
				}
			}
			if (!found)
				result.push_back(currRow);
		}
	}
}

void Table::notDistinct(const string& toPrint, vector<string>& result, const vector<Record>& selected) const
{
	if (toPrint == "*")
	{
		for (size_t i = 0; i < selected.size(); i++)
		{
			string currRow;
			for (size_t j = 0; j < selected[i].size(); j++)
				currRow += selected[i].getColData(j).toString() + ' ';

			currRow.pop_back();
			result.push_back(currRow);
		}
	}
	else
	{
		vector<string> colsToPrint = parseColsToPrint(toPrint);

		for (size_t i = 0; i < selected.size(); i++)
		{
			string currRow;
			for (size_t j = 0; j < colsToPrint.size(); j++)
			{
				if (colNameIndexHT.find(colsToPrint[j]) == colNameIndexHT.end())
					throw std::exception("Invalid column name!");

				currRow += selected[i].getColData(colNameIndexHT.at(colsToPrint[j])).toString() + ' ';
			}
			currRow.pop_back();
			result.push_back(currRow);
		}
	}
}
