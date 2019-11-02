#include <iostream>
#include <string>
#include <thread>

#include "Benchmark.hpp"

using std::cout;
using std::endl;

int main()
{
	CBenchmark* bench = new CBenchmark();
	
	if (!bench->LoadSettingsFile("BenchProps.txt"))
	{
		wprintf(L"error\n");
		return -1;
	}
	auto finished = bench->IsFinished();

	std::thread ThreadFunc([](bool* isFinished)
		{
			constexpr const char* loadArr[] = { "Loading" , "Loading.", "Loading..", "Loading..." };
			int index = 0;
			while (!(*isFinished))
			{
				CLEAR_CONSOLE();
				cout << loadArr[index % 4];
				++index;
				std::this_thread::sleep_for(std::chrono::milliseconds(250));
			}
			CLEAR_CONSOLE();

		}, std::ref(finished));

	bench->Start();
	ThreadFunc.join();
	delete bench;
	cout << "Success\n";

	return 0;
}