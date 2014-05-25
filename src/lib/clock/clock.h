#pragma once
#include <chrono>
#include <atomic>
#include <thread>
#include <iostream>
#include <functional>
#include <vector>
#include <unistd.h>

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
            using namespace std::chrono;
            int timestamp_origin = duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();

            std::cerr << "Nice value: " << nice(-20) << std::endl;
			_running = true;
			while(_running)
			{
                for(auto& handle : _handles) handle(_time.load());
                // Attente passive : std::this_thread::sleep_for(_step);

                // Attente active :
                long timestamp{};
                while(true)
                {
                    timestamp = duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
                    if(timestamp - timestamp_origin >= _time.load() + _step.count())
                        break;
                }

                _time.store(timestamp - timestamp_origin);
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
        const std::chrono::milliseconds _step{std::chrono::milliseconds(1)};

		std::vector<handler_type> _handles{};

		std::thread _runThread;
};
