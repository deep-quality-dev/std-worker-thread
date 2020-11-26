#include "stdafx.h"
#include "WorkerThread.h"
#include <iostream>
#include <memory>

#define MSG_DATA 1
#define MSG_EXIT 2

struct ThreadMessage
{
	ThreadMessage(int msgType, std::shared_ptr<void> msgData) {
		this->msgType = msgType;
		this->msgData = msgData;
	}

	int msgType;
	std::shared_ptr<void> msgData;
};

WorkerThread::WorkerThread()
{
}

WorkerThread::~WorkerThread()
{
}

bool WorkerThread::CreateThread()
{
	if (!thread_) {
		thread_ = std::unique_ptr<std::thread>(new std::thread(&WorkerThread::Process, this));
	}
	return true;
}

void WorkerThread::ExitThread()
{
	if (!thread_)
		return;

	std::shared_ptr<ThreadMessage> msg(new ThreadMessage(MSG_EXIT, 0));
	{
		std::unique_lock<std::mutex> lk(mutex_);
		queue_.push(msg);
		cv_.notify_one();
	}

	thread_->join();
	thread_ = nullptr;
}

std::thread::id WorkerThread::GetThreadId()
{
	return thread_->get_id();
}

void WorkerThread::PostMessage(std::shared_ptr<Message> msg)
{
	if (!thread_)
		return;

	{
		std::unique_lock<std::mutex> lk(mutex_);
		queue_.push(std::make_shared<ThreadMessage>(ThreadMessage(MSG_DATA, msg)));
		cv_.notify_one();
	}
}

void WorkerThread::Process()
{
	while (true) {
		std::shared_ptr<ThreadMessage> threadmsg;
		{
			std::unique_lock<std::mutex> lk(mutex_);
			while (queue_.empty())
				cv_.wait(lk);

			if (queue_.size() < 1)
				continue;

			threadmsg = queue_.front();
			queue_.pop();
		}

		switch (threadmsg->msgType) {
		case MSG_DATA:
		{
			auto userData = std::static_pointer_cast<Message>(threadmsg->msgData);
			std::cout << userData->msg.c_str() << ", " << userData->content.c_str() << std::endl;
		}
		break;

		case MSG_EXIT:
		{
			return;
		}
		}
	}
}