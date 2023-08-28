#include <iostream>

using namespace std;

struct Node
{
	int data;
	Node* next;
	Node(int d = -1, Node* n = nullptr) : data(d), next(n) {}
};

class LinkedList
{

public:
	LinkedList()
		: head(nullptr)
		, tail(nullptr)
		, size(0)
	{}
	LinkedList(const LinkedList& other)
	{
		copy(other);
	}
	LinkedList& operator=(const LinkedList& other)
	{
		if (this != &other)
		{
			clear();
			copy(other);
		}
		return *this;
	}
	~LinkedList()
	{
		clear();
	}

	void push_back(int data)
	{
		Node* newNode = new Node(data);
		if (!head)
			head = newNode;
		else
		{
			tail->next = newNode;
			tail = tail->next;
		}
		tail = newNode;
	}

	inline size_t getSize() const { return size; }
	inline Node* begin() const { return head; }
	inline Node* end() const { return tail; }

	// find the length of linked list recursively
	int findLengthRec(const Node* node) const
	{
		if (!node)
			return 0;

		return 1 + findLengthRec(node->next);
	}

	// swap two nodes of linked list without swaping the data
	void swapTwoNodes(int first, int second)
	{
		if (!head)
			return;
		if (first == second)
			return;

		Node* prevFirst = nullptr;
		Node* firstNode = head;
		while (firstNode && firstNode->data != first)
		{
			if (firstNode->next && firstNode->next->data == first) prevFirst = firstNode;
			firstNode = firstNode->next;
		}

		Node* prevSecond = nullptr;
		Node* secondNode = head;
		while (secondNode && secondNode->data != second)
		{
			if (secondNode->next && secondNode->next->data == second) prevSecond = secondNode;
			secondNode = secondNode->next;
		}

		if (!firstNode || !secondNode)
			return;

		if (prevFirst)
			prevFirst->next = secondNode;
		else
			head = secondNode;

		if (prevSecond)
			prevSecond->next = firstNode;
		else
			head = firstNode;

		if (!firstNode->next)
			tail = secondNode;
		else if (!secondNode->next)
			tail = firstNode;

		Node* temp = secondNode->next;
		secondNode->next = firstNode->next;
		firstNode->next = temp;
	}

	Node* mergeSort(Node* list) const
	{
		if (!list->next)
			return list;

		Node* left = nullptr, * right = nullptr;
		split(list, left, right);

		return merge(mergeSort(left), mergeSort(right));
	}
	void split(Node* list, Node*& first, Node*& second) const
	{
		Node dummyLeft;
		Node dummyRight;
		Node* iter = list;
		Node* end_left = &dummyLeft;
		Node* end_right = &dummyRight;

		bool swithToLeft = true;
		while (iter)
		{
			if (swithToLeft)
			{
				end_left->next = iter;
				end_left = end_left->next;
			}
			else
			{
				end_right->next = iter;
				end_right = end_right->next;
			}
			swithToLeft = !swithToLeft;
			iter = iter->next;
		}

		end_left->next = nullptr;
		end_right->next = nullptr;

		first = dummyLeft.next;
		second = dummyRight.next;
	}
	Node* merge(Node* first, Node* second) const
	{
		Node merged;
		Node* curr = &merged;

		while (first && second)
		{
			if (first->data < second->data)
			{
				curr->next = first;
				first = first->next;
			}
			else
			{
				curr->next = second;
				second = second->next;
			}
			curr = curr->next;
		}

		curr->next = first ? first : second;
		return merged.next;
	}

	Node* reverseInGroups(Node* node, int k)
	{
		if (!node) return nullptr;

		Node* curr = node;
		Node* next = nullptr, * prev = nullptr;
		int count = 0;

		while (curr && count < k)
		{
			next = curr->next;
			curr->next = prev;
			prev = curr;
			curr = next;
			count++;
		}

		if (next)
			node->next = reverseInGroups(next, k);

		return prev;
	}

	bool detectAndRemoveLoop(Node* list)
	{
		Node* slow = list, * fast = list;

		while (slow && fast->next)
		{
			slow = slow->next;
			fast = fast->next->next;

			if (slow->data == fast->data)
			{
				removeLoop(slow, list);
				return true;
			}
		}

		return false;
	}
	void removeLoop(Node* loopNode, Node* list)
	{
		Node* ptr1 = loopNode;
		Node* ptr2 = loopNode;
		size_t loopLen = 1;

		while (ptr1->next != ptr2)
		{
			ptr1 = ptr1->next;
			loopLen++;
		}

		ptr1 = list;

		ptr2 = list;
		for (size_t i = 0; i < loopLen; i++)
			ptr2 = ptr2->next;

		while (ptr1 != ptr2)
		{
			ptr1 = ptr1->next;
			ptr2 = ptr2->next;
		}

		while (ptr2->next != ptr1)
			ptr2 = ptr2->next;

		ptr2->next = nullptr;
	}

private:
	void copy(const LinkedList& other)
	{
		if (!other.head)
			head = tail = nullptr;
		else
		{
			head = new Node(other.head->data);
			Node* curr = head;
			Node* otherCurr = other.head;

			while (otherCurr->next)
			{
				otherCurr = otherCurr->next;
				curr->next = new Node(otherCurr->data);
				curr = curr->next;
			}
			tail = curr;
		}

		size = other.size;
	}
	void clear()
	{
		if (head)
		{
			Node* curr = head;
			while (curr)
			{
				Node* toRem = curr;
				curr = curr->next;
				delete toRem;
				size--;
			}
		}
		head = tail = nullptr;
	}

private:
	Node* head, * tail;
	size_t size;
};

int main()
{
	LinkedList lst;
	lst.push_back(15);
	lst.push_back(10);
	lst.push_back(12);
	lst.push_back(20);
	lst.push_back(14);
	lst.push_back(13);

	//lst.swapTwoNodes(12, 14);

	//Node* sorted = lst.mergeSort(lst.begin());

	Node* reversed = lst.reverseInGroups(lst.begin(), 3);

	return 0;
}