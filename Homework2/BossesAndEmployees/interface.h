#pragma once
#include <string>
#include <vector>
#include "Tree.h"

using std::string;

const string THE_BOSS = "Uspeshnia";

class Hierarchy
{
public:
	Hierarchy(Hierarchy&& r) noexcept
	{
		fTree = std::move(r.fTree);
		fRepresentation = std::move(r.fRepresentation);
		fName = std::move(r.fName);
		fSaved = std::move(r.fSaved);
	}
	Hierarchy(const Hierarchy& r) {
		fTree = r.fTree;
		fRepresentation = r.fRepresentation;
		fName = r.fName;
		fSaved = r.fSaved;
	}
	Hierarchy(const string& data)
	{
		std::vector<string> tokens = splitByDelim(data, '\n');

		for (size_t i = 0; i < tokens.size(); i++)
		{
			std::vector<string> bossEmpPair = splitByDelim(tokens[i], '-');

			if (bossEmpPair.size() != 2)
				throw std::exception("Invalid input!");
			else if (i == 0 && bossEmpPair[0] != THE_BOSS)
				throw std::exception("Invalid input!");
			else if (i > 0 && !fTree.find(bossEmpPair[0]))
				throw std::exception("Invalid input!");
			else if (bossEmpPair[1] == THE_BOSS)
				throw std::exception("Invalid input!");

			fTree.insert(bossEmpPair[0], bossEmpPair[1]);
		}
		fRepresentation = fTree.toString();
	}
	~Hierarchy() noexcept = default;
	void operator=(const Hierarchy&) = delete;

	string print()const { return fRepresentation; }

	int longest_chain() const { return fTree.height(); }
	bool find(const string& name) const { return fTree.find(name) ? true : false; }
	int num_employees() const { return fTree.getSize(); }
	int num_overloaded(int level = 20) const
	{
		return fTree.overloaded(level);
	}

	string manager(const string& name) const
	{
		if (name == THE_BOSS)
			return "";

		Tree::Node* found = fTree.find(name);

		if (!found)
			throw std::exception("Invalid input!");

		return found->fManager->fName;
	}
	int num_subordinates(const string& name) const
	{
		Tree::Node* found = fTree.find(name);

		if (!found)
			return -1;

		return found->fEmployees.size();
	}
	unsigned long getSalary(const string& who) const { return fTree.salary(who); }

	bool fire(const string& who)
	{
		if (who == THE_BOSS)
			return false;

		bool removed = fTree.remove(who);
		fRepresentation = fTree.toString();
		return removed;
	}
	bool hire(const string& who, const string& boss)
	{
		if (find(boss))
		{
			fTree.insert(boss, who);
			fRepresentation = fTree.toString();
			return true;
		}
		return false;
	}

	void incorporate() { fTree.incorporate(); fRepresentation = fTree.toString(); }
	void modernize() { fTree.modernize(); fRepresentation = fTree.toString(); }

	Hierarchy join(const Hierarchy& right) const
	{
		return Hierarchy(fTree.join(right.fTree).toString());
	}

	void setName(const string& name) { fName = name; }

	string getName() const { return fName; }

	void setSaved(bool isSaved) { fSaved = isSaved; }

	bool getSaved() const { return fSaved; }

private:
	std::vector<string> splitByDelim(string str, char delimeter)
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

			if (currStr == "")
				throw std::exception("Invalid input!");

			while (currStr[currStr.size() - 1] == ' ')
				currStr.pop_back();

			tokens.push_back(currStr);
		}

		return tokens;
	}

private:
	Tree fTree;
	string fRepresentation;
	string fName;
	bool fSaved = false;
};