#include "rangeview.h"
#include <string>

int main() {
	//std::vector<int> a = { 2, 4, 6, 8 };
	//std::function<bool(int)> f = [](int i) {return true; };
	//auto test = view::ints(10);
	//test.toVector();
	auto rv = view::ints(1)
		| view::take(10);
	int test = 0;
}