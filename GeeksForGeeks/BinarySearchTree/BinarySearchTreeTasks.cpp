#include <iostream>
#include <stack>
#include <vector>

using namespace std;

struct Node
{
	int data;
	Node* left, * right;
	Node(int d, Node* l = nullptr, Node* r = nullptr)
		: data(d), left(l), right(r) {}
};

void insert(Node*& root, int key)
{
	if (!root)
		root = new Node(key);
	else
		root->data < key ? insert(root->right, key) : insert(root->left, key);
}

Node* find(Node* root, int key)
{
	if (!root)
		return nullptr;

	if (root->data == key)
		return root;

	return root->data < key ? find(root->left, key) : find(root->right, key);
}

Node* extractMin(Node*& root)
{
	if (root->left)
		return extractMin(root->left);

	Node* res = root;
	root = root->right;
	return res;
}

void remove(Node*& root, int key)
{
	if (!root)
		return;

	if (root->data == key)
	{
		Node* toRem = root;
		if (!root->left)
			root = root->right;
		else if (!root->right)
			root = root->left;
		else
		{
			Node* minRight = extractMin(root->right);
			minRight->left = root->left;
			minRight->right = root->right;
			root = minRight;
		}
		delete toRem;
	}
	else
		root->data < key ? remove(root->left, key) : remove(root->right, key);
}

Node* findMostRight(Node* root)
{
	if (root->right)
		return findMostRight(root->right);

	return root;
}
Node* findMostLeft(Node* root)
{
	if (root->left)
		return findMostLeft(root->left);

	return root;
}
void findPredAndSucc(Node* root, Node*& pred, Node*& succ, int key)
{
	if (!root)
		return;

	if (root->data == key)
	{
		if (root->left)
			succ = findMostRight(root->left);
		if (root->right)
			pred = findMostLeft(root->right);
		return;
	}

	if (key > root->data)
	{
		pred = root;
		findPredAndSucc(root->right, pred, succ, key);
	}
	else if (key <= root->data)
	{
		succ = root;
		findPredAndSucc(root->left, pred, succ, key);
	}
}

bool isBST(Node* root, Node* leftParent, Node* rightParent)
{
	if (!root)
		return true;

	if ((leftParent && leftParent->data > root->data) || (rightParent && rightParent->data < root->data))
		return false;

	return isBST(root->left, leftParent, root) && isBST(root->right, root, rightParent);
}

Node* lowestCommonAncestor(Node* root, int val1, int val2)
{
	while (root)
	{
		if (val1 < root->data && val2 < root->data)
			root = root->left;
		else if (val1 > root->data && val2 > root->data)
			root = root->right;
		else break;
	}
	return root;
}

Node* findNext(Node* root, Node* parent, int key)
{
	if (!root)
		return nullptr;

	if (root->data == key)
	{
		if (root->right)
			return findMostLeft(root->right);
		else
			return parent;
	}
	else if (root->data > key)
	{
		parent = root;
	}

	return root->data < key ? findNext(root->right, parent, key) : findNext(root->left, parent, key);
}

Node* findKthMax(Node* root, int& k)
{
	if (!root)
		return nullptr;

	Node* left = findKthMax(root->left, k);
	if (left) return left;

	if (k != 1)
		k--;
	else
		return root;

	return findKthMax(root->right, k);
}

void printInorderBothTrees(const Node* root1, const Node* root2)
{
	stack<const Node*> s1, s2;
	while (root1 || root2 || !s1.empty() || !s2.empty())
	{
		while (root1)
		{
			s1.push(root1);
			root1 = root1->left;
		}
		while (root2)
		{
			s2.push(root2);
			root2 = root2->left;
		}

		if (s2.empty() || (!s1.empty() && s1.top()->data <= s2.top()->data))
		{
			root1 = s1.top();
			s1.pop();
			cout << root1->data << ' ';
			root1 = root1->right;
		}
		else
		{
			root2 = s2.top();
			s2.pop();
			cout << root2->data << ' ';
			root2 = root2->right;
		}
	}
}

void findWrongNodes(Node* root, Node*& first, Node*& middle, Node*& last, Node*& prev)
{
	if (!root)
		return;

	findWrongNodes(root->left, first, middle, last, prev);

	if (prev && root->data < prev->data)
	{
		if (!first)
		{
			first = prev;
			middle = root;
		}
		else
			last = root;
	}

	prev = root;

	findWrongNodes(root->right, first, middle, last, prev);
}
void swapTwoWrongPlacedNodes(Node*& root)
{
	Node* first = nullptr, * middle = nullptr, * last = nullptr, * prev = nullptr;

	findWrongNodes(root, first, middle, last, prev);

	if (first && last)
	{
		swap(first->data, last->data);
	}
	else if (first && middle)
	{
		swap(first->data, middle->data);
	}
}

int findCeil(const Node* root, int key)
{
	if (!root)
		return -1;

	if (root->data == key)
		return key;

	if (root->data < key)
		return findCeil(root->right, key);

	int ceil = findCeil(root->left, key);
	return ceil >= key ? ceil : root->data;
}

void traverseAndFill(const Node* root, vector<int>& vec)
{
	if (!root)
		return;

	traverseAndFill(root->left, vec);
	vec.push_back(root->data);
	traverseAndFill(root->right, vec);
}
bool findPair(const Node* root, int pair)
{
	vector<int> vec;
	traverseAndFill(root, vec);

	int leftInd = 0;
	int rightInd = vec.size() - 1;

	while (leftInd < rightInd)
	{
		if (vec[leftInd] + vec[rightInd] == pair)
			return true;
		else if (vec[leftInd] + vec[rightInd] < pair)
			leftInd++;
		else
			rightInd--;
	}

	return false;
}

int size(Node* root)
{
	if (!root)
		return 0;

	return 1 + size(root->left) + size(root->right);
}
void fillArr(const Node* root, int* arr, int& ind)
{
	if (!root)
		return;

	fillArr(root->left, arr, ind);
	arr[ind++] = root->data;
	fillArr(root->right, arr, ind);
}
int* merge(int* arr1, int* arr2, int size1, int size2)
{
	int* merged = new int[size1 + size2];
	int ind = 0, i = 0, j = 0;
	while (i < size1 && j < size2)
	{
		if (arr1[i] < arr2[j])
			merged[ind++] = arr1[i++];
		else
			merged[ind++] = arr2[j++];
	}

	while (i < size1)
		merged[ind++] = arr1[i++];

	while (j < size2)
		merged[ind++] = arr2[j++];

	return merged;
}
Node* toTree(int arr[], int left, int right)
{
	if (left > right)
		return nullptr;

	int mid = (left + right) / 2;
	Node* root = new Node(arr[mid]);

	root->left = toTree(arr, left, mid - 1);
	root->right = toTree(arr, mid + 1, right);

	return root;
}
Node* mergeTrees(Node* root1, Node* root2)
{
	if (!root1) return root2;
	if (!root2) return root1;

	int size1 = size(root1);
	int size2 = size(root2);
	int* arr1 = new int[size1];
	int* arr2 = new int[size2];
	int ind1 = 0, ind2 = 0;
	fillArr(root1, arr1, ind1);
	fillArr(root2, arr2, ind2);

	int* merged = merge(arr1, arr2, size1, size2);
	int newSize = size1 + size2;
	Node* root = toTree(merged, 0, newSize - 1);

	delete[] arr1;
	delete[] arr2;
	delete[] merged;

	return root;
}

void printTree(const Node* root)
{
	if (root)
	{
		printTree(root->left);
		cout << root->data << ' ';
		printTree(root->right);
	}
}

void heapify(vector<int>& vec, int size, int ind)
{
	int largest = ind;
	int leftInd = 2 * ind + 1;
	int rightInd = 2 * ind + 2;

	if (leftInd < size && vec[leftInd] > vec[largest])
		largest = leftInd;

	if (rightInd < size && vec[rightInd] > vec[largest])
		largest = rightInd;

	if (ind != largest)
	{
		swap(vec[ind], vec[largest]);
		heapify(vec, size, largest);
	}
}
void heapSort(vector<int>& vec, int size)
{
	for (int i = size / 2 - 1; i >= 0; i--)
		heapify(vec, size, i);

	for (int i = size - 1; i > 0; i--)
	{
		swap(vec[0], vec[i]);
		heapify(vec, i, 0);
	}
}
void traverseAndChange(Node* root, vector<int>& vec, int& ind)
{
	if (root)
	{
		traverseAndChange(root->left, vec, ind);
		root->data = vec[ind++];
		traverseAndChange(root->right, vec, ind);
	}
}
Node* convertToBst(Node* root)
{
	vector<int> vec;
	traverseAndFill(root, vec);
	heapSort(vec, vec.size());
	int ind = 0;
	traverseAndChange(root, vec, ind);

	return root;
}

int main()
{
	Node* tree = nullptr;
	insert(tree, 5);
	insert(tree, 3);
	insert(tree, 9);
	insert(tree, 2);
	insert(tree, 4);
	insert(tree, 8);
	insert(tree, 10);
	insert(tree, 1);
	insert(tree, 7);
	insert(tree, 11);
	insert(tree, 13);
	insert(tree, 12);

	/*Node* pred = nullptr, * succ = nullptr;
	findPredAndSucc(tree, pred, succ, 6);
	cout << pred->data << ' ' << succ->data << endl;
	findPredAndSucc(tree, pred, succ, 5);
	cout << pred->data << ' ' << succ->data;*/

	//cout << boolalpha << isBST(tree, nullptr, nullptr);

	//cout << lowestCommonAncestor(tree, 1, 4)->data;

	//cout << findNext(tree, nullptr, 4)->data;

	/*int k = 8;
	cout << findKthMax(tree, k)->data;*/

	Node* tree2 = nullptr;
	insert(tree2, 10);
	tree2->right = new Node(7);
	tree2->left = new Node(2);
	tree2->left->left = new Node(8);
	tree2->left->right = new Node(4);

	//printInorderBothTrees(tree, tree2);

	//swapTwoWrongPlacedNodes(tree2);

	//cout << findCeil(tree2, 6);

	//cout << boolalpha << findPair(tree, 16);

	/*Node* mergedTree = mergeTrees(tree, tree2);
	printTree(mergedTree);*/

	/*printTree(tree2); cout << endl;
	Node* converted = convertToBst(tree2);
	printTree(converted);*/

	return 0;
}