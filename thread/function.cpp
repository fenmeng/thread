#include <iostream>
#include "function.h"

namespace wiseos {
	std::string AA::test1
		(
			std::string &aa,
			std::string &bb
			)
	{
		std::cout << aa.c_str() << " and " << bb.c_str() << std::endl;
		return aa;
	}

	int AA::test2(int & a, int & b)
	{
		if (a > b) {
			std::cout << "yes";
		}
		else
			std::cout << "no";

		std::this_thread::sleep_for(std::chrono::seconds(1));
		return 0;
	}
	void AA::test3() {
		std::cout << "Hello world!" << std::endl;
	}
}

