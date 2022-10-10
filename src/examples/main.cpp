#include "..\Logger.h"
#include <vector>



int main(int argc, char* argv[])
{
	for (unsigned int i = 0; i < 1024; ++i)
	{
		LOG_ERROR(std::to_string(i).c_str());
	}

	auto threadTest = [](unsigned int id) {

		id *= 10000;
		for (unsigned int i = 0; i < 1024; ++i)
		{
			LOG_ERROR(std::to_string(id + i).c_str());
		}
	};

	std::vector<std::thread*> threads;
	const unsigned int threadCount = 5;
	for (unsigned int i = 0; i < threadCount; ++i)
	{
		threads.push_back(new std::thread(threadTest, i + 1));
	}

	for (unsigned int i = 0; i < threadCount; ++i)
	{
		threads[i]->join();
	}

	LOG_INFO("There is some info here...");
}