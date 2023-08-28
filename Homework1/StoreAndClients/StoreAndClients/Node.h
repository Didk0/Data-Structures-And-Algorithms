#pragma once
#include "MyStore.h"

template<typename DataType>
struct Node
{
	DataType person;
	Node* next;
	Node* prev;

	Node(const DataType& person, Node* next = nullptr, Node* prev = nullptr)
		: person(person)
		, next(next)
		, prev(prev)
	{}
};