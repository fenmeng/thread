#include "threadpool.h"
#include "singleton.h"
#include "function.h"
#include <iostream>
//#include <functional>

using wiseos::threadpool;
using wiseos::Singleton;
using wiseos::AA;

void func1()
{
	std::this_thread::sleep_for(std::chrono::seconds(1));
	std::cout << "Hello \t";
	
}

int func2(std::string a, std::string b)
{
	if (a.find(b) != std::string::npos)
	{
		std::cout << a.c_str() << std::endl;
	}
	else
	{
		std::cout << "without found!" << std::endl;
	}
	return 0;
}

int main()
{
	std::shared_ptr<threadpool> thrPool_;
	thrPool_ = Singleton<threadpool>::instance(25);

	std::shared_ptr<AA> a_;
	a_ = Singleton<AA>::instance();

	std::cout << thrPool_->idlCount() << std::endl;

	int a, b;
	a = 3, b = 4;

	for (int i = 0; i < 10; i++)
	{
		thrPool_->push([&] { a_->test2(a, b); });
	}
	
	thrPool_->push([=] {a_->test3(); });
	thrPool_->push(func2, "fengmeng", "f");
	//std::future<int> fm =  thrpool.push(func2, "fengmeng", "fe");
	

	std::cout << "===========================" << std::endl;

	std::cout << thrPool_->idlCount() << std::endl;
	return 0;
}