#pragma once
#include "Data.h"
#include "RecordPtr.h"
#include <set>
#include <vector>

using std::set;
using std::vector;
using std::pair;
using std::ifstream;
using std::ofstream;

const size_t DEGREE = 12;

class BPlusTree
{
	/// @brief A pair struct with Data and RecordPtr elements
	struct Kvp
	{
		Data first;
		RecordPtr second;

		bool operator<(const Kvp& other) const
		{
			if (first < other.first)
			{
				return true;
			}
			else if (first == other.first)
			{
				if (second < other.second)
					return true;
				
				/*if (second.rowNumber() + second.pageNumber() < other.second.rowNumber() + other.second.pageNumber())
					return true;*/
			}

			return false;
		}

		bool operator>(const Kvp& other) const
		{
			if (first > other.first)
			{
				return true;
			}
			else if (first == other.first)
			{
				if (second > other.second)
					return true;

				/*if (second.rowNumber() + second.pageNumber() > other.second.rowNumber() + other.second.pageNumber())
					return true;*/
			}

			return false;
		}

		bool operator==(const Kvp& other) const
		{
			return first == other.first && second == other.second;
		}
	};

	/// @brief Node of the tree
	struct Node
	{
		bool isLeaf;
		vector<Kvp> keys;

		Node(bool l) : isLeaf(l) {}
	};

	/// @brief The inner Node
	struct InnerNode : public Node
	{
		vector<Node*> children;

		InnerNode(bool l = false) 
			: Node(l) 
		{
			children.reserve(DEGREE + 1);
			for (size_t i = 0; i < DEGREE + 1; i++)
				children.push_back(nullptr);
		}

		int getInd(const Kvp& kvp)
		{
			for (size_t i = 0; i < keys.size(); i++)
				if (keys[i] == kvp)
					return i;

			return -1;
		}
	};

	/// @brief The leaf Node
	struct LeafNode : public Node
	{
		LeafNode* next;
		LeafNode* prev;

		LeafNode(bool l = true, LeafNode* n = nullptr, LeafNode* p = nullptr) 
			: Node(l)
			, next(n)
			, prev(p)
		{}

		int getInd(const Data& key)
		{
			for (size_t i = 0; i < keys.size(); i++)
				if (keys[i].first == key)
					return i;

			return -1;
		}
	};

public:
	BPlusTree(size_t deg = DEGREE) : root(nullptr), headLeaf(nullptr), tailLeaf(nullptr), degree(deg), size(0) {};
	/// @brief Deserializing constructor
	/// @param in - the file that the tree will be read from
	BPlusTree(ifstream& in);
	BPlusTree(const BPlusTree& other);
	BPlusTree(BPlusTree&& other) noexcept;
	BPlusTree& operator=(const BPlusTree& other);
	BPlusTree& operator=(BPlusTree&& other) noexcept;
	~BPlusTree() { clear(root); size = 0; headLeaf = nullptr; tailLeaf = nullptr; }

	/// @brief Inserts an element into the tree
	/// @param toAdd - the element to be inserted
	void insert(const Kvp& toAdd);

	/// @brief Searches for a Node with a given value
	/// @param toFind - the value to be searched
	/// @return Node with the searched value if found and nullptr otherwise
	Node* find(const Data& toFind) const;

	/// @brief Removes an element from the tree
	/// @param toRem - the element to be removed
	void remove(const Kvp& toRem);

	/// @brief Gets a vector of elements in a given range
	/// @param from - starting poing
	/// @param to - ending point
	/// @return Vector of record pointer in the range [from, to]  
	vector<RecordPtr> getElementsInRange(const Data& from, const Data& to) const;

	/// @brief Gets a set of elements in a given range
	/// @param from - starting poing
	/// @param to - ending point
	/// @return Set of record pointer in the range [from, to] 
	set<RecordPtr> getElementsInRangeInSet(const Data& from, const Data& to) const;

	/// @brief Gets the element after elem
	/// @param elem - the searched element
	/// @return The next greater element
	Data getNextGreaterThan(const Data& elem) const;

	/// @brief Gets the element before elem
	/// @param elem - the searched element
	/// @return The previous lesser element
	Data getPrevLesserThan(const Data& elem) const;

	/// @brief Gets the min element in the tree
	/// @return the min element 
	const Data min() const { return headLeaf->keys[0].first; };

	/// @brief Gets the max element in the tree
	/// @return the max element 
	const Data max() const { return tailLeaf->keys[tailLeaf->keys.size() - 1].first; };

	inline size_t getSize() const { return size; }
	inline bool isEmpty() const { return size == 0; }

	/// @brief Saves the tree to a file in binary format
	/// @param out - the file
	void serialize(ofstream& out) const;

private:
	/// @brief Helper function for the destructor
	/// @param root - root of the tree
	void clear(Node*& root);

	/// @brief Helper function for the copy constructor
	/// @param other - the other root
	/// @return the root of the newly copied tree
	Node* copy(Node* other);

	/// @brief Splits the cursor Node into 2 Nodes
	/// @param cursor - the Node that will be splitted
	/// @param toAdd - the value of the element that will be inserted
	/// @return Node that contains half of the cursor Node
	Node* splitChild(Node*& cursor, const Kvp& toAdd);

	/// @brief Function that inserts the new element in the inner Nodes if needed
	/// @param toAdd - the element that will be inserted
	/// @param parent - the parent of the new Node
	/// @param child - the new Node
	void insertInner(const Kvp& toAdd, Node* parent, Node* child);

	/// @brief Finds the parent of a given Node
	/// @param root - the root we start searching from
	/// @param child - the node which parent we search for
	/// @return the parent Node of the child if found and nullptr otherwise
	Node* findParent(Node* root, Node* child) const;

	/// @brief Removes a given element up in the inner nodes of the tree
	/// @param cursor - the Node we start removing up from
	/// @param toDel - the element we want to be removing
	void removeUp(Node* cursor, const Kvp& toDel);

	/// @brief Gets the most left element starting from a given Node and going down
	/// @param root - the Node we start from
	/// @return the smallest element in a leaf Node
	Kvp getMinElem(Node* root) const;

	/// @brief Function that removes a given element from the inner Nodes if needed
	/// @param toRem - the element to remove
	/// @param parent - the parent of the Node that is deleted from
	/// @param child - the Node that is deleted from
	void removeInner(const Kvp& toRem, Node*& parent, Node*& child);

private:
	Node* root;
	LeafNode* headLeaf;
	LeafNode* tailLeaf;
	size_t degree;
	size_t size;
};

