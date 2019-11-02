#ifndef TIMER_HPP
#define TIMER_HPP

#include <chrono>

namespace Timer
{

	class CTimer
	{
		std::chrono::steady_clock::time_point Start_, End_;
	public:
		CTimer() = default;

		void Start()
		{
			this->Start_ = std::chrono::high_resolution_clock::now();
		}

		// Returns milliseconds count
		float StopAndGet()
		{
			this->End_ = std::chrono::high_resolution_clock::now();

			std::chrono::duration<float, std::milli> fmSeconds = End_ - Start_;

			return fmSeconds.count();
		}

	};
}



#endif // TIMER_HPP