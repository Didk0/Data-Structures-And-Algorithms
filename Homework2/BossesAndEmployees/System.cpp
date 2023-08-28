#include "System.h"

using std::cout;
using std::endl;
using std::getline;
using std::cin;
using std::cerr;

System::~System()
{
	for (size_t i = 0; i < fVector.size(); i++)
		delete fVector[i];
}

System& System::i()
{
	static System sys;
	return sys;
}

System::System()
{}

void System::processCommand(const string& cmd, tokens& tokens)
{
	tokens.clear();
	int currInd = 0;

	for (size_t i = 0; i < cmd.size(); i++)
	{
		while (cmd[i] == ' ')
		{
			i++;
			currInd++;
		}

		while (cmd[i] != ' ' && i < cmd.size())
			i++;

		tokens.push_back(cmd.substr(currInd, i - currInd));
		currInd = i + 1;
	}

	if (tokens.size() > 4)
		throw std::invalid_argument("Invalid command!");
}

std::vector<string> System::splitByDelim(string str, char delimeter)
{
	{
		std::vector<std::string> tokens;

		size_t startInd = 0;

		for (size_t i = 0; i < str.size(); i++)
		{
			while (str[i] == ' ' && i < str.size())
				i++;

			startInd = i;

			while (str[i] != delimeter && i < str.size())
				i++;

			std::string currStr = str.substr(startInd, i - startInd);

			while (currStr[currStr.size() - 1] == ' ')
				currStr.pop_back();

			tokens.push_back(currStr);
		}

		return tokens;
	}
}

void System::checkIfInvalid(const string& hierName)
{
	for (size_t i = 0; i < hierName.size(); i++)
	{
		if (!isValid(hierName[i]))
			throw std::invalid_argument("Invalid hierarchy name!");
	}
}

bool System::isValid(char ch)
{
	return ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z' || ch >= '1' && ch <= '9' || ch == '_';
}

Hierarchy* System::findHierarchy(const string& hierName)
{
	for (size_t i = 0; i < fVector.size(); i++)
	{
		if (fVector[i]->getName() == hierName)
			return fVector[i];
	}

	return nullptr;
}

void System::readFromFile(const string& file, const string& hierName)
{
	std::ifstream in(file);
	if (!in)
	{
		throw std::invalid_argument("Couldn't open file for reading!");
	}

	string representation;

	while (!in.eof())
	{
		string currPair;
		getline(in, currPair);
		representation.append(currPair);
		representation.append("\n");
	}

	if (representation.size() >= 2 && representation[representation.size() - 1] == '\n' && representation[representation.size() - 2] == '\n')
		representation.pop_back();

	Hierarchy* h = new Hierarchy(representation);
	h->setName(hierName);
	fVector.push_back(h);

	in.close();
}

void System::writeToFile(const string& file, const Hierarchy* hier)
{
	std::ofstream out(file);
	if (!out)
	{
		throw std::invalid_argument("Couldn't open file for reading!");
	}

	out << hier->print();

	out.close();
}

void System::run()
{
	tokens tokens;
	string command;

	while (true)
	{
		cout << '>';
		getline(cin, command);
		if (command == "")
			continue;

		try
		{
			processCommand(command, tokens);
		}
		catch (const std::invalid_argument& e)
		{
			cerr << e.what() << endl;
			continue;
		}

		if (tokens[0] == "load")
		{
			if (tokens.size() == 2)
			{
				string representation;
				string currPair;
				while (getline(cin, currPair))
				{
					representation.append(currPair);
					representation.append("\n");
				}
				try
				{
					if (representation.empty())
						representation.push_back('\n');

					checkIfInvalid(tokens[1]);
					Hierarchy* h = new Hierarchy(representation);
					h->setName(tokens[1]);
					fVector.push_back(h);
				}
				catch (const std::exception& e)
				{
					cerr << e.what() << endl;
					cin.clear();
					continue;
				}
				cout << tokens[1] << " loaded successfully!" << endl;
				cin.clear();
			}
			else if (tokens.size() == 3)
			{
				try
				{
					checkIfInvalid(tokens[1]);
					readFromFile(tokens[2], tokens[1]);
				}
				catch (const std::exception& e)
				{
					cerr << e.what() << endl;
					continue;
				}
				cout << tokens[1] << " loaded successfully!" << endl;
			}
			else
				cerr << "Unknown command!" << endl;
		}
		else if (tokens[0] == "save")
		{
			if (tokens.size() == 2)
			{
				Hierarchy* h = findHierarchy(tokens[1]);
				if (!h)
				{
					cout << tokens[1] << " is an unknown office!" << endl;
					continue;
				}

				cout << findHierarchy(tokens[1])->print();
			}
			else if (tokens.size() == 3)
			{
				Hierarchy* h = findHierarchy(tokens[1]);
				if (!h)
				{
					cout << tokens[1] << " is an unknown office!" << endl;
					continue;
				}
				h->setSaved(true);
				writeToFile(tokens[2], h);
				cout << tokens[1] << " saved." << endl;
			}
			else
				cerr << "Unknown command!" << endl;
		}
		else if (tokens[0] == "find")
		{
			if (tokens.size() != 3)
			{
				cerr << "Unknown command!" << endl;
				continue;
			}

			Hierarchy* h = findHierarchy(tokens[1]);

			if (!h)
				cout << tokens[1] << " is an unknown office!" << endl;
			else
			{
				if (h->find(tokens[2]))
					cout << tokens[2] << " is employed in " << tokens[1] << "." << endl;
				else
					cout << tokens[2] << " is not employed in " << tokens[1] << "." << endl;
			}
		}
		else if (tokens[0] == "num_subordinates")
		{
			if (tokens.size() != 3)
			{
				cerr << "Unknown command!" << endl;
				continue;
			}

			Hierarchy* h = findHierarchy(tokens[1]);

			if (!h)
				cout << tokens[1] << " is an unknown office!" << endl;
			else
			{
				if (h->find(tokens[2]))
				{
					cout << tokens[2] << " has ";
					int numSubords = h->num_subordinates(tokens[2]);
					if (numSubords > 0)
						cout << numSubords;
					else
						cout << " no";

					cout << " subordinates." << endl;
				}
				else
					cout << "There is no " << tokens[2] << " in " << tokens[1] << endl;
			}

		}
		else if (tokens[0] == "manager")
		{
			if (tokens.size() != 3)
			{
				cerr << "Unknown command!" << endl;
				continue;
			}

			Hierarchy* h = findHierarchy(tokens[1]);

			if (!h)
				cout << tokens[1] << " is an unknown office!" << endl;
			else
			{
				if (h->find(tokens[2]))
					cout << "The manager of " << tokens[2] << " is " << h->manager(tokens[2]) << "." << endl;
				else
					cout << "There is no " << tokens[2] << " in " << tokens[1] << "." << endl;
			}
		}
		else if (tokens[0] == "num_employees")
		{
			if (tokens.size() != 2)
			{
				cerr << "Unknown command!" << endl;
				continue;
			}

			Hierarchy* h = findHierarchy(tokens[1]);

			if (!h)
				cout << tokens[1] << " is an unknown office!" << endl;
			else
				cout << "There are " << findHierarchy(tokens[1])->num_employees() << " employees in " << tokens[1] << "." << endl;
		}
		else if (tokens[0] == "overloaded")
		{
			if (tokens.size() != 2)
			{
				cerr << "Unknown command!" << endl;
				continue;
			}

			Hierarchy* h = findHierarchy(tokens[1]);

			if (!h)
				cout << tokens[1] << " is an unknown office!" << endl;
			else
			{
				int numOverl = h->num_overloaded();

				if (numOverl > 0)
					cout << "There are " << numOverl << " overloaded employees in " << tokens[1] << "." << endl;
				else
					cout << "No overloaded employees in " << tokens[1] << "." << endl;
			}
		}
		else if (tokens[0] == "join")
		{
			if (tokens.size() != 4)
			{
				cerr << "Unknown command!" << endl;
				continue;
			}

			Hierarchy* h1 = findHierarchy(tokens[1]);
			Hierarchy* h2 = findHierarchy(tokens[2]);

			if (!h1)
			{
				cout << tokens[1] << " is an unknown office!" << endl;
			}
			else if (!h2)
			{
				cout << tokens[2] << " is an unknown office!" << endl;
			}
			else
			{
				string newName = tokens[3];
				Hierarchy* newHier = new Hierarchy(h1->join(*h2));
				if (newHier->print() == "")
				{
					cerr << "Couldn't join " << tokens[1] << " and " << tokens[2] << "!" << endl;
					delete newHier;
					continue;
				}
				newHier->setName(tokens[3]);
				fVector.push_back(newHier);
				cout << newName << " created." << endl;
			}
		}
		else if (tokens[0] == "fire")
		{
			if (tokens.size() != 3)
			{
				cerr << "Unknown command!" << endl;
				continue;
			}

			Hierarchy* h = findHierarchy(tokens[1]);
			if (!h)
				cout << tokens[1] << " is an unknown office!" << endl;
			else
			{
				bool fired = h->fire(tokens[2]);
				if (!fired)
				{
					cerr << tokens[2] << " cannot be fired!" << endl;
					continue;
				}
				cout << tokens[2] << " was fired." << endl;
			}
		}
		else if (tokens[0] == "hire")
		{
			if (tokens.size() != 4)
			{
				cerr << "Unknown command!" << endl;
				continue;
			}

			Hierarchy* h = findHierarchy(tokens[1]);
			if (!h)
				cout << tokens[1] << " is an unknown office!" << endl;
			else
			{
				h->hire(tokens[2], tokens[3]);
				cout << tokens[2] << " was hired." << endl;
			}
		}
		else if (tokens[0] == "salary")
		{
			if (tokens.size() != 3)
			{
				cerr << "Unknown command!" << endl;
				continue;
			}

			Hierarchy* h = findHierarchy(tokens[1]);
			if (!h)
				cout << tokens[1] << " is an unknown office!" << endl;
			else
				cout << "The salary is " << h->getSalary(tokens[2]) << " BGN." << endl;
		}
		else if (tokens[0] == "incorporate")
		{
			if (tokens.size() != 2)
			{
				cerr << "Unknown command!" << endl;
				continue;
			}

			Hierarchy* h = findHierarchy(tokens[1]);
			if (!h)
				cout << tokens[1] << " is an unknown office!" << endl;
			else
			{
				h->incorporate();
				cout << tokens[1] << " was incorporated." << endl;
			}
		}
		else if (tokens[0] == "modernize")
		{
			if (tokens.size() != 2)
			{
				cerr << "Unknown command!" << endl;
				continue;
			}

			Hierarchy* h = findHierarchy(tokens[1]);
			if (!h)
				cout << tokens[1] << " is an unknown office!" << endl;
			else
			{
				h->modernize();
				cout << tokens[1] << " was modernized." << endl;
			}
		}
		else if (tokens[0] == "help")
		{
			if (tokens.size() != 1)
			{
				cerr << "Unknown command!" << endl;
				continue;
			}

			printHelp();
		}
		else if (tokens[0] == "exit")
		{
			if (tokens.size() != 1)
			{
				cerr << "Unknown command!" << endl;
				continue;
			}

			for (size_t i = 0; i < fVector.size(); i++)
			{
				if (!fVector[i]->getSaved())
				{
					try
					{
						cout << fVector[i]->getName() << " is modified, but not saved." << endl;
						cout << "Enter file name to save it:" << endl;
						string file;
						cout << ">";
						cin >> file;
						writeToFile(file, fVector[i]);
						fVector[i]->setSaved(true);
						cout << fVector[i]->getName() << " saved." << endl;
					}
					catch (const std::exception& e)
					{
						cout << e.what() << endl;
						continue;
					}
				}
			}

			cout << "Goodbye!" << endl;
			break;
		}
		else
		{
			cout << "Unknown command!" << endl;
		}
	}
}

void System::printHelp()
{
	std::cout
		<< " ------------------ Commands ------------------- - -------------------- Info ------------------------------------" << std::endl
		<< "  help       \t\t\t\t\t - displays this message" << std::endl
		<< "  load <object_name> <file_path> (optional) \t - loads data for object_name from file_path or from the console" << std::endl
		<< "  save <object_name> <file_path> (optional) \t - saves data for object_name to file_path or prints it on the console" << std::endl
		<< "  find <object_name> <employee_name>\t\t - searches employee_name in object_name" << std::endl
		<< "  num_subordinates <object_name> <employee_name> - displays the count of the direct subordinates of employee_name" << std::endl
		<< "  manager <object_name> <employee_name>\t\t - displays the manager of employee_name" << std::endl
		<< "  num_employees <object_name>\t\t\t - displays the count of employees in object_name" << std::endl
		<< "  overloaded <object_name>\t\t\t - displays the count of employees with more than 20 subordinates" << std::endl
		<< "  join <object_name_1> <object_name_2> <result_object_name> - unites object_name_1 and object_name_2 and creates result_object_name" << std::endl
		<< "  fire <object_name> <employee_name>\t\t - fires employee_name from object_name" << std::endl
		<< "  hire <object_name> <employee_name>\t\t - hires employee_name to object_name" << std::endl
		<< "  salary <object_name> <employee_name>\t\t - displays the salary of employee_name in object_name" << std::endl
		<< "  incorporate <object_name>\t\t\t - incorporates object_name" << std::endl
		<< "  modernize <object_name>\t\t\t - modernizes object_name" << std::endl
		<< "  exit       \t\t\t\t\t - exits the program." << std::endl
		<< " ----------------------------------------------------------------------------------------------------------------" << std::endl;
}
