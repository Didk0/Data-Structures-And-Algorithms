#pragma once
#include <algorithm>
#include <string>
#include <list>
#include <queue>

using std::string;

const int BIG_SALARY = 500;
const int SMALL_SALARY = 50;

class Tree
{
public:
	/// @brief Struct for the elements of the tree
	struct Node {
		std::string fName;
		Node* fManager;
		std::list<Node*> fEmployees;
		unsigned long fSalary;
		bool fChecked;
		size_t fLevel;

		Node(const std::string& name = "", Node* manager = nullptr, unsigned long salary = 0, bool checked = false, size_t level = 0)
			: fName(name)
			, fManager(manager)
			, fSalary(salary)
			, fChecked(checked)
			, fLevel(level)
		{}
	} *root;

public:
	Tree()
		: root(nullptr)
		, size(0)
	{}
	Tree(const Tree& other)
	{
		root = copy(other.root, nullptr);
		size = other.size;
	}
	Tree(Tree&& other) noexcept : Tree()
	{
		std::swap(root, other.root);
		std::swap(size, other.size);
	}
	Tree& operator=(const Tree& other)
	{
		if (this != &other)
		{
			clear();
			root = copy(other.root, nullptr);
			size = other.size;
		}
		return *this;
	}
	~Tree()
	{
		clear(root);
	}
	
	/// @brief Wrapper method for insert
	/// @boss - boss of who
	/// @who - the employee to insert
	void insert(const string& boss, const string& who) { insert(root, boss, who);	++size; }

	/// @brief Wrapper method for find
	/// @name - element to find
	Node* find(const string& name) const { return find(root, name); }

	/// @brief Wrapper method for remove
	/// @param name - element to remove
	/// @return true if employee is removed and false - otherwise
	bool remove(const string& name)
	{
		std::string toRemove;
		if (remove(root, name, toRemove))
		{
			size--;
			return true;
		}
		return false;
	}

	/// @brief Wrapper method for height
	int height() const { return height(root); }

	/// @brief Wrapper method for toString
	/// @return The representation of the tree
	string toString() const
	{
		string result;
		toString(root, result);
		return result;
	}

	/// @brief Gives the size of the tree
	/// @return size of tree 
	size_t getSize() const { return size; }

	/// @brief Wrapper method for clear
	void clear() { clear(root); }

	/// @brief Wrapper method for overloaded
	/// @param level 
	/// @return Count of overloaded employees
	int overloaded(int level = 20) const { return overloaded(root, level); }

	/// @brief Wrapper method for salary
	/// @param who - employee to get salary of
	/// @return Salary of who
	unsigned long salary(const std::string& who) const
	{
		const Node* search = find(who);
		if (!search)
			return -1;
		return salary(search);
	}

	/// @brief Wrapper method for incorporate
	void incorporate()
	{
		setSalaries(root);
		incorporate(root);
		unsetChecked(root);
	}

	/// @brief Wrapper method for modernize
	void modernize()
	{
		setLevels(root, 0);
		modernize(root);
	}

	/// @brief Wrapper method for join
	Tree join(const Tree& other) const { return join(other, root); }

private:
	/// @brief Deletes root and all elements below root
	/// @param root - root from which the clearing starts
	void clear(Node*& root)
	{
		if (root)
		{
			for (Node*& employee : root->fEmployees)
				clear(employee);

			delete root;
			root = nullptr;
			--size;
		}
	}

	/// @brief Copies the other root
	/// @param other - root to copy
	/// @param manager - the manager of root
	/// @return the coppied root
	Node* copy(Node* other, Node* manager)
	{
		if (other)
		{
			Node* newNode = new Node(other->fName, manager, other->fSalary, other->fChecked, other->fLevel);

			for (Node* employee : other->fEmployees)
			{
				newNode->fEmployees.push_back(copy(employee, newNode));
			}

			return newNode;
		}
		else
		{
			return nullptr;
		}
	}

	/// @brief Inserts who with manager boss. If empty tree inserts boss and who
	/// If who already exists, inserts it in its new place and deletes the old one
	/// @param root - root of tree
	/// @param boss - boss of who
	/// @param who - element to insert
	void insert(Node*& root, const string& boss, const string& who)
	{
		if (!root)
		{
			root = new Node(boss);
			++size;
			Node* emp = new Node(who, root, 0, false, root->fLevel + 1);
			root->fEmployees.push_back(emp);
		}
		else
		{
			if (root->fName == boss)
			{
				Node* toHier = find(who);

				if (toHier)
				{
					toHier->fManager->fEmployees.remove(toHier);
					toHier->fManager = root;
					root->fEmployees.push_back(toHier);
					--size;
				}
				else
				{
					Node* emp = new Node(who, root, 0, false, root->fLevel + 1);
					root->fEmployees.push_back(emp);
				}
			}
			else
			{
				for (Node* employee : root->fEmployees)
				{
					insert(employee, boss, who);
				}
			}
		}
	}

	/// @brief Searches node with name in the tree
	/// @param root - root of tree
	/// @param name - element to find
	/// @return Node with name or nullptr if not found.
	Node* find(Node* root, const string& name) const
	{
		if (!root)
		{
			return nullptr;
		}

		if (root->fName == name)
		{
			return root;
		}
		else
		{
			for (Node* employee : root->fEmployees)
			{
				Node* search = find(employee, name);
				if (search)
					return search;
			}
		}

		return nullptr;
	}

	/// @brief Removes an element with name from the tree
	/// @param root - root of tree
	/// @param name - element to remove
	/// @param toRemove - string to save the name of the removed element
	/// @return true if an element is removed and false - othwerwise
	bool remove(Node*& root, const string& name, string& toRemove)
	{
		for (Node*& employee : root->fEmployees)
		{
			if (remove(employee, name, toRemove))
			{
				if (employee->fName == toRemove)
					root->fEmployees.remove(employee);
				return true;
			}
		}

		if (root->fName == name)
		{
			if (!root->fEmployees.empty())
			{
				for (Node*& employee : root->fEmployees)
				{
					root->fManager->fEmployees.push_back(employee);
					employee->fManager = root->fManager;
				}
			}

			toRemove = root->fName;
			return true;
		}

		return false;
	}
	
	/// @brief Gives the height of the tree
	/// @param root - root of tree
	/// @return Height of the tree
	int height(const Node* root) const
	{
		if (!root)
		{
			return 0;
		}

		int maxHeight = 0;
		for (Node* employee : root->fEmployees)
		{
			int currHeight = height(employee);
			if (currHeight > maxHeight)
				maxHeight = currHeight;
		}

		return 1 + maxHeight;
	}

	/// @brief Makes the tree to given string format
	/// @param root - root of tree
	void toString(const Node* root, std::string& result) const
	{
		if (!root)
			return;

		std::queue<const Node*> wave;
		wave.push(root);

		while (!wave.empty())
		{
			std::vector<const Node*> sorted;
			const Node* curr = wave.front();
			wave.pop();

			for (const Node* employee : curr->fEmployees)
				sorted.push_back(employee);

			if (sorted.size() > 0)
				sortNames(sorted, result);

			for (const Node* employee : sorted)
				wave.push(employee);
		}
	}

	/// @brief Gives the size of a tree with given root
	/// @param root - root of tree
	size_t sizeSubTree(const Node* root) const
	{
		if (!root)
			return 0;

		size_t result = 0;

		for (const Node* employee : root->fEmployees)
			result += sizeSubTree(employee);

		return 1 + result;
	}

	/// @brief Gives the count of employees with more than level subordinates
	/// @param root - root of tree
	/// @param level - overload factor
	/// @return Count of the overloaded employees
	int overloaded(const Node* root, int level = 20) const
	{
		if (!root)
			return 0;

		size_t result = sizeSubTree(root) - 1 > level ? 1 : 0;

		for (const Node* employee : root->fEmployees)
			result += overloaded(employee, level);

		return result;
	}

	/// @brief Gives the salary of an employee
	/// @param search - element to find the salary of
	/// @return Salary of search
	unsigned long salary(const Node* search) const
	{
		return search->fEmployees.size() * BIG_SALARY + (sizeSubTree(search) - 1 - search->fEmployees.size()) * SMALL_SALARY;
	}

	/// @brief Incorporates the tree
	void incorporate(Node*& root)
	{
		if (!root)
			throw std::exception("Invalid argument!");

		for (Node*& employee : root->fEmployees)
		{
			incorporate(employee);
		}

		if (root->fManager && root->fManager->fEmployees.size() > 1 && toBePromoted(root, root->fManager->fEmployees))
		{
			std::list<Node*>::iterator it = root->fManager->fEmployees.begin();

			while (it != root->fManager->fEmployees.end())
			{
				if (*it != root)
				{
					root->fEmployees.push_back(*it);
					(*it)->fManager = root;
					it = root->fManager->fEmployees.erase(it);
					continue;
				}
				++it;
			}

			root->fChecked = true;

			for (Node*& employee : root->fEmployees)
			{
				if (!employee->fChecked)
					incorporate(employee);
			}
		}
		root->fChecked = true;
	}

	/// @brief Checks if a given employee must be promoted
	/// @param employee - employee to check
	/// @param team - team of the employee
	/// @return true if employee will be promoted and false - otherwise
	bool toBePromoted(Node*& employee, std::list<Node*>& team)
	{
		unsigned long maxSalary = team.front()->fSalary;
		Node* toBePromoted = employee;

		if (employee->fManager->fChecked)
		{
			return false;
		}

		for (Node* emp : team)
			if (emp->fSalary > maxSalary
				|| (emp->fSalary == maxSalary && emp->fName < toBePromoted->fName))
				toBePromoted = emp;

		return toBePromoted == employee;
	}

	/// @brief Sets the salaries of all employees below root
	/// @param root - root of tree
	void setSalaries(Node*& root)
	{
		if (!root)
			throw std::exception("Invalid input!");

		for (Node*& employee : root->fEmployees)
			setSalaries(employee);

		root->fSalary = salary(root->fName);
	}

	/// @brief Sets the bool in every Node to false
	/// @param root - root of tree
	void unsetChecked(Node*& root)
	{
		if (!root)
			throw std::exception("Invalid input!");

		for (Node*& employee : root->fEmployees)
			unsetChecked(employee);

		root->fChecked = false;
	}

	/// @brief Sort the names of employees lexicographically
	/// @param vec - vector with employees
	/// @param result - string to concatenate
	void sortNames(std::vector<const Node*>& vec, std::string& result) const
	{
		std::sort(vec.begin(), vec.end(), comp);

		for (const Node* elem : vec)
			result += elem->fManager->fName + "-" + elem->fName + "\n";
	}

	/// @brief Sets the levels of all nodes
	/// @param root - root of tree
	/// @param level - level of each node
	void setLevels(Node*& root, size_t level)
	{
		if (!root)
			throw std::exception("Invalid input!");

		for (Node*& employee : root->fEmployees)
			setLevels(employee, level + 1);

		root->fLevel = level;
	}

	/// @brief Modernizes the tree
	/// @param root - root of tree
	void modernize(Node*& root)
	{
		if (!root)
			throw std::exception("Invalid input!");

		std::list<Node*>::iterator curr = root->fEmployees.begin();

		while (curr != root->fEmployees.end())
		{
			modernize(*curr);
			if (!*curr)
				curr = root->fEmployees.erase(curr);
			else
				++curr;
		}

		if (root->fManager && root->fLevel % 2 == 1 && root->fEmployees.size() > 0)
		{
			std::list<Node*>::iterator curr = root->fEmployees.begin();
			while (curr != root->fEmployees.end())
			{
				(*curr)->fManager = root->fManager;
				root->fManager->fEmployees.push_front(*curr);
				curr = root->fEmployees.erase(curr);
			}

			delete root;
			root = nullptr;
			--size;
		}
	}

	/// @brief Searches all predecessors in rootOfSubTree
	/// @param predecessor - element to search
	/// @param rootOfSubTree - all elements below rootOfSubTree and rootOfSubTree
	/// @return true if predecessor is found in rootOfSubTree
	bool searchInPredecessors(const Node* predecessor, Node* rootOfSubTree) const
	{
		if (!predecessor)
			return false;

		if (find(rootOfSubTree, predecessor->fName))
			return true;
		else
		{
			if (searchInPredecessors(predecessor->fManager, rootOfSubTree))
				return true;
		}

		return false;
	}

	/// @brief Unites two trees
	/// @param other - tree to join
	/// @param root - root of tree
	/// @return The result tree
	Tree join(const Tree& other, const Node* root) const
	{
		if (!root || !other.root)
			return Tree();

		Tree result;

		std::queue<const Node*> wave;
		wave.push(root);
		wave.push(other.root);

		while (!wave.empty())
		{
			const Node* curr = wave.front();
			wave.pop();

			if (curr->fEmployees.empty())
			{
				if (curr->fManager && !result.find(curr->fName))
					result.insert(curr->fManager->fName, curr->fName);
			}
			else
			{
				for (Node* employee : curr->fEmployees)
				{
					if (result.find(employee->fName))
						continue;

					Node* left = find(employee->fName);
					Node* right = other.find(employee->fName);

					std::string boss;
					if (left && right && left->fManager->fName != right->fManager->fName)
					{
						if (left && right)
						{
							if (left->fManager->fLevel < right->fManager->fLevel)
								boss = left->fManager->fName;
							else if (left->fManager->fLevel > right->fManager->fLevel)
								boss = right->fManager->fName;
							else
							{
								if (left->fManager->fName < right->fManager->fName)
									boss = left->fManager->fName;
								else
									boss = right->fManager->fName;
							}

							if (searchInPredecessors(right->fManager, left))
								return Tree();

							result.insert(boss, employee->fName);
						}
					}
					else if (left && right && left->fManager->fName == right->fManager->fName)
					{
						boss = left->fManager->fName;
						result.insert(boss, employee->fName);
					}
					else if (left && !right)
						result.insert(left->fManager->fName, employee->fName);
					else if (!left && right)
						result.insert(right->fManager->fName, employee->fName);
				}
			}

			for (Node* employee : curr->fEmployees)
				wave.push(employee);
		}

		return result;
	}

private:
	/// @brief Comparator for sorting lexicographically
	struct Comparator
	{
		bool operator()(const Node*& first, const Node*& second)
		{
			return first->fManager->fName < second->fManager->fName || (first->fManager->fName == second->fManager->fName && first->fName < second->fName);
		}
	} comp;

private:
	size_t size;
};