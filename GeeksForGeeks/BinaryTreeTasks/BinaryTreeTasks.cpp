#include <iostream>
using namespace std;

struct Node
{
	int data;
	Node* left, * right;
	Node(int d, Node* l = nullptr, Node* r = nullptr)
		: data(d), left(l), right(r) {}
};

int height(const Node* root)
{
	if (!root)
		return 0;

	return 1 + max(height(root->left), height(root->right));
}

int diameter(const Node* root)
{
	if (!root)
		return 0;

	int leftHeight = height(root->left);
	int rightHeight = height(root->right);

	int leftDiameter = diameter(root->left);
	int rightDiameter = diameter(root->right);

	return max(leftHeight + rightHeight + 1, max(leftDiameter, rightDiameter));
}

int search(int* arr, int startInd, int endInd, int value)
{
	for (int i = startInd; i <= endInd; i++)
	{
		if (arr[i] == value)
			return i;
	}
	return -1;
}

Node* buildTree(int* inArr, int* preArr, int startInd, int endInd)
{
	static int preInd = 0;

	if (startInd > endInd)
		return nullptr;

	Node* tNode = new Node(preArr[preInd++]);

	if (startInd == endInd)
		return tNode;

	int inInd = search(inArr, startInd, endInd, tNode->data);

	tNode->left = buildTree(inArr, preArr, startInd, inInd - 1);
	tNode->right = buildTree(inArr, preArr, inInd + 1, endInd);

	return tNode;
}

int getWidthAtLevel(const Node* root, int level)
{
	if (!root)
		return 0;

	if (level == 0)
		return 1;

	if (level < 0)
		return 0;

	return getWidthAtLevel(root->left, level - 1) + getWidthAtLevel(root->right, level - 1);
}

int getMaxWidth(const Node* root)
{
	if (!root)
		return 0;

	int max = 0;
	int h = height(root);

	for (int i = 0; i < h; i++)
	{
		int curr = getWidthAtLevel(root, i);
		if (curr > max)
			max = curr;
	}
	return max;
}

void printAllAtDist(const Node* root, int dist)
{
	if (!root)
		return;

	if (dist == 0)
		cout << root->data << ' ';

	printAllAtDist(root->left, dist - 1);
	printAllAtDist(root->right, dist - 1);
}

bool printAllAncestors(const Node* root, int key)
{
	if (!root)
		return false;

	if (root->data == key)
		return true;

	
	if (printAllAncestors(root->left, key) || printAllAncestors(root->right, key))
	{
		cout << root->data << ' ';
		return true;
	}

	return false;
}

bool isSubtreeWithRoot(const Node* root1, const Node* root2)
{
	if (!root2)
		return true;

	if (!root1)
		return false;

	if ((root2->left && root2->left->data != root1->left->data) || (root2->right && root2->right->data != root1->right->data))
		return false;

	return isSubtreeWithRoot(root1->left, root2->left) && isSubtreeWithRoot(root1->right, root2->right);
}

bool isSubTree(const Node* main, const Node* sub)
{
	if (!main) return false;

	if (!sub) return true;

	if (main->data == sub->data && isSubtreeWithRoot(main, sub))
		return true;

	return isSubTree(main->left, sub) || isSubTree(main->right, sub);
}

int main()
{
	Node* tree = new Node(5);
	tree->left = new Node(3);
	tree->left->left = new Node(2);
	tree->left->right = new Node(4);
	tree->left->right->left = new Node(6);
	tree->left->right->right = new Node(8);
	tree->right = new Node(7);
	tree->right->right = new Node(9);
	tree->right->right->right = new Node(10);
	tree->right->right->right->right = new Node(11);
	tree->right->right->right->left = new Node(12);
	tree->right->right->right->left->right = new Node(13);

	/*cout << diameter(tree);

	int arrInorder[12] = {2, 3, 6, 4, 8, 5, 7, 9, 12, 13, 10, 11};
	int arrPreorder[12] = {5, 3, 2, 4, 6, 8, 7, 9, 10, 12, 13, 11};

	Node* build = buildTree(arrInorder, arrPreorder, 0, 11);

	cout << getMaxWidth(tree);
	printAllAtDist(tree, 2);

	printAllAncestors(tree, 13);*/

	Node* subTree = new Node(3);
	subTree->left = new Node(2);
	subTree->right = new Node(4);

	cout << isSubTree(tree, subTree);

	return 0;
}
