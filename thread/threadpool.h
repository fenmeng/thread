
#pragma once
#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include "singleton.h"
#include <vector>
#include <queue>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

using wiseos::Singleton;

namespace wiseos
{
	class threadpool
	{
		using Task = std::function<void()>;
		// 线程池
		std::vector<std::thread> pool;
		// 任务队列
		std::queue<Task> tasks;
		// 同步
		std::mutex m_lock;
		// 条件阻塞
		std::condition_variable cv_task;
		// 是否关闭push
		std::atomic<bool> stoped;
		//空闲线程数量
		std::atomic<int>  idlThrNum;

	public:
		inline threadpool(unsigned short size = 20) :stoped{ false }
		{
			idlThrNum = size < 1 ? 1 : size;
			for (size = 0; size < idlThrNum; ++size)
			{   //初始化线程数量
				pool.emplace_back(
					[this]
				{ // 工作线程函数
					while (!this->stoped)
					{
						std::function<void()> task;
						{   // 获取一个待执行的 task
							std::unique_lock<std::mutex> lock{ this->m_lock };
							this->cv_task.wait(lock,
								[this] {
								return this->stoped.load() || !this->tasks.empty();
							}
							); // wait 直到有 task
							if (this->stoped && this->tasks.empty())
								return;
							task = std::move(this->tasks.front()); // 取一个 task
							this->tasks.pop();
						}
						idlThrNum--;
						task();
						idlThrNum++;
					}
				}
				);
			}
		}
		inline ~threadpool()
		{
			stoped.store(true);
			cv_task.notify_all(); // 唤醒所有线程执行
			for (std::thread& thread : pool) {
				if (thread.joinable())
					thread.join(); // 等待任务结束， 前提：线程一定会执行完
			}
		}

	public:
		template<class F, class... Args>
		auto push(F&& f, Args&&... args) ->std::future<decltype(f(args...))>
		{
			if (stoped.load())
				throw std::runtime_error("commit on ThreadPool is stopped.");

			using RetType = decltype(f(args...)); 
			auto task = std::make_shared<std::packaged_task<RetType()> >(
				std::bind(std::forward<F>(f), std::forward<Args>(args)...)
				);
			std::future<RetType> future = task->get_future();
			{    // 添加任务到队列
				std::lock_guard<std::mutex> lock{ m_lock };
				tasks.emplace(
					[task]()
				{
					(*task)();
				}
				);
			}
			cv_task.notify_one(); // 唤醒一个线程执行

			return future;
		}

		/*template<typename xReturn, typename...xParam>
		void AddTask(xReturn(*pfn)(xParam...), xParam...lp) {
			if (stoped.load())
				thow std::runtime_error("addtask on threadpool is stopped!");
			using Rettype = decltype()
		}*/

		//空闲线程数量
		int idlCount() { return idlThrNum; }
	private:
		SINGLETON_DECL(threadpool);
	};

}

#endif