#include "..\include\rangeview.h"
#include "gtest\gtest.h"

#include <vector>

class RangeTest : public ::testing::Test 
{
protected:
	std::vector<int> *_test_vector;
	void SetUp() 
	{
		_test_vector = new std::vector<int>({ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 });
	}

	void TearDown() 
	{
		delete _test_vector;
	}

};

TEST_F(RangeTest, createRangeFromCollection) 
{
	auto rv = (*_test_vector) | view::remove_if([](int i) { return i % 2 == 0; });
}

TEST_F(RangeTest, simpleTake) 
{
	auto v = new std::vector<int>({ 1, 2, 3, 4, 5, 6, 7, 10, 20, 30, 40 });
	auto rvtest1 = (*v) | view::take(2);
	ASSERT_EQ(static_cast<std::vector<int>>(rvtest1), std::vector<int>({ 1, 2}));
}

TEST_F(RangeTest, removeAndTake) 
{
	auto rv = (*_test_vector) | view::remove_if([](int i) { return i % 2 == 0; })
		| view::take(2);

	ASSERT_EQ(static_cast<std::vector<int>>(rv), std::vector<int>({ 1, 3 }));
}


TEST_F(RangeTest, ints) 
{
	auto rv = view::ints(1)
		| view::take(10);
	ASSERT_EQ(static_cast<std::vector<int>>(rv), std::vector<int>({ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 }));
}

TEST_F(RangeTest, removeAndAccumulate) 
{
	auto rv = view::ints(1)
		| view::remove_if([](int i) { return i % 2 == 0; })
		| view::take(10);

	ASSERT_EQ(accumulate(rv), 25);
}

TEST_F(RangeTest, reverseAndStringLess) 
{
	auto rv = view::ints(1)
		| view::transform([](int i) { return std::to_string(i); })
		| view::remove_if([](std::string i) { return i.length() > 1; })
		| view::take(4);

	ASSERT_EQ(accumulate(rv), "1234");
}

TEST_F(RangeTest, reverseAndStringMore) 
{
	auto rv = (*_test_vector) | view::reverse()
		| view::transform([](int i) { return std::to_string(i); })
		| view::remove_if([](std::string i) { return i.length() > 1; })
		| view::take(10);

	ASSERT_EQ(accumulate(rv), "987654321");
}

TEST_F(RangeTest, Transform) {
	auto rv = view::ints(5)
		| view::transform([](int i) { return std::to_string(i); })
		| view::take(10)
		| view::remove_if([](std::string i) { return i.size() > 1; });

	ASSERT_EQ(accumulate(rv), "56789");
}

int main(int argc, char *argv[]) 
{
	testing::InitGoogleTest(&argc, argv);
	RUN_ALL_TESTS();
	char biba;
	std::cin >> biba;
	return 0;
}