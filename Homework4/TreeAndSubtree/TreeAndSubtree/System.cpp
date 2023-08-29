#include "System.h"

using std::cout;
using std::endl;
using std::getline;
using std::cin;
using std::cerr;

System& System::i()
{
	static System sys;
	return sys;
}

void System::run()
{
	printHelp();

	tokens tokens;
	string command;

	while (true)
	{
		cout << '>';
		getline(cin, command);

		try
		{
			processCommand(command, tokens);
		}
		catch (const std::invalid_argument& e)
		{
			cerr << e.what() << endl;
			continue;
		}

		if (tokens[0] == "LOAD")
		{
			if (tokens.size() == 3)
			{
				Tree tree;
				try
				{
					tree.load(tokens[2]);
				}
				catch (const std::exception& e)
				{
					cerr << e.what() << endl;
					cin.clear();
					continue;
				}
				trees[tokens[1]] = tree;
				cout << tokens[1] << " loaded successfully" << endl;
			}
			else
				cerr << "Unknown command!" << endl;
		}
		else if (tokens[0] == "SAVE")
		{
			if (tokens.size() == 3)
			{
				if (trees.find(tokens[1]) != trees.end())
				{
					std::ifstream file;
					file.open(tokens[2]);
					if (file)
					{
						cout << "File with name: " << tokens[2] << " exists. Do you want to overwrite its content? (y/n): ";
						char answer;
						cin >> answer;
						cin.ignore();
						if (answer == 'y')
						{
							file.close();
							try
							{
								trees[tokens[1]].save(tokens[2]);
							}
							catch (const std::exception& e)
							{
								cerr << e.what() << endl;
								cin.clear();
								continue;
							}
							cout << tokens[1] << " saved successfully." << endl;
						}
						else if (answer == 'n')
						{
							cout << tokens[1] << " was not saved." << endl;
							file.close();
						}
						else
						{
							cerr << "Unknown answer!" << endl;
							file.close();
						}
					}
					else
					{
						file.close();
						try
						{
							trees[tokens[1]].save(tokens[2]);
						}
						catch (const std::exception& e)
						{
							cerr << e.what() << endl;
							cin.clear();
							continue;
						}
						cout << tokens[1] << " saved successfully." << endl;
					}
				}
				else
					cerr << "Tree with name " << tokens[1] << " not found" << endl;
			}
			else
				cerr << "Unknown command!" << endl;
		}
		else if (tokens[0] == "CONTAINS")
		{
			if (tokens.size() == 3)
			{
				if (trees.find(tokens[1]) != trees.end() && trees.find(tokens[2]) != trees.end())
				{
					try
					{
						if (trees[tokens[1]].containsSubtree(trees[tokens[2]]))
							cout << tokens[2] << " is a subtree of " << tokens[1] << endl;
						else
							cout << tokens[2] << " is not a subtree of " << tokens[1] << endl;
					}
					catch (const std::exception& e)
					{
						cerr << e.what() << endl;
						cin.clear();
						continue;
					}
				}
				else
					cerr << "One or both of the trees not found" << endl;
			}
			else
				cerr << "Unknown command!" << endl;
		}
		else if (tokens[0] == "REMOVE")
		{
			if (tokens.size() == 3)
			{
				if (trees.find(tokens[1]) != trees.end() && trees.find(tokens[2]) != trees.end())
				{
					Tree subTree = trees[tokens[2]];

					try
					{
						if (trees[tokens[1]].containsSubtree(subTree))
						{
							trees[tokens[1]].deleteSubtree(subTree);
						}
						else
						{
							cerr << tokens[2] << " is not a subtree of " << tokens[1] << endl;
							cin.clear();
							continue;
						}
					}
					catch (const std::exception& e)
					{
						cerr << e.what() << endl;
						cin.clear();
						continue;
					}
					cout << tokens[2] << " deleted from " << tokens[1] << " successfully" << endl;
					cin.clear();
				}
				else
					cerr << "One or both of the trees not found" << endl;
			}
			else
				cerr << "Unknown command!" << endl;
		}
		else if (tokens[0] == "HELP")
		{
			printHelp();
		}
		else if (tokens[0] == "EXIT")
		{
			cout << "Bye!" << endl;
			break;
		}
		else
		{
			cerr << "Unknown command!" << endl;
		}

		cin.clear();
	}
}

void System::printHelp()
{
	std::cout
		<< " ------------------ Commands ------------------- - -------------------- Info ------------------------------------" << std::endl
		<< "  HELP       \t\t\t\t\t - displays this message" << std::endl
		<< "  LOAD <name> <file> \t\t\t\t - reads a tree from <file> and gives it <name>" << std::endl
		<< "  SAVE <name> <file> \t\t\t\t - saves a tree with <name> to <file>" << std::endl
		<< "  CONTAINS <tree1> <tree2>\t\t\t - checks if <tree2> is a somewhere in <tree1>" << std::endl
		<< "  REMOVE <tree1> <tree2>\t\t\t - removes <tree2> from <tree1> if possible" << std::endl
		<< "  EXIT       \t\t\t\t\t - exits the program." << std::endl
		<< " ----------------------------------------------------------------------------------------------------------------" << std::endl;
}

void System::processCommand(const string& command, tokens& tokens)
{
	tokens.clear();

	string curr;
	size_t count = 0;
	for (size_t i = 0; i < command.size(); i++)
	{
		if (command[i] != ' ')
			curr += command[i];

		if (command[i] == ' ')
		{
			if (count < 2)
			{
				tokens.push_back(curr);
				count++;
				curr.clear();
			}
			else
				curr += command[i];
		}
	}
	if (!curr.empty())
		tokens.push_back(curr);

	if (tokens.size() != 3 && tokens.size() != 1)
		throw std::invalid_argument("Invalid command!");
}
