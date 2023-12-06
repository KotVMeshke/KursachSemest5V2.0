#include "ThreadSafeQueue.h"


ThreadSafeQueue::ThreadSafeQueue()
{
	InitializeCriticalSection(&critSection);
}

ThreadSafeQueue::~ThreadSafeQueue()
{
	DeleteCriticalSection(&critSection);
}

void ThreadSafeQueue::InsertIntoQueue(std::string data)
{
	//EnterCriticalSection(&critSection);
	queue.push(data);
	//LeaveCriticalSection(&critSection);
}

std::string ThreadSafeQueue::RemoveFromQueue()
{
	std::string result;
	EnterCriticalSection(&critSection);
	if (!queue.empty())
	{
		result = queue.front();
		queue.pop();
	}
	else
	{
		result = "sdfsdf";
	}
	LeaveCriticalSection(&critSection);
	return result;
}

void ThreadSafeQueue::ClearQueue()
{
	while (!queue.empty())
	{
		queue.pop();
	}
}

bool ThreadSafeQueue::isEmpty()
{

	EnterCriticalSection(&critSection);
	if (queue.empty())
	{
		return true;
	}
	LeaveCriticalSection(&critSection);

	return false;
}