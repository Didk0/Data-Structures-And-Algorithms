#include "Tree.h"

Tree::Tree(const std::string& file)
{
	root = init(file);
}

Tree::Tree(const Tree& other)
{
	root = copy(other.root);
	size = other.size;
}

Tree& Tree::operator=(const Tree& other)
{
	if (this != &other)
	{
		clear(root);
		root = copy(other.root);
		size = other.size;
	}
	return *this;
}

Tree::~Tree()
{
	clear(root);
}

void Tree::save(const std::string& file) const
{
	std::ofstream out(file);
	if (!out)
		throw std::exception("Couldn't open file for writing");

	if (root)
	{
		std::vector<const Node*> currRow;
		std::vector<const Node*> nextRow;

		currRow.push_back(root);
		currRow.push_back(nullptr);

		while (!currRow.empty())
		{
			out << "| ";
			for (size_t i = 0; i < currRow.size(); i++)
			{
				if (currRow[i])
				{
					out << currRow[i]->data << ' ';
					if (!currRow[i]->children.empty())
					{
						for (const Node* child : currRow[i]->children)
							nextRow.push_back(child);
						nextRow.push_back(nullptr);
					}
					else
						nextRow.push_back(nullptr);
				}
				else
				{
					out << '|';
					if (i != currRow.size() - 1) out << ' ';
				}
			}
			currRow = nextRow;
			nextRow.clear();

			if (currRow.empty()) break;
			out << std::endl;
		}
	}
	else
		out.clear();

	out.close();
}

void Tree::deleteSubtree(const Tree& other)
{
	if (!other.root) return;

	while (true)
	{
		int sum = 0;
		Node* toDelete = findSubtree(other, sum);
		if (!toDelete) return;

		if (isEqual(other))
		{
			clear(root);
			return;
		}

		Node* parent = getParent(root, toDelete);

		if (!parent)
			throw std::invalid_argument("Couldn't remove this subtree!");

		std::list<Node*>::iterator toRem = parent->children.begin();
		for (std::list<Node*>::iterator it = parent->children.begin(); it != parent->children.end(); ++it)
		{
			if (*it == toDelete)
			{
				toRem = it;
				++it;

				bool found = false;
				for (const Node* child : parent->children)
				{
					if (child->data == sum)
					{
						found = true;
						break;
					}
				}
				if (!found)
				{
					parent->children.insert(it, new Node(sum));
					size++;
				}

				clear(*toRem);
				parent->children.erase(toRem);
			}
			if (it == parent->children.end()) break;
		}
		toDelete = nullptr;
	}
}

void Tree::clear(Node*& root)
{
	if (root)
	{
		for (Node*& child : root->children)
			clear(child);
		delete root;
		root = nullptr;
		size--;
	}
}

Node* Tree::copy(Node* other)
{
	if (other)
	{
		Node* newNode = new Node(other->data);
		for (Node* child : other->children)
			newNode->children.push_back(copy(child));

		return newNode;
	}
	else
		return nullptr;
}

Node* Tree::init(const std::string& file)
{
	std::ifstream in(file);
	if (!in)
		throw std::exception("Couldn't open file for reading!");

	if (in.peek() == std::ifstream::traits_type::eof()) return nullptr;

	char ch;
	int rootVal;
	in >> ch >> rootVal;
	in >> ch;
	in.ignore();

	root = new Node(rootVal);
	if (!root)
		return nullptr;
	size++;

	std::queue<Node*> wave;
	wave.push(root);

	while (!wave.empty())
	{
		Node* curr = wave.front();
		wave.pop();

		std::string row;
		getline(in, row);

		if (row.empty())
		{
			clear(root);
			throw std::invalid_argument("Invalid input given!");
		}

		std::vector<std::string> rowSymbols = parseRow(row);

		for (size_t i = 0; i < rowSymbols.size(); i++)
		{
			std::string elem = rowSymbols[i];

			if (elem == "|")
			{
				curr = wave.front();
				wave.pop();
			}
			else
			{
				Node* child = new Node(std::stoi(elem));
				if (!child)
				{
					clear(root);
					return nullptr;
				}
				if (curr)
				{
					for (const Node* ch : curr->children)
					{
						if (ch->data == child->data)
						{
							clear(root);
							in.close();
							throw std::invalid_argument("Trees can't have same elements in one brotherhood.");
						}
					}

					curr->children.push_back(child);
					size++;
					wave.push(child);
				}
			}
		}
	}

	in.close();

	return root;
}

std::vector<std::string> Tree::parseRow(const std::string& row) const
{
	std::vector<std::string> result;
	std::string curr;

	for (size_t i = 1; i < row.size() - 1; i++)
	{
		if (row[i] != ' ')
		{
			curr.push_back(row[i]);
		}
		else
		{
			if (curr != "") result.push_back(curr);
			curr.clear();
		}
	}
	if (curr != "") result.push_back(curr);

	return result;
}

bool Tree::containsSubtree(const Node* main, const Node* sub) const
{
	if (!sub) return true;

	if (!main) return false;

	if (main->data == sub->data && isSubTreeWithRoot(main, sub))
		return true;
	else
	{
		for (const Node* child : main->children)
			if (child->data == sub->data && isSubTreeWithRoot(child, sub))
				return true;
	}

	for (const Node* child : main->children)
		if (containsSubtree(child, sub)) return true;

	return false;
}

bool Tree::isSubTreeWithRoot(const Node* root1, const Node* root2) const
{
	if (!root2)	return true;

	for (const Node* child2 : root2->children)
	{
		bool found = false;
		const Node* child = nullptr;
		for (const Node* child1 : root1->children)
		{
			if (child2->data == child1->data)
			{
				found = true;
				child = child1;
				break;
			}
		}
		if (!found) return false;
		if (!isSubTreeWithRoot(child, child2)) return false;
	}

	return true;
}

Node* Tree::findSubtreeRec(Node* main, const Node* sub, int& sum) const
{
	if (!sub) return main;

	if (!main) return nullptr;

	if (main->data == sub->data && isSubTreeWithRoot(main, sub))
	{
		calcSum(main, sub, sum);
		return main;
	}
	else
	{
		sum = 0;
		for (Node* child : main->children)
			if (child->data == sub->data && isSubTreeWithRoot(child, sub))
			{
				calcSum(child, sub, sum);
				return child;
			}
		sum = 0;
	}

	for (Node* child : main->children)
	{
		Node* res = findSubtreeRec(child, sub, sum);
		if (res)
			return res;
	}

	return nullptr;
}

void Tree::calcSum(const Node* root1, const Node* root2, int& sum) const
{
	sum = sumOfTree(root1) - sumOfTree(root2);
}

Node* Tree::getParent(Node* root, const Node* toFind) const
{
	if (!root)
		return nullptr;

	if (root == toFind)
		return nullptr;

	for (const Node* child : root->children)
		if (child == toFind)
			return root;

	for (Node* child : root->children)
	{
		Node* parent = getParent(child, toFind);
		if (parent) return parent;
	}

	return nullptr;
}

int Tree::sumOfTree(const Node* root) const
{
	if (!root)
		return 0;

	int res = 0;
	for (const Node* child : root->children)
		res += sumOfTree(child);

	return res + root->data;
}

bool Tree::isEqual(const Tree& other) const
{
	return size == other.size && isSubTreeWithRoot(root, other.root);
}
