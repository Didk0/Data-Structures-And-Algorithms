#include <fstream>
#include "interface.h"
#include <sstream>

float percent(size_t x, size_t y)
{
	return (float)x / y * 100;
}

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		std::cerr << "Invalid input!";
		return 1;
	}

	std::ifstream file1(argv[1]), file2(argv[2]);

	if (!file1 || !file2)
	{
		std::cerr << "Couldn't open file for reading!";
		return 2;
	}

	Comparator c;
	ComparisonReport report = c.compare(file1, file2);

	size_t common = report.commonWords.words().size();
	size_t count1 = common + report.uniqueWords[0].words().size();
	size_t count2 = common + report.uniqueWords[1].words().size();

	std::cout << "File1 contains " << count1 <<  " words and " << common
		<< " are in File2. (" << percent(common, count1) << "%)" << std::endl;

	std::cout << "File2 contains " << count2 << " words and " << common
		<< " are in File1. (" << percent(common, count2) << "%)" << std::endl;

    return 0;
}