#include "BPlusTree.h"

BPlusTree::BPlusTree(ifstream& in)
{
	root = headLeaf = tailLeaf = nullptr;
	in.read((char*)&degree, sizeof(degree));
	in.read((char*)&size, sizeof(size));

	for (size_t i = 0; i < size; i++)
	{
		insert({ Data(in), RecordPtr(in) });
		size--;
	}
}

BPlusTree::BPlusTree(const BPlusTree& other)
{
	headLeaf = tailLeaf = nullptr;
	root = copy(other.root);
	degree = other.degree;
	size = other.size;
}

BPlusTree::BPlusTree(BPlusTree&& other) noexcept
	: BPlusTree()
{
	std::swap(root, other.root);
	std::swap(degree, other.degree);
	std::swap(size, other.size);
	std::swap(headLeaf, other.headLeaf);
	std::swap(tailLeaf, other.tailLeaf);
}

BPlusTree& BPlusTree::operator=(const BPlusTree& other)
{
	if (this != &other)
	{
		clear(root);
		headLeaf = tailLeaf = nullptr;
		root = copy(other.root);
		degree = other.degree;
		size = other.size;
	}
	return *this;
}

BPlusTree& BPlusTree::operator=(BPlusTree&& other) noexcept
{
	if (this != &other)
	{
		std::swap(root, other.root);
		std::swap(degree, other.degree);
		std::swap(size, other.size);
		std::swap(headLeaf, other.headLeaf);
		std::swap(tailLeaf, other.tailLeaf);
	}
	return *this;
}

void BPlusTree::insert(const Kvp& toAdd)
{
	if (!root)
	{
		root = new LeafNode;
		LeafNode* leafNode = static_cast<LeafNode*>(root);
		leafNode->keys.push_back(toAdd);
		root = leafNode;
		headLeaf = tailLeaf = leafNode;
	}
	else
	{
		Node* cursor = root;
		InnerNode* innerCursor = static_cast<InnerNode*>(cursor);
		Node* parent = nullptr;

		// finding the right leaf to insert in
		while (!cursor->isLeaf)
		{
			parent = cursor;
			for (size_t i = 0; i < innerCursor->keys.size(); i++)
			{
				if (toAdd < innerCursor->keys[i])
				{
					cursor = innerCursor->children[i];
					innerCursor = static_cast<InnerNode*>(innerCursor->children[i]);
					break;
				}

				if (i == innerCursor->keys.size() - 1)
				{
					cursor = innerCursor->children[i + 1];
					innerCursor = static_cast<InnerNode*>(innerCursor->children[i + 1]);
					break;
				}
			}
		}

		LeafNode* leafNode = static_cast<LeafNode*>(cursor);
		if (leafNode->keys.size() < degree)
		{
			size_t ind = 0;

			while (ind < leafNode->keys.size() && toAdd > leafNode->keys[ind])
				ind++;

			leafNode->keys.insert(leafNode->keys.begin() + ind, toAdd);
		}
		else
		{
			Node* newNode = splitChild(cursor, toAdd);
			LeafNode* newLeaf = static_cast<LeafNode*>(newNode);

			if (cursor == root)
			{
				InnerNode* newRoot = new InnerNode;
				newRoot->keys.push_back(newLeaf->keys.front());
				newRoot->children[0] = cursor;
				newRoot->children[1] = newNode;
				root = newRoot;
			}
			else
			{
				insertInner(newLeaf->keys[0], parent, newNode);
			}
		}
	}
	size++;
}

BPlusTree::Node* BPlusTree::find(const Data& toFind) const
{
	if (!root)
		return nullptr;

	// looking for the searched leaf node
	Node* cursor = root;
	InnerNode* innerCursor = static_cast<InnerNode*>(cursor);
	while (!cursor->isLeaf)
	{
		for (size_t i = 0; i < innerCursor->keys.size(); i++)
		{
			if (toFind < innerCursor->keys[i].first)
			{
				cursor = innerCursor->children[i];
				innerCursor = static_cast<InnerNode*>(innerCursor->children[i]);
				break;
			}

			if (i == innerCursor->keys.size() - 1)
			{
				cursor = innerCursor->children[i + 1];
				innerCursor = static_cast<InnerNode*>(innerCursor->children[i + 1]);
				break;
			}
		}
	}

	// looking for the element in the found leaf node
	LeafNode* leafNode = static_cast<LeafNode*>(cursor);
	for (size_t i = 0; i < leafNode->keys.size(); i++)
		if (leafNode->keys[i].first == toFind)
			return leafNode;

	return nullptr;
}

void BPlusTree::remove(const Kvp& toRem)
{
	if (!root)
		return;

	Node* cursor = root;
	InnerNode* innerCursor = static_cast<InnerNode*>(cursor);
	Node* parent = nullptr;
	int leftSiblingInd = -1, rightSiblingInd = innerCursor->keys.size() + 1;

	while (!cursor->isLeaf)
	{
		parent = cursor;
		for (size_t i = 0; i < innerCursor->keys.size(); i++)
		{
			leftSiblingInd = i - 1;
			rightSiblingInd = i + 1;
			if (toRem < innerCursor->keys[i])
			{
				cursor = innerCursor->children[i];
				innerCursor = static_cast<InnerNode*>(innerCursor->children[i]);
				break;
			}

			if (i == innerCursor->keys.size() - 1)
			{
				leftSiblingInd = i;
				rightSiblingInd = i + 2;

				cursor = innerCursor->children[i + 1];
				innerCursor = static_cast<InnerNode*>(innerCursor->children[i + 1]);
				break;
			}
		}
	}


	LeafNode* leafCursor = static_cast<LeafNode*>(cursor);
	int ind = -1;
	for (size_t i = 0; i < leafCursor->keys.size(); i++)
	{
		if (leafCursor->keys[i] == toRem)
		{
			ind = i;
			break;
		}
	}

	if (ind == -1)
		return;

	leafCursor->keys.erase(leafCursor->keys.begin() + ind);
	size--;

	if (cursor == root)
	{
		if (leafCursor->keys.size() == 0)
		{
			delete cursor;
			root = nullptr;
			headLeaf = nullptr;
			tailLeaf = nullptr;
		}
		return;
	}

	if (leafCursor->keys.size() >= (degree + 1) / 2 - 1)
	{
		removeUp(parent, toRem);
		return;
	}

	InnerNode* innerParent = static_cast<InnerNode*>(parent);

	// check if we can take key from the left sibling if there is one
	if (leftSiblingInd >= 0)
	{
		Node* leftSibling = innerParent->children[leftSiblingInd];
		LeafNode* leafLeftSibling = static_cast<LeafNode*>(leftSibling);

		if (leafLeftSibling->keys.size() >= (degree + 1) / 2)
		{
			// get the left sibling's last key
			leafCursor->keys.insert(leafCursor->keys.begin(), leafLeftSibling->keys.back());

			leafLeftSibling->keys.erase(leafLeftSibling->keys.end() - 1);

			innerParent->keys[leftSiblingInd] = leafCursor->keys[0];

			removeUp(parent, toRem);
			return;
		}
	}

	// check if we can take key from the right sibling if there is one
	if (rightSiblingInd <= innerParent->keys.size())
	{
		Node* rightSibling = innerParent->children[rightSiblingInd];
		LeafNode* leafRightSibling = static_cast<LeafNode*>(rightSibling);

		if (leafRightSibling->keys.size() >= (degree + 1) / 2)
		{
			// get the right sibling's first key
			leafCursor->keys.push_back(leafRightSibling->keys[0]);

			leafRightSibling->keys.erase(leafRightSibling->keys.begin());

			innerParent->keys[rightSiblingInd - 1] = leafRightSibling->keys[0];

			removeUp(parent, toRem);
			return;
		}
	}

	if (leftSiblingInd >= 0)
	{
		Node* leftSibling = innerParent->children[leftSiblingInd];
		LeafNode* leafLeftSibling = static_cast<LeafNode*>(leftSibling);

		for (size_t i = 0; i < leafCursor->keys.size(); i++)
			leafLeftSibling->keys.push_back(leafCursor->keys[i]);

		leafLeftSibling->next = leafCursor->next;
		if (leafCursor->next)
			leafCursor->next->prev = leafLeftSibling;
		if (tailLeaf == leafCursor)
			tailLeaf = leafLeftSibling;

		removeInner(innerParent->keys[leftSiblingInd], parent, cursor);
		delete cursor;
	}
	else if (rightSiblingInd <= innerParent->keys.size())
	{
		Node* rightSibling = innerParent->children[rightSiblingInd];
		LeafNode* leafRightSibling = static_cast<LeafNode*>(rightSibling);

		for (size_t i = leafCursor->keys.size(), j = 0; j < leafRightSibling->keys.size(); i++, j++)
			leafCursor->keys.insert(leafCursor->keys.begin() + i, leafRightSibling->keys[j]);

		leafCursor->next = leafRightSibling->next;
		if (leafRightSibling->next)
			leafRightSibling->next->prev = leafCursor;
		if (tailLeaf == leafRightSibling)
			tailLeaf = leafCursor;

		removeInner(innerParent->keys[rightSiblingInd - 1], parent, rightSibling);
		delete rightSibling;
	}

	removeUp(parent, toRem);
}

vector<RecordPtr> BPlusTree::getElementsInRange(const Data& from, const Data& to) const
{
	vector<RecordPtr> result;
	Node* cursor = find(from);
	if (!cursor)
		return result;

	LeafNode* leafCursor = static_cast<LeafNode*>(cursor);

	size_t startInd = leafCursor->getInd(from);
	bool inRange = true;

	// finish the current Node
	for (size_t i = startInd; i < leafCursor->keys.size(); i++)
	{
		if (leafCursor->keys[i].first <= to)
			result.push_back(leafCursor->keys[i].second);
		else
		{
			inRange = false;
			break;
		}
	}

	leafCursor = leafCursor->next;

	while (inRange && leafCursor)
	{
		for (size_t i = 0; i < leafCursor->keys.size(); i++)
		{
			if (leafCursor->keys[i].first <= to)
				result.push_back(leafCursor->keys[i].second);
			else
			{
				inRange = false;
				break;
			}
		}
		leafCursor = leafCursor->next;
	}

	return result;
}

set<RecordPtr> BPlusTree::getElementsInRangeInSet(const Data& from, const Data& to) const
{
	set<RecordPtr> result;
	Node* cursor = find(from);
	if (!cursor)
		return result;

	LeafNode* leafCursor = static_cast<LeafNode*>(cursor);

	size_t startInd = leafCursor->getInd(from);
	bool inRange = true;

	// finish the current Node
	for (size_t i = startInd; i < leafCursor->keys.size(); i++)
	{
		if (leafCursor->keys[i].first <= to)
			result.insert(leafCursor->keys[i].second);
		else
		{
			inRange = false;
			break;
		}
	}

	leafCursor = leafCursor->next;

	while (inRange && leafCursor)
	{
		for (size_t i = 0; i < leafCursor->keys.size(); i++)
		{
			if (leafCursor->keys[i].first <= to)
				result.insert(leafCursor->keys[i].second);
			else
			{
				inRange = false;
				break;
			}
		}
		leafCursor = leafCursor->next;
	}

	return result;
}

Data BPlusTree::getNextGreaterThan(const Data& elem) const
{
	Node* cursor = find(elem);
	if (!cursor)
		return Data();

	LeafNode* leafCursor = static_cast<LeafNode*>(cursor);

	size_t startInd = leafCursor->getInd(elem);

	if (leafCursor->keys[startInd].first == max())
		return Data();

	if (startInd == leafCursor->keys.size() - 1)
	{
		leafCursor = leafCursor->next;
		return leafCursor->keys[0].first;
	}
	else
		return leafCursor->keys[startInd + 1].first;
}

Data BPlusTree::getPrevLesserThan(const Data& elem) const
{
	Node* cursor = find(elem);
	if (!cursor)
		return Data();

	LeafNode* leafCursor = static_cast<LeafNode*>(cursor);

	size_t startInd = leafCursor->getInd(elem);

	if (leafCursor->keys[startInd].first == min())
		return Data();

	if (startInd == 0)
	{
		leafCursor = leafCursor->prev;
		return leafCursor->keys[leafCursor->keys.size() - 1].first;
	}
	else
		return leafCursor->keys[startInd - 1].first;

	return Data();
}

void BPlusTree::serialize(ofstream& out) const
{
	out.write((char*)&degree, sizeof(degree));
	out.write((char*)&size, sizeof(size));

	LeafNode* leafNode = headLeaf;
	while (leafNode)
	{
		for (size_t i = 0; i < leafNode->keys.size(); i++)
		{
			leafNode->keys[i].first.serialize(out);
			leafNode->keys[i].second.serialize(out);
		}
		leafNode = leafNode->next;
	}
}

void BPlusTree::clear(Node*& root)
{
	if (root)
	{
		if (!root->isLeaf)
		{
			InnerNode* innernode = static_cast<InnerNode*>(root);
			for (size_t i = 0; i < innernode->keys.size() + 1; i++)
			{
				clear(innernode->children[i]);
			}
		}

		delete root;
		root = nullptr;
	}
}

BPlusTree::Node* BPlusTree::copy(Node* other)
{
	if (!other)
		return nullptr;

	if (other->isLeaf)
	{
		LeafNode* newLeaf = new LeafNode(other->isLeaf);
		LeafNode* otherLeaf = static_cast<LeafNode*>(other);

		newLeaf->keys.reserve(otherLeaf->keys.size());
		for (size_t i = 0; i < otherLeaf->keys.size(); i++)
			newLeaf->keys.push_back(otherLeaf->keys[i]);

		if (!headLeaf)
		{
			headLeaf = tailLeaf = newLeaf;
			newLeaf->prev = newLeaf->next = nullptr;
		}
		else
		{
			newLeaf->prev = tailLeaf;
			tailLeaf->next = newLeaf;
			tailLeaf = newLeaf;
		}

		return newLeaf;
	}
	else
	{
		InnerNode* newInner = new InnerNode(other->isLeaf);
		InnerNode* otherInner = static_cast<InnerNode*>(other);

		newInner->keys.reserve(otherInner->keys.size());
		for (size_t i = 0; i < otherInner->keys.size(); i++)
			newInner->keys.push_back(otherInner->keys[i]);

		for (size_t i = 0; i < otherInner->keys.size() + 1; i++)
			newInner->children[i] = copy(otherInner->children[i]);

		return newInner;
	}
}

BPlusTree::Node* BPlusTree::splitChild(Node*& cursor, const Kvp& toAdd)
{
	LeafNode* newLeaf = new LeafNode;
	LeafNode* cursorLeaf = static_cast<LeafNode*>(cursor);
	vector<Kvp> virtualNode;
	virtualNode.reserve(degree + 1);
	virtualNode.insert(virtualNode.begin(), cursorLeaf->keys.begin(), cursorLeaf->keys.end());

	size_t ind = 0;
	while (ind < degree && toAdd > virtualNode[ind])
		ind++;

	virtualNode.insert(virtualNode.begin() + ind, toAdd);

	cursorLeaf->keys.clear();
	cursorLeaf->keys.reserve((degree + 1) / 2);
	cursorLeaf->keys.insert(cursorLeaf->keys.begin(), virtualNode.begin(), virtualNode.begin() + (degree + 1) / 2);

	size_t newLeafPairsSize = degree + 1 - (degree + 1) / 2;
	newLeaf->keys.reserve(newLeafPairsSize);
	newLeaf->keys.insert(newLeaf->keys.begin(), virtualNode.begin() + (degree + 1) / 2, virtualNode.end());

	newLeaf->next = cursorLeaf->next;
	cursorLeaf->next = newLeaf;
	newLeaf->prev = cursorLeaf;
	if (newLeaf->next)
		newLeaf->next->prev = newLeaf;

	if (tailLeaf == cursorLeaf)
		tailLeaf = newLeaf;

	return newLeaf;
}

void BPlusTree::insertInner(const Kvp& toAdd, Node* parent, Node* child)
{
	InnerNode* innerParent = static_cast<InnerNode*>(parent);
	if (innerParent->keys.size() < degree)
	{
		size_t ind = 0;
		while (ind < innerParent->keys.size() && toAdd > innerParent->keys[ind])
			ind++;

		for (size_t j = innerParent->keys.size() + 1; j > ind + 1; j--)
			innerParent->children[j] = innerParent->children[j - 1];

		innerParent->keys.insert(innerParent->keys.begin() + ind, toAdd);
		innerParent->children[ind + 1] = child;
	}
	else
	{
		vector<Kvp> virtualKeys;
		vector<Node*> virtualChildren;
		virtualKeys.reserve(degree + 1);
		virtualChildren.reserve(degree + 2);

		virtualKeys.insert(virtualKeys.begin(), innerParent->keys.begin(), innerParent->keys.end());
		virtualChildren.insert(virtualChildren.begin(), innerParent->children.begin(), innerParent->children.end());

		size_t i = 0;
		while (i < degree && toAdd > virtualKeys[i])
			i++;

		virtualKeys.insert(virtualKeys.begin() + i, toAdd);
		virtualChildren.insert(virtualChildren.begin() + i + 1, child);

		size_t k = innerParent->children.size() - 1;
		while (innerParent->keys.size() != (degree + 1) / 2)
		{
			innerParent->keys.pop_back();
			innerParent->children[k] = nullptr;
			k--;
		}

		InnerNode* newInnerNode = new InnerNode;
		size_t newNodeKeysSize = degree - (degree + 1) / 2;
		newInnerNode->keys.reserve(newNodeKeysSize);
		newInnerNode->keys.insert(newInnerNode->keys.begin(), virtualKeys.begin() + innerParent->keys.size() + 1, virtualKeys.end());

		for (size_t i = 0; i < innerParent->keys.size() + 1; i++)
			innerParent->children[i] = virtualChildren[i];

		for (size_t j = 0, i = innerParent->keys.size() + 1; i < virtualChildren.size(); i++, j++)
			newInnerNode->children[j] = virtualChildren[i];

		if (parent == root)
		{
			InnerNode* newRoot = new InnerNode;
			newRoot->keys.push_back(virtualKeys[innerParent->keys.size()]);
			newRoot->children[0] = parent;
			newRoot->children[1] = newInnerNode;
			root = newRoot;
		}
		else
		{
			insertInner(virtualKeys[innerParent->keys.size()], findParent(root, parent), newInnerNode);
		}
	}
}

BPlusTree::Node* BPlusTree::findParent(Node* root, Node* child) const
{
	if (!root || child == root)
		return nullptr;

	Node* parent = nullptr;
	InnerNode* innerRoot = static_cast<InnerNode*>(root);
	if (root->isLeaf)
		return nullptr;

	for (Node* rootChild : innerRoot->children)
	{
		if (rootChild == child)
			return root;
		else
		{
			parent = findParent(rootChild, child);
			if (parent)
				return parent;
		}
	}

	return parent;
}

void BPlusTree::removeUp(Node* cursor, const Kvp& toDel)
{
	if (cursor)
	{
		InnerNode* innerCursor = static_cast<InnerNode*>(cursor);
		int ind = innerCursor->getInd(toDel);
		if (ind == -1)
		{
			removeUp(findParent(root, cursor), toDel);
		}
		else
		{
			innerCursor->keys[ind] = getMinElem(innerCursor->children[ind + 1]);
		}
	}
}

BPlusTree::Kvp BPlusTree::getMinElem(Node* root) const
{
	if (root)
	{
		InnerNode* innerNode = static_cast<InnerNode*>(root);
		while (!root->isLeaf)
		{
			root = innerNode->children[0];
			innerNode = static_cast<InnerNode*>(innerNode->children[0]);
		}

		LeafNode* leafNode = static_cast<LeafNode*>(root);
		return leafNode ? leafNode->keys.front() : Kvp{ Data(), RecordPtr(-1,-1) };
	}

	return { Data(), RecordPtr(-1,-1) };
}

void BPlusTree::removeInner(const Kvp& toRem, Node*& cursor, Node*& child)
{
	InnerNode* innerCursor = static_cast<InnerNode*>(cursor);

	// something like the bottom of the recursion
	if (cursor == root)
	{
		if (innerCursor->keys.size() == 1)
		{
			if (innerCursor->children[1] == child)
			{
				delete child;
				root = innerCursor->children[0];
				delete cursor;
				cursor = child = nullptr;
				return;
			}
			else if (innerCursor->children[0] == child) {
				delete child;
				root = innerCursor->children[1];
				delete cursor;
				cursor = child = nullptr;
				return;
			}
		}
	}

	// find child's index from parent's children pointers
	size_t ind;
	for (ind = 0; ind < innerCursor->keys.size() + 1; ind++)
		if (innerCursor->children[ind] == child)
			break;

	// rearrange parent's children pointers
	for (size_t i = ind; i < innerCursor->keys.size() + 1; i++)
	{
		if (i != innerCursor->keys.size())
			innerCursor->children[i] = innerCursor->children[i + 1];
		else
			innerCursor->children[i] = nullptr;
	}

	// find the index of the element that will be removed from parent's keys
	for (ind = 0; ind < innerCursor->keys.size(); ind++)
		if (innerCursor->keys[ind] == toRem)
			break;

	innerCursor->keys.erase(innerCursor->keys.begin() + ind);

	// if it is still more than half full we are OK
	if (innerCursor->keys.size() >= (degree + 1) / 2 - 1)
		return;

	if (innerCursor == root)
		return;

	Node* parent = findParent(root, cursor);
	InnerNode* innerParent = static_cast<InnerNode*>(parent);

	int leftSiblingInd = -1, rightSiblingInd = innerParent->keys.size() + 1;

	// get the indexes of left and right (if they exist) siblings from the parent
	for (ind = 0; ind < innerParent->keys.size() + 1; ind++)
	{
		if (innerParent->children[ind] == cursor)
		{
			leftSiblingInd = ind - 1;
			rightSiblingInd = ind + 1;
			break;
		}
	}

	// check if we can take key from the left sibling if there is one
	if (leftSiblingInd >= 0)
	{
		Node* leftSibling = innerParent->children[leftSiblingInd];
		InnerNode* innerLeftSibling = static_cast<InnerNode*>(leftSibling);

		if (innerLeftSibling->keys.size() >= (degree + 1) / 2)
		{
			innerCursor->keys.insert(innerCursor->keys.begin(), innerParent->keys[leftSiblingInd]);
			innerParent->keys[leftSiblingInd] = innerLeftSibling->keys[innerLeftSibling->keys.size() - 1];

			for (int i = innerCursor->keys.size() + 1; i > 0; i--)
				innerCursor->children[i] = innerCursor->children[i - 1];

			innerCursor->children[0] = innerLeftSibling->children[innerLeftSibling->keys.size()];
			innerLeftSibling->children[innerLeftSibling->keys.size()] = nullptr;
			innerLeftSibling->keys.pop_back();
			return;
		}
	}

	// check if we can take key from the right sibling if there is one
	if (rightSiblingInd <= innerParent->keys.size())
	{
		Node* rightSibling = innerParent->children[rightSiblingInd];
		InnerNode* innerRightSibling = static_cast<InnerNode*>(rightSibling);

		if (innerRightSibling->keys.size() >= (degree + 1) / 2)
		{
			innerCursor->keys.push_back(innerParent->keys[ind]);
			innerParent->keys[ind] = innerRightSibling->keys[0];
			innerCursor->children[innerCursor->keys.size()] = innerRightSibling->children[0];

			for (size_t i = 0; i < innerRightSibling->keys.size(); i++)
				innerRightSibling->children[i] = innerRightSibling->children[i + 1];

			innerRightSibling->children[innerRightSibling->keys.size()] = nullptr;
			innerRightSibling->keys.erase(innerRightSibling->keys.begin());
			return;
		}
	}

	if (leftSiblingInd >= 0)
	{
		Node* leftSibling = innerParent->children[leftSiblingInd];
		InnerNode* innerLeftSibling = static_cast<InnerNode*>(leftSibling);

		innerLeftSibling->keys.push_back(innerParent->keys[leftSiblingInd]);

		for (size_t i = innerLeftSibling->keys.size(), j = 0; i < degree + 1 && j < innerCursor->keys.size() + 1; j++, i++)
		{
			innerLeftSibling->children[i] = innerCursor->children[j];
			innerCursor->children[j] = nullptr;
		}

		for (size_t j = 0; j < innerCursor->keys.size(); j++)
			innerLeftSibling->keys.push_back(innerCursor->keys[j]);

		removeInner(innerParent->keys[leftSiblingInd], parent, cursor);
	}
	else if (rightSiblingInd <= innerParent->keys.size())
	{
		Node* rightSibling = innerParent->children[rightSiblingInd];
		InnerNode* innerRightSibling = static_cast<InnerNode*>(rightSibling);

		innerCursor->keys.push_back(innerParent->keys[rightSiblingInd - 1]);

		for (size_t i = innerCursor->keys.size(), j = 0; i < degree + 1 && j < innerRightSibling->keys.size() + 1; j++, i++)
		{
			innerCursor->children[i] = innerRightSibling->children[j];
			innerRightSibling->children[j] = nullptr;
		}

		for (size_t j = 0; j < innerRightSibling->keys.size(); j++)
			innerCursor->keys.push_back(innerRightSibling->keys[j]);

		removeInner(innerParent->keys[rightSiblingInd - 1], parent, rightSibling);
	}
}
