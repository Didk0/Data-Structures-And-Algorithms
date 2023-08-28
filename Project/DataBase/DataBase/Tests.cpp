#define CATCH_CONFIG_MAIN 
#include <cassert>
#include "catch2.hpp"
#include "Table.h"

TEST_CASE("Integer Constructor", "[Integer]")
{
	SECTION("Integer_GivenNoValue_Creates")
	{
		Integer i;

		REQUIRE(i.toInteger() == 0);
		REQUIRE(i.toString() == "0");
		REQUIRE(i.getBytes() == 4);
	}
	SECTION("Integer_GivenValue_Creates")
	{
		Integer i(123);

		REQUIRE(i.toInteger() == 123);
		REQUIRE(i.toString() == "123");
		REQUIRE(i.getBytes() == 4);
	}
}

TEST_CASE("Integer Operators", "[operator]")
{
	SECTION("Integer_Compares")
	{
		Integer i1(5);
		Integer i2(10);

		REQUIRE(i1 < i2);
		REQUIRE(i1 <= i2);
		REQUIRE(i1 == i1);
		REQUIRE(i2 >= i1);
		REQUIRE(i2 > i1);
	}
}

TEST_CASE("Double Constructor", "[Double]")
{
	SECTION("Double_GivenNoValue_Creates")
	{
		Double d;

		REQUIRE(d.toDouble() == 0);
		REQUIRE(d.toString() == "0.000");
		REQUIRE(d.getBytes() == 8);
	}
	SECTION("Double_GivenValue_Creates")
	{
		Double d(6.574);

		REQUIRE(d.toDouble() == 6.574);
		REQUIRE(d.toString() == "6.574");
		REQUIRE(d.getBytes() == 8);
	}
}

TEST_CASE("Double Operators", "[operator]")
{
	SECTION("Double_Compares")
	{
		Double d1(5.65);
		Double d2(6.87);

		REQUIRE(d1 < d2);
		REQUIRE(d1 <= d2);
		REQUIRE(d1 == d1);
		REQUIRE(d2 >= d1);
		REQUIRE(d2 > d1);
	}
}

TEST_CASE("String Constructor", "[String]")
{
	SECTION("String_GivenNoValue_Creates")
	{
		String s;

		REQUIRE(s.toDouble() == 0);
		REQUIRE(s.toInteger() == 0);
		REQUIRE(s.toString() == "");
		REQUIRE(s.getBytes() == 28);
	}
	SECTION("String_GivenValue_Creates")
	{
		String s("Deyan");

		REQUIRE(s.toString() == "Deyan");
		REQUIRE(s.getBytes() == 28);
	}
	SECTION("String_GivenNumValue_Creates")
	{
		String s("65");

		REQUIRE(s.toInteger() == 65);
		REQUIRE(s.toDouble() == 65);
		REQUIRE(s.toString() == "65");
		REQUIRE(s.getBytes() == 28);
	}
}

TEST_CASE("String Operators", "[operator]")
{
	SECTION("String_Compares")
	{
		String s1("Alex");
		String s2("Ivan");

		REQUIRE(s1 < s2);
		REQUIRE(s1 <= s2);
		REQUIRE(s1 == s1);
		REQUIRE(s2 >= s1);
		REQUIRE(s2 > s1);
	}
}

TEST_CASE("DateTime Constructor", "[DateTime]")
{
	SECTION("DateTime_GivenNoValue_Creates")
	{
		DateTime d;

		REQUIRE(d.toString() == "1-1-1");
		REQUIRE(d.getBytes() == 12);
	}
	SECTION("DateTime_GivenValue_Creates")
	{
		DateTime d("20-4-2000");

		REQUIRE(d.toString() == "20-4-2000");
		REQUIRE(d.getBytes() == 12);
	}
	SECTION("DateTime_GivenInvalidValue_Throws")
	{
		REQUIRE_THROWS(DateTime("32-4-2000"));
		REQUIRE_THROWS(DateTime("30-14-2000"));
		REQUIRE_THROWS(DateTime("30-2-1985"));
		REQUIRE_THROWS(DateTime("31-4-1985"));
	}
}

TEST_CASE("DateTime Operators", "[operator]")
{
	SECTION("DateTime_Compares")
	{
		DateTime d1("25-12-1985");
		DateTime d2("26-10-1986");

		REQUIRE(d1 < d2);
		REQUIRE(d1 <= d2);
		REQUIRE(d1 == d1);
		REQUIRE(d2 >= d1);
		REQUIRE(d2 > d1);
	}
}

TEST_CASE("Data OperatorsAndConstructors", "[operator]")
{
	SECTION("Data_Compares")
	{
		Data first(5);
		Data second(10);
		Data third(first);

		REQUIRE(first < second);
		REQUIRE(first <= second);
		REQUIRE(first == first);
		REQUIRE(second >= first);
		REQUIRE(second > first);
		REQUIRE(first.getBytes() == third.getBytes());
		REQUIRE(first.toString() == third.toString());

		third.~Data();
		REQUIRE(nullptr == third.getValue());

		second = first;
		REQUIRE(first.getBytes() == second.getBytes());
		REQUIRE(first.toString() == second.toString());
	}
}

TEST_CASE("Interval Constructor", "[Interval]")
{
	SECTION("Interval_GivenValue_Creates")
	{
		Interval i(Data(5), Data(8), "ID", "Int", false);

		REQUIRE(i.getLeftBound() == 5);
		REQUIRE(i.getRightBound() == 8);
		REQUIRE(i.getColName() == "ID");
		REQUIRE(i.isEmpty() == false);
		REQUIRE(i.toString() == "Int 5 8 ID false");

		Interval i2(Data("devet"), Data("sto"), "Val", "String", true);

		REQUIRE(i2.getLeftBound() == Data("devet"));
		REQUIRE(i2.getRightBound() == Data("sto"));
		REQUIRE(i2.getColName() == "Val");
		REQUIRE(i2.isEmpty() == true);
		REQUIRE(i2.toString() == "String devet sto Val true");
	}
}

TEST_CASE("Page Constructor", "[Page]")
{
	SECTION("Page_GivenValue_Creates")
	{
		Page p(10);

		REQUIRE(p.isEmpty() == true);
		REQUIRE(p.isFull() == false);
		REQUIRE(p.size() == 0);
	}
}

TEST_CASE("Page Methods", "[method]")
{
	SECTION("Page_AddRecord_Adds")
	{
		Page p(1);
		Record r;
		r.addColumn(Data(5));
		p.addRecord(r);

		REQUIRE(p.isEmpty() == false);
		REQUIRE(p.isFull() == true);
		REQUIRE(p.size() == 1);
	}
	SECTION("Page_RemoveRecord_Removes")
	{
		Page p(3);
		Record r;
		r.addColumn(Data(5));
		p.addRecord(r);
		p.removeRecord(0);

		REQUIRE(p.getRecord(0).getBytes() == 0);
	}
	SECTION("Page_GetRecord_Gets")
	{
		Page p(3);
		Record r;
		r.addColumn(Data(5));
		p.addRecord(r);

		REQUIRE(p.getRecord(0).getBytes() == 4);
		REQUIRE(p.getRecord(0).getColData(0) == r.getColData(0));
	}
	SECTION("Page_GivenInvalidIndex_Thros")
	{
		Page p(3);
		Record r;
		r.addColumn(Data(5));
		p.addRecord(r);

		REQUIRE_THROWS(p.getRecord(1));
		REQUIRE_THROWS(p.removeRecord(1));
	}
}

TEST_CASE("Record Constructor", "[Record]")
{
	SECTION("Record_GivenNoValue_Creates")
	{
		Record r;

		REQUIRE(r.isEmpty() == true);
		REQUIRE(r.size() == 0);
		REQUIRE(r.getBytes() == 0);
	}
}

TEST_CASE("Record Methods", "[method]")
{
	SECTION("Record_GivenNoValue_Creates")
	{
		Record r;

		REQUIRE(r.isEmpty() == true);
		REQUIRE(r.size() == 0);
		REQUIRE(r.getBytes() == 0);

		r.addColumn(Data(5));
		r.clear();
		REQUIRE(r.isEmpty() == true);
		REQUIRE(r.size() == 0);
		REQUIRE(r.getBytes() == 0);
	}
	SECTION("Record_GivenDataToInsert_Adds")
	{
		Record r;
		r.addColumn(Data(5));
		r.addColumn(Data("Pesho"));

		REQUIRE(r.isEmpty() == false);
		REQUIRE(r.size() == 2);
		REQUIRE(r.getBytes() == 32);
	}
	SECTION("Record_GivenIndex_GetsData")
	{
		Record r;
		r.addColumn(Data(5));
		r.addColumn(Data("Pesho"));

		REQUIRE(r.getColData(0) == 5);
		REQUIRE(r.getColData(1) == Data("Pesho"));
	}
	SECTION("Record_GivenInvalidIndex_Throws")
	{
		Record r;
		r.addColumn(Data(5));
		r.addColumn(Data("Pesho"));

		REQUIRE_THROWS(r.getColData(2));
	}
}

TEST_CASE("RecordPtr Constructor", "[RecordPtr]")
{
	SECTION("RecordPtr_GivenValue_Creates")
	{
		RecordPtr recPtr(1, 6);

		REQUIRE(recPtr.pageNumber() == 1);
		REQUIRE(recPtr.rowNumber() == 6);
	}
}

TEST_CASE("RecordPtr Operators", "[operator]")
{
	SECTION("RecordPtr_Compares")
	{
		RecordPtr r1(1, 6);
		RecordPtr r2(1, 8);
		RecordPtr r3(2, 1);

		REQUIRE(r1 < r2);
		REQUIRE(r1 == r1);
		REQUIRE(r2 > r1);
		REQUIRE(r1 < r3);
	}
}

TEST_CASE("BPlusTree Constructors", "[BPlusTree]")
{
	SECTION("BPlusTree_EmptyTree_Creates")
	{
		BPlusTree tree;

		REQUIRE(tree.getSize() == 0);
		REQUIRE(tree.isEmpty() == true);
	}
	SECTION("BPlusTree_CopyConstructor_Copies")
	{
		BPlusTree tree;
		tree.insert({ Data(5), RecordPtr(1,1) });

		BPlusTree tree2(tree);

		REQUIRE(tree2.getSize() == 1);
		REQUIRE(tree2.isEmpty() == false);
		REQUIRE(tree2.find(Data(5)));
	}
	SECTION("BPlusTree_operator=_Copies")
	{
		BPlusTree tree;
		tree.insert({ Data(5), RecordPtr(1,1) });

		BPlusTree tree2;
		tree2.insert({ Data(10), RecordPtr(1,1) });

		tree2 = tree;

		REQUIRE(tree2.getSize() == 1);
		REQUIRE(tree2.isEmpty() == false);
		REQUIRE(tree2.find(Data(5)));
	}
	SECTION("BPlusTree_Destructor_Clears")
	{
		BPlusTree tree;
		for (int i = 0; i < 10; i++)
		{
			tree.insert({ Data(i), RecordPtr(i,i) });
		}

		tree.~BPlusTree();

		REQUIRE(tree.getSize() == 0);
		REQUIRE(tree.isEmpty() == true);
	}
}

TEST_CASE("BPlusTree Insert, Min, Max, GetElementsInRange")
{
	SECTION("BPlusTree_Insert_GivenElement_Insert")
	{
		BPlusTree tree;
		vector<RecordPtr> vec;
		for (int i = 0; i < 100; i++)
		{
			tree.insert({ Data(i), RecordPtr(i,i) });
		}
		vec = tree.getElementsInRange(tree.min(), tree.max());

		REQUIRE(tree.getSize() == 100);
		REQUIRE(tree.isEmpty() == false);
		REQUIRE(tree.min() == 0);
		REQUIRE(tree.max() == 99);

		for (int i = 0; i < 100; i++)
		{
			REQUIRE(vec[i] == RecordPtr(i, i));
		}
	}
}

TEST_CASE("BPlusTree Remove")
{
	SECTION("BPlusTree_Remove_GivenElement_Remove")
	{
		BPlusTree tree(3);
		for (int i = 0; i < 100; i++)
		{
			tree.insert({ Data(i), RecordPtr(i,i) });
		}

		tree.remove({ Data(5), RecordPtr(5,5) });

		REQUIRE(tree.getSize() == 99);

		for (int i = 0; i < 5; i++)
		{
			tree.remove({ Data(i), RecordPtr(i,i) });
		}
		for (int i = 6; i < 100; i++)
		{
			tree.remove({ Data(i), RecordPtr(i,i) });
		}

		REQUIRE(tree.getSize() == 0);
		REQUIRE(tree.isEmpty() == true);
	}
}

TEST_CASE("BPlusTree Find")
{
	SECTION("BPlusTree_Find_GivenElement_Finds")
	{
		BPlusTree tree(3);
		for (int i = 0; i < 100; i++)
		{
			tree.insert({ Data(i), RecordPtr(i,i) });
		}

		for (int i = 0; i < 100; i++)
		{
			REQUIRE(tree.find(Data(i)));
		}
	}
}