#ifndef WORKER_THREAD_H_
#define WORKER_THREAD_H_

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <atomic>

struct Message
{
	std::string msg;
	std::string content;
};

struct ThreadMessage;

class WorkerThread
{
public:
	WorkerThread();
	~WorkerThread();

	bool CreateThread();
	void ExitThread();

	std::thread::id GetThreadId();

	void PostMessage(std::shared_ptr<Message> msg);

private:
	WorkerThread(const WorkerThread&) = delete;
	WorkerThread& operator=(const WorkerThread&) = delete;

	void Process();

	std::unique_ptr<std::thread> thread_;
	std::mutex mutex_;
	std::queue<std::shared_ptr<ThreadMessage>> queue_;
	std::condition_variable cv_;
	std::atomic<bool> exit_;

};

#endif
