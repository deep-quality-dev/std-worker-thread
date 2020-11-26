// StdWorkerThread.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "WorkerThread.h"

int main()
{
	WorkerThread thread;
	thread.CreateThread();

	int repeat = 10;
	while (repeat > 0) {
		std::shared_ptr<Message> msg(new Message());
		msg->msg = "Hello";
		msg->content = "Content";

		thread.PostMessage(msg);

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));

		repeat--;
	}

	thread.ExitThread();

    return 0;
}

