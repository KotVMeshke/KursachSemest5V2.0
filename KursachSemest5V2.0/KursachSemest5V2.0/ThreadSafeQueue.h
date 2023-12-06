#pragma once
#include <vector>
#include <queue>
#include <Windows.h>
#include <string>

class ThreadSafeQueue
{
private:
	std::queue<std::string> queue;
	CRITICAL_SECTION critSection;
public:

	ThreadSafeQueue();
	~ThreadSafeQueue();
	void InsertIntoQueue(std::string data);
	std::string RemoveFromQueue();
	void ClearQueue();
	bool isEmpty();

};

