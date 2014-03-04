#pragma once
#include <chrono>
#include <atomic>
#include <thread>
#include <iostream>
#include <functional>
#include <vector>

#define DEBUG(x) std::cerr << #x << ": " << ( x ) << std::endl;


class Clock
{
	public:
		using time_type = long long int;
		using handler_type = std::function<void(time_type)>;

		Clock() = default;
		Clock(const unsigned int step):
			_step(step)
		{
		}

		void addHandle(handler_type h)
		{
			_handles.push_back(h);
		}

		void set(const time_type t)
		{
			DEBUG(t)
			_time = t;
		}

		time_type get() const
		{
			return _time.load();
		}

		void run() // Dans un thread
		{
			_running = true;
			while(_running)
			{
				std::this_thread::sleep_for(_step);
				_time.store(_time.load() + _step.count());
				for(auto& handle : _handles) handle(_time.load());
			}
		}

		void start()
		{
			_time.store(0);
			_runThread = std::thread(&Clock::run, this);
		}

		void stop()
		{
			_running = false;
		}

	private:
		std::atomic_llong _time{0};
		bool _running{false};
		const std::chrono::milliseconds _step{std::chrono::milliseconds(20)};

		std::vector<handler_type> _handles{};

		std::thread _runThread;
};
