#pragma once
#include <iostream>
#include "Node.h"

template<typename DataType>
struct DoubleLinkedList
{
	DoubleLinkedList()
		: head(nullptr)
		, tail(nullptr)
		, elemCount(0)
	{}
	DoubleLinkedList(const DoubleLinkedList& other) = delete;
	DoubleLinkedList& operator=(const DoubleLinkedList& other) = delete;
	~DoubleLinkedList()
	{
		clear();
	}

	DataType& front() { return head->person; }
	const DataType& front() const { return head->person; }

	DataType& back() { return tail->person; }
	const DataType& back() const { return tail->person; }

	Node<DataType>* begin() { return head; }
	const Node<DataType>* begin() const { return head; }

	Node<DataType>* end() { return tail; }
	const Node<DataType>* end() const { return tail; }

	void push_front(const DataType& value)
	{
		Node<DataType>* newNode = new Node<DataType>(value, head, nullptr);

		if (head)
			head->prev = newNode;

		head = newNode;

		if (!tail)
			tail = head;

		elemCount++;
	}

	void push_back(const DataType& value)
	{
		Node<DataType>* newNode = new Node<DataType>(value, nullptr, tail);

		if (tail)
			tail->next = newNode;

		tail = newNode;

		if (!head)
			head = tail;

		elemCount++;
	}

	void pop_front()
	{
		if (empty())
			throw std::underflow_error("Empty list!");

		Node<DataType>* toRem = head;
		head = head->next;

		if (elemCount == 1)
			tail = tail->next;

		delete toRem;

		if (head)
			head->prev = nullptr;

		elemCount--;
	}

	void pop_back()
	{
		if (empty())
			throw std::underflow_error("Empty list!");

		Node<DataType>* toRem = tail;
		tail = tail->prev;

		if (elemCount == 1)
			head = head->prev;

		delete toRem;

		if (tail)
			tail->next = nullptr;

		elemCount--;
	}

	Node<DataType>* insert(Node<DataType>* curr, const DataType& value)
	{
		if (!curr)
			throw std::invalid_argument("Invalid argument!");

		Node<DataType>* toInsert = new Node<DataType>(value, curr->next, curr);
		curr->next = toInsert;
		toInsert->next->prev = toInsert;

		elemCount++;

		return toInsert;
	}

	Node<DataType>* erase(Node<DataType>* curr)
	{
		if (!curr)
			throw std::invalid_argument("Invalid argument!");

		if (empty())
			throw std::underflow_error("Empty list!");

		if (!curr->next)
		{
			pop_back();
			return tail;
		}

		if (!curr->prev)
		{
			pop_front();
			return head;
		}

		Node<DataType>* toRem = curr;
		
		curr->next->prev = curr->prev;
		curr->prev->next = curr->next;
		curr = curr->next;

		delete toRem;

		elemCount--;

		return curr;
	}

	bool empty() const
	{
		return head == nullptr && tail == nullptr;
	}

	int size() const
	{
		return elemCount;
	}

private:
	void clear()
	{
		while (!empty())
		{
			pop_front();
		}
	}

private:
	Node<DataType>* head;
	Node<DataType>* tail;
	int elemCount;
};