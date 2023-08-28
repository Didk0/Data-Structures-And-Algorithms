#include "System.h"

using std::cerr;
using std::getline;
using std::cin;
using std::endl;

System& System::i()
{
	static System sys;
	return sys;
}

void System::run()
{
	help();

	ifstream in("database.bin", std::ios::binary);
	if (in)
	{
		try
		{
			db.deserialize(in);
		}
		catch (const exception& e)
		{
			cerr << e.what();
		}
		in.close();
	}

	tokens tokens;
	string command;

	while (true)
	{
		cout << "FMISql> ";

		getline(cin, command);

		try
		{
			processCommand(command, tokens);
		}
		catch (const exception& e)
		{
			cerr << e.what() << endl;
			cin.clear();
			continue;
		}

		if (tokens[0] == "CreateTable")
		{
			if (tokens.size() < 3)
			{
				cerr << "Invalid command!" << endl;
				cin.clear();
				continue;
			}

			string tableName = tokens[1];
			string tableHeader = tokens[2];
			if (tokens.size() == 3)
			{
				try
				{
					db.createTable(tableHeader, tableName);
				}
				catch (const exception& e)
				{
					cerr << e.what() << endl;
					cin.clear();
					continue;
				}
				cout << "Table " << tableName << " created!" << endl;
			}
			else if (tokens.size() == 6)
			{
				if (tokens[3] != "Index" || tokens[4] != "ON")
				{
					cerr << "Invalid command!" << endl;
					cin.clear();
					continue;
				}

				string indexedCol = tokens[5];
				try
				{
					db.createTable(tableHeader, tableName, indexedCol);
				}
				catch (const exception& e)
				{
					cerr << e.what() << endl;
					cin.clear();
					continue;
				}
				cout << "Table " << tableName << " created!" << endl;
			}
			else
				cerr << "Invalid command!" << endl;
		}
		else if (tokens[0] == "DropTable")
		{
			if (tokens.size() == 2)
			{
				string tableName = tokens[1];
				try
				{
					db.dropTable(tableName);
				}
				catch (const exception& e)
				{
					cerr << e.what() << endl;
					cin.clear();
					continue;
				}
				cout << "Table " << tableName << " deleted!" << endl;
			}
			else
				cerr << "Invalid command!" << endl;
		}
		else if (tokens[0] == "ListTables")
		{
			if (tokens.size() == 1)
			{
				cout << "There " << (db.size() == 1 ? "is " : "are ") << db.size()
					<< (db.size() == 1 ? " table " : " tables ") << "in the database" << (db.size() == 0 ? "." : " : ");
				if (db.size() > 0)
				{
					cout << endl;
					db.listTables();
				}
			}
			else
				cerr << "Invalid command!";

			cout << endl;
		}
		else if (tokens[0] == "TableInfo")
		{
			if (tokens.size() == 2)
			{
				string tableName = tokens[1];
				if (db.contains(tableName))
				{
					cout << "Table " << tableName << " : " << db.getTable(tableName).getHeaderInfo() << endl;
					cout << "Total " << db.getTable(tableName).size() << " rows (" <<
						db.getTable(tableName).getBytes() * 0.001 << " KB data) in the table" << endl;
				}
				else
					cerr << "There is no such table!" << endl;
			}
			else
				cerr << "Invalid command!" << endl;
		}
		else if (tokens[0] == "Insert")
		{
			if (tokens.size() == 4)
			{
				if (tokens[1] != "INTO")
				{
					cerr << "Invalid command!" << endl;
					cin.clear();
					continue;
				}

				string tableName = tokens[2];
				string toInsert = tokens[3];

				vector<string> recordsToInsert = parseRecordsToInsert(toInsert);

				try
				{
					db.insertInto(tableName, recordsToInsert);
				}
				catch (const exception& e)
				{
					cerr << e.what() << endl;
					cin.clear();
					continue;
				}
				cout << recordsToInsert.size() << " rows inserted." << endl;
			}
			else
				cerr << "Invalid command!" << endl;
		}
		else if (tokens[0] == "Select")
		{
			if (tokens.size() < 4)
			{
				cerr << "Invalid command!" << endl;
				cin.clear();
				continue;
			}

			size_t ind = 1;
			string toPrint = "";

			while (tokens[ind] != "FROM" && ind != tokens.size() - 1)
			{
				toPrint += tokens[ind] + " ";
				ind++;
			}
			toPrint.pop_back();

			if (ind == tokens.size() - 1)
			{
				cerr << "Invalid command!" << endl;
				cin.clear();
				continue;
			}

			ind++;
			string tableName = tokens[ind];
			string whereExpr = "";
			string order = "";
			bool distinct = false;

			if (ind == tokens.size() - 1 || tokens[ind + 1] != "WHERE")
				whereExpr = "";

			if (ind + 1 < tokens.size() && tokens[ind + 1] == "WHERE")
			{
				ind += 2;
				while (tokens[ind] != "ORDER" && tokens[ind] != "DISTINCT" && ind != tokens.size() - 1)
				{
					whereExpr += tokens[ind] + " ";
					ind++;
				}
				if (ind == tokens.size() - 1)
					whereExpr += tokens[ind];
				else
					whereExpr.pop_back();

				if (tokens[ind] == "ORDER")
				{
					order = tokens[ind + 2];
					ind += 3;
				}
				if (ind < tokens.size() && tokens[ind] == "DISTINCT")
					distinct = true;
			}
			else if (ind + 1 < tokens.size() && tokens[ind + 1] == "ORDER")
			{
				ind += 3;
				order = tokens[ind];
				if (ind + 1 < tokens.size() && tokens[ind + 1] == "DISTINCT")
					distinct = true;
			}
			else if (ind + 1 < tokens.size() && tokens[ind + 1] == "DISTINCT")
			{
				if (ind + 2 != tokens.size())
				{
					cerr << "Invalid command!" << endl;
					cin.clear();
					continue;
				}
				distinct = true;
			}

			size_t selectedCount = 0;

			try
			{
				db.selectFrom(tableName, whereExpr, order, distinct, toPrint, selectedCount);
			}
			catch (const exception& e)
			{
				cerr << e.what() << endl;
				cin.clear();
				continue;
			}
			cout << "Total " << selectedCount << " rows selected" << endl;
		}
		else if (tokens[0] == "Remove")
		{
			if (tokens.size() < 5)
			{
				cerr << "Invalid command!" << endl;
				cin.clear();
				continue;
			}

			if (tokens[1] != "FROM")
			{
				cerr << "Invalid command!" << endl;
				cin.clear();
				continue;
			}

			size_t ind = 2;
			string tableName = tokens[ind];
			string whereExpr = "";
			ind += 2;

			if (ind != tokens.size() - 1 && tokens[ind] != "WHERE")
			{
				while (ind != tokens.size() - 1)
				{
					whereExpr += tokens[ind] + " ";
					ind++;
				}
				whereExpr += tokens[ind];
			}
			else
			{
				cerr << "Invalid command!" << endl;
				cin.clear();
				continue;
			}

			size_t previousSize = 0;
			if (db.contains(tableName))
				previousSize = db.getTable(tableName).size();

			try
			{
				db.removeFrom(tableName, whereExpr);
			}
			catch (const exception& e)
			{
				cerr << e.what() << endl;
				cin.clear();
				continue;
			}

			cout << previousSize - db.getTable(tableName).size() << " rows deleted." << endl;
		}
		else if (tokens[0] == "CreateIndex")
		{
			if (tokens.size() == 4)
			{
				if (tokens[1] != "ON")
				{
					cerr << "Invalid command!" << endl;
					cin.clear();
					continue;
				}

				string tableName = tokens[2];
				string colName = tokens[3];

				try
				{
					db.createIndex(tableName, colName);
				}
				catch (const exception& e)
				{
					cerr << e.what() << endl;
					cin.clear();
					continue;
				}
				cout << "Index set on " << colName << '.' << endl;
			}
			else
				cerr << "Invalid command!" << endl;
		}
		else if (tokens[0] == "Help")
		{
			if (tokens.size() == 2)
				help();
			else
				cerr << "Invalid command!" << endl;
		}
		else if (tokens[0] == "Quit")
		{
			if (tokens.size() == 1)
			{
				std::ofstream out("database.bin", std::ios::binary);
				db.serialize(out);
				out.close();
				cout << "Goodbye" << endl;
				break;
			}
			else
				cerr << "Invalid command!" << endl;
		}
		else
		{
			cerr << "Unknown command!" << endl;
		}

		cin.clear();
	}
}

void System::help()
{
	std::cout
		<< " ------------------ Commands ------------------- - -------------------- Info ------------------------------------" << std::endl
		<< "  Help       \t\t\t\t\t - displays this message" << std::endl
		<< "  CreateTable \t\t\t\t\t - creates a table in the database" << std::endl
		<< "  DropTable \t\t\t\t\t - removes a table from the database" << std::endl
		<< "  ListTables \t\t\t\t\t - prints the tables in the database" << std::endl
		<< "  TableInfo \t\t\t\t\t - shows information about a table" << std::endl
		<< "  Select \t\t\t\t\t - prints rows of a table by given criteria" << std::endl
		<< "  Remove \t\t\t\t\t - removes rows from a table by given criteria" << std::endl
		<< "  Insert \t\t\t\t\t - insert rows into a table" << std::endl
		<< "  CreateIndex \t\t\t\t\t - creates index to a column" << std::endl
		<< "  Quit       \t\t\t\t\t - exits the program." << std::endl
		<< " ----------------------------------------------------------------------------------------------------------------" << std::endl;
}

void System::processCommand(const string& command, tokens& tokens) const
{
	if (command.empty())
		throw std::invalid_argument("Invalid input!");

	tokens.clear();

	int currInd = 0;
	bool isSmallBr = false;
	bool isBigBr = false;

	string currToken;

	for (size_t i = 0; i < command.size(); i++)
	{
		if (command[i] == '{')
			isBigBr = true;
		else if (command[i] == '(')
			isSmallBr = true;

		if (isBigBr)
		{
			while (i < command.size() && command[i] != '}')
			{
				currToken += command[i];
				i++;
			}
			if (i < command.size() && command[i] == '}')
			{
				currToken += command[i];
				tokens.push_back(currToken);
				currToken.clear();
				isBigBr = false;
			}
			else
				throw std::invalid_argument("Invalid input!");
		}
		else if (isSmallBr)
		{
			while (i < command.size() && command[i] != ')')
			{
				currToken += command[i];
				i++;
			}
			if (i < command.size() && command[i] == ')')
			{
				currToken += command[i];
				tokens.push_back(currToken);
				currToken.clear();
				isSmallBr = false;
			}
			else
				throw std::invalid_argument("Invalid input!");
		}
		else if (command[i] == ' ')
		{
			if (!currToken.empty())
			{
				tokens.push_back(currToken);
				currToken.clear();
			}
		}
		else
			currToken += command[i];
	}

	if (!currToken.empty())
		tokens.push_back(currToken);

	if (tokens.empty())
		throw std::invalid_argument("Invalid input!");
}

vector<string> System::parseRecordsToInsert(const string& toInsert) const
{
	if (toInsert[0] != '{' || toInsert[toInsert.size() - 1] != '}')
		throw std::exception("Invalid input!");

	vector<string> result;
	bool readingRecord = false;
	string currRec;

	// toInsert example: {(1, "Test", 1), (2, "something else", 100)}

	for (size_t i = 1; i < toInsert.size() - 1; i++)
	{
		if (toInsert[i] == '(')
			readingRecord = true;

		if (readingRecord)
		{
			currRec += toInsert[i];

			if (toInsert[i] == ')')
			{
				result.push_back(currRec);
				currRec.clear();
				readingRecord = false;
			}
		}
	}

	return result;
}
