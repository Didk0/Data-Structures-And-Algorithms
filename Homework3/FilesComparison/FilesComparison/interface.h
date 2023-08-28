#include <iostream>
#include <set>
#include <string>
#include "HashMap.h"

class WordsMultiset {
public:
	void add(const std::string& word, size_t times = 1)
	{
		fHashMap[word] += times;
	}

	bool contains(const std::string& word) const
	{
		return fHashMap.contains(word);
	}

	size_t countOf(const std::string& word) const
	{
		const HashMap::Data* d = fHashMap.find(word);
		return d ? d->fValue : 0;
	}

	size_t countOfUniqueWords() const
	{
		return fHashMap.size();
	}

	std::multiset<std::string> words() const
	{
		std::multiset<std::string> multiset;
		return fillMultiset(multiset);
	}

	/// @brief Erases an element from the hash table if its value is equal or below 0
	/// @param word - the key of the element
	void extract(const std::string& word)
	{
		if (--fHashMap[word] <= 0)
			fHashMap.erase(word);
	}

private:
	/// @brief Fills words with each <key> <value> times
	/// @param words - multiset to fill
	/// @return The filled multiset
	std::multiset<std::string> fillMultiset(std::multiset<std::string>& words) const
	{
		fHashMap.fillMultiset(words);
		return words;
	}

private:
	HashMap fHashMap;
};

class ComparisonReport {
public:
	WordsMultiset commonWords;

	WordsMultiset uniqueWords[2];
};

class Comparator {
public:
	ComparisonReport compare(std::istream& a, std::istream& b)
	{
		ComparisonReport result;

		while (!a.eof() || !b.eof())
		{
			std::string currWordA, currWordB;
			currWordA = readWord(a, currWordA);
			currWordB = readWord(b, currWordB);

			if (currWordA != "")
			{
				if (result.uniqueWords[1].contains(currWordA))
				{
					result.uniqueWords[1].extract(currWordA);
					result.commonWords.add(currWordA);
				}
				else
					result.uniqueWords[0].add(currWordA);
			}

			if (currWordB != "")
			{
				if (result.uniqueWords[0].contains(currWordB))
				{
					result.uniqueWords[0].extract(currWordB);
					result.commonWords.add(currWordB);
				}
				else
					result.uniqueWords[1].add(currWordB);
			}
		}

		return result;
	}

private:
	bool isWhiteSpace(char c)
	{
		return c == 'n' || c == 'r' || c == 't';
	}
	
	/// @brief Reads and validates a word
	/// @param stream - stream to read from
	/// @param str - string to save the read word 
	/// @return The validated word
	std::string readWord(std::istream& stream, std::string str)
	{
		std::string res;
		stream >> str;

		for (size_t i = 0; i < str.size(); i++)
		{
			if (str[i] == '\\' && isWhiteSpace(str[i + 1]))
				i++;
			else
				res += str[i];
		}

		return res;
	}
};