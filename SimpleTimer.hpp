#ifndef SIMPLE_TIMER_HPP
#define SIMPLE_TIMER_HPP

#include<iostream>
#include<chrono>
#include<string_view>

namespace my
{
	template<class T = std::chrono::milliseconds>
	class SimpleTimer
	{
	private:
	    std::chrono::steady_clock::time_point begin_time_;
	    std::chrono::steady_clock::time_point end_time_;
	    bool stopped_{true};

	public:
	    explicit SimpleTimer(bool stopped = true) 
		: 	begin_time_(std::chrono::steady_clock::now()), 
	        stopped_(stopped){}

	    inline T elapsed_time()
	    {
	        if(!stopped_) end_time_ = std::chrono::steady_clock::now();
	        return std::chrono::duration_cast<T>(end_time_ - begin_time_);
	    }

	    inline void start()
	    {
	        stopped_ = false;
	        begin_time_ = std::chrono::steady_clock::now();
	    }

	    inline void stop()
	    {
	        end_time_ = std::chrono::steady_clock::now();
	        stopped_ = true;
	    }
	};

}

#endif // SIMPLE_TIMER_HPP