#pragma once
#include <set>
#include <string>
#include <vector>
#include <forward_list>

const size_t    BUCKET_COUNT = 8;
const float     MAX_LOAD_FACTOR = 0.6;

class HashMap
{
	using Key = std::string;
	using Value = size_t;
	using Hash = std::hash<Key>;

public:
	HashMap(size_t buckets = BUCKET_COUNT)
		: fTable(buckets)
		, fSize(0)
		, fMaxLoadFactor(MAX_LOAD_FACTOR)
	{}
	HashMap(const HashMap&) = default;
	HashMap& operator=(const HashMap&) = default;
	~HashMap() = default;

	struct Data
	{
		Key     fKey;
		Value   fValue;
	};
	
	/// @brief Inserts pair in the hash table
	/// @param pair - pair of key and value
	/// @return If pair's key is found, returns false, otherwise returns true
	bool insert(const Data& pair)
	{
		if (contains(pair.fKey))
			return false;

		add(pair);

		return true;
	}

	/// @brief Erases a pair from the hash table with given key
	/// @param key - pair's key
	/// @return True if the pair is erased and false - otherwise
	bool erase(const Key& key)
	{
		size_t ind = hash(key);

		std::forward_list<Data>::iterator next = fTable[ind].before_begin();
		std::forward_list<Data>::iterator curr = fTable[ind].begin();

		for (; curr != fTable[ind].end(); ++next, ++curr)
		{
			if (curr->fKey == key)
			{
				fTable[ind].erase_after(next);
				--fSize;
				return true;
			}
		}

		return false;
	}

	/// @brief Checks if an element with given key is in the hash table
	/// @param key - key to search for
	/// @return True if pair with this key is found and false - otherwise
	bool contains(const Key& key) const
	{
		if (find(key))
			return true;

		return false;
	}

	/// @brief Searches for an element with given key and adds it if not found
	/// @param key - key to search for
	/// @return If found, returns the value of the pair with given key, otherwise adds it with default value 0
	Value& operator[](const Key& key)
	{
		if (Data* found = find(key))
			return found->fValue;

		Data d = { key };
		return add(d)->fValue;
	}

	/// @brief Gives the count of the buckets (rows in the array)
	/// @return The size of the vector
	size_t bucketCount() const
	{
		return fTable.size();
	}

	/// @brief Gives the count of the elements in the hash table
	/// @return The count of the elements
	size_t size() const {
		return fSize;
	}

	/// @brief Creates new hash table if needed and inserts all elements in it with new hash values
	/// @param bucketsCount - the current count of buckets multiplied by 2
	void rehash(size_t bucketsCount)
	{
		size_t minBuckets = std::ceil(fSize / fMaxLoadFactor);

		if (bucketsCount < minBuckets)
			bucketsCount = minBuckets;

		if (bucketCount() == bucketsCount)
			return;

		std::vector<std::forward_list<Data>> newTable(bucketsCount);
		std::swap(fTable, newTable);
		fSize = 0;

		for (const std::forward_list<Data>& lst : newTable)
			for (const Data& elem : lst)
				this->insert({ elem.fKey, elem.fValue });
	}

	/// @brief Const version of the find method
	/// @param key - key of the element
	/// @return Pointer to the element if found and nullptr - otherwise
	const Data* find(const Key& key) const
	{
		size_t ind = hash(key);

		for (const Data& elem : fTable[ind])
			if (elem.fKey == key)
				return &elem;

		return nullptr;
	}

	/// @brief Searches for an element with given key
	/// @param key - key of the element
	/// @return Pointer to the element if found and nullptr - otherwise
	Data* find(const Key& key)
	{
		return const_cast<Data*>(std::as_const(*this).find(key));
	}

	//.................. methods for interface.h .............................//

	/// @brief Fills words with each <key> <value> times
	/// @param words - multiset to fill
	void fillMultiset(std::multiset<std::string>& words) const
	{
		for (std::forward_list<Data> lst : fTable)
			for (Data& elem : lst)
				for (size_t i = 0; i < elem.fValue; i++)
					words.insert(elem.fKey);
	}

	//........................................................................//

private:
	/// @brief Finds the index in the vector for key with the hashing function
	/// @param key - key to find
	/// @return The hashed index 
	size_t hash(const Key& key) const
	{
		return fHashFunc(key) % bucketCount();
	}

	/// @brief Adds an element in the hash table
	/// @param pair - element to add
	/// @return Pointer to the added element
	Data* add(const Data& pair)
	{
		size_t ind = hash(pair.fKey);

		if ((float)(fSize + 1) / bucketCount() > fMaxLoadFactor)
			rehash(bucketCount() * 2);

		fTable[ind].push_front(pair);

		fSize++;
		return &fTable[ind].front();
	}

private:
	std::vector<std::forward_list<Data>> fTable;
	Hash fHashFunc;
	size_t fSize;
	float fMaxLoadFactor;
};