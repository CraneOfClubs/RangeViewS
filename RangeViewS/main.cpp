#include "rangeview.h"
#include <string>

int main() {
	//std::vector<int> a = { 2, 4, 6, 8 };
	//std::function<bool(int)> f = [](int i) {return true; };
	//auto test = view::ints(10);
	//test.toVector();
	auto rv = view::ints(1)
		| view::take(10);
	std::vector<int> *v;
	v = new std::vector<int>({ 1, 2, 3, 4, 5, 6, 7, 10, 20, 30, 40 });
	auto rvtest1 = (*v) | view::take(2);
	auto rvtest2 = rvtest1.toVector();
	//int sum = view::accumulate(view::ints(1)
	//	| view::take(10));

	int sum = view::accumulate(view::ints(1)
		| view::transform([](int i) {return i * i; })
		| view::take(10));

	auto str = view::ints(1)
		| view::transform([](int i) { return std::to_string(i); })
		| view::remove_if([](std::string i) { return i.length() < 2; })
		| view::take(20);

	auto resstr = view::accumulate(str);

	auto str2 = (*v) 
		| view::reverse()
		| view::transform([](int i) { return std::to_string(i); })
		| view::remove_if([](std::string i) { return i.length() < 2; })
		| view::take(20);

	auto resstr2 = view::accumulate(str2);

	int test = 0;
}