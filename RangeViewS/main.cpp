#include "rangeview.h"
#include <string>

int main() {
	std::vector<int> a = { 2, 4, 6, 8 };
	std::function<bool(int)> f = [](int i) {return true; };
}