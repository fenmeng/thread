
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
		// �̳߳�
		std::vector<std::thread> pool;
		// �������
		std::queue<Task> tasks;
		// ͬ��
		std::mutex m_lock;
		// ��������
		std::condition_variable cv_task;
		// �Ƿ�ر�push
		std::atomic<bool> stoped;
		//�����߳�����
		std::atomic<int>  idlThrNum;

	public:
		inline threadpool(unsigned short size = 20) :stoped{ false }
		{
			idlThrNum = size < 1 ? 1 : size;
			for (size = 0; size < idlThrNum; ++size)
			{   //��ʼ���߳�����
				pool.emplace_back(
					[this]
				{ // �����̺߳���
					while (!this->stoped)
					{
						std::function<void()> task;
						{   // ��ȡһ����ִ�е� task
							std::unique_lock<std::mutex> lock{ this->m_lock };
							this->cv_task.wait(lock,
								[this] {
								return this->stoped.load() || !this->tasks.empty();
							}
							); // wait ֱ���� task
							if (this->stoped && this->tasks.empty())
								return;
							task = std::move(this->tasks.front()); // ȡһ�� task
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
			cv_task.notify_all(); // ���������߳�ִ��
			for (std::thread& thread : pool) {
				if (thread.joinable())
					thread.join(); // �ȴ���������� ǰ�᣺�߳�һ����ִ����
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
			{    // ������񵽶���
				std::lock_guard<std::mutex> lock{ m_lock };
				tasks.emplace(
					[task]()
				{
					(*task)();
				}
				);
			}
			cv_task.notify_one(); // ����һ���߳�ִ��

			return future;
		}

		/*template<typename xReturn, typename...xParam>
		void AddTask(xReturn(*pfn)(xParam...), xParam...lp) {
			if (stoped.load())
				thow std::runtime_error("addtask on threadpool is stopped!");
			using Rettype = decltype()
		}*/

		//�����߳�����
		int idlCount() { return idlThrNum; }
	private:
		SINGLETON_DECL(threadpool);
	};

}

#endif