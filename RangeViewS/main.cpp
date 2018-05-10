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
	v = new std::vector<int>({ 1, 2, 3, 4, 5, 6, 7 });
	auto rvtest1 = (*v) | view::take(2);
	auto rvtest2 = rvtest1.toVector();
	//int sum = view::accumulate(view::ints(1)
	//	| view::take(10));

	int sum = view::accumulate(view::ints(1)
		| view::transform([](int i) {return i * i; })
		| view::take(10));

	int test = 0;
}