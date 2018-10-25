
#pragma once
#ifndef CLASSAA
#define CLASSAA

#include "singleton.h"
#include <iostream>

namespace wiseos {
	class AA
	{
	public:
		std::string test1(
			std::string &aa,
			std::string &bb);
		int test2(
			int &a,
			int &b);
		void test3();
	private:
		SINGLETON_DECL(AA);
	};
}

#endif // CLASSAA