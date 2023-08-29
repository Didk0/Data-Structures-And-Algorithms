#pragma once
#include <list>
#include <fstream>
#include <string>
#include <exception>
#include <queue>
#include <vector>
#include <iostream>

struct Node
{
	int data;
	std::list<Node*> children;
	Node(int d) : data(d) {}
};

class Tree
{
public:
	Tree() : root(nullptr), size(0) {}
	Tree(const std::string& file);
	Tree(const Tree& other);
	Tree& operator=(const Tree& other);
	~Tree();

	void save(const std::string& file) const;
	void load(const std::string& file) { init(file); }

	bool containsSubtree(const Tree& other) const
	{
		if (size < other.size) return false;

		return containsSubtree(root, other.root);
	};

	void deleteSubtree(const Tree& other);

private:
	void clear(Node*& root);
	Node* copy(Node* other);

	Node* init(const std::string& file);
	std::vector<std::string> parseRow(const std::string& row) const;

	bool containsSubtree(const Node* main, const Node* sub) const;
	bool isSubTreeWithRoot(const Node* root1, const Node* root2) const;

	Node* findSubtree(const Tree& other, int& sum) const
	{
		if (size < other.size) return nullptr;

		return findSubtreeRec(root, other.root, sum);
	};
	Node* findSubtreeRec(Node* main, const Node* sub, int& sum) const;
	void calcSum(const Node* root1, const Node* root2, int& sum) const;
	Node* getParent(Node* root, const Node* toFind) const;
	int sumOfTree(const Node* root) const;
	bool isEqual(const Tree& other) const;

private:
	Node* root;
	size_t size;
};

