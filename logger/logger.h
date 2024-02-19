#pragma once

#include <iostream>
#include <fstream>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <boost/asio.hpp>
#include <queue>
//#include <iomanip>

// Downsides:
// - no log levels
// - no log rotation
// - very inefficient (locking the q and timestamp generation)

class Logger
{
private:
	std::queue<std::string> q;
	std::condition_variable cv;
	std::mutex mutex;
	bool stop = false;

protected:
	virtual void doLog(const std::string& message) = 0;

	void loggingThread()
	{
		while (true) {
			// Acquire lock
			std::unique_lock<std::mutex> lock(mutex);

			cv.wait(lock, [this](){ return !q.empty() || stop; });

			if (stop && q.empty()) {
				return;
			}

			doLog(q.front());
			q.pop();
		}
	}

	std::string getTimestamp()
	{
		using namespace std::chrono;

		// When std::chrono::format will be implemented in libc++:
		//return format(std::format("{:%Y-%m-%d %H:%M:%S}.%N", system_clock::now()));

		// But until then we suffer
		auto now = system_clock::now();
		auto now_time_t = system_clock::to_time_t(now);
		auto now_tm = std::gmtime(&now_time_t);
		auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();

		// Return the formatted timestamp
		std::stringstream ss;
    	ss << std::put_time(now_tm, "%Y-%m-%d %H:%M:%S.") << (nanoseconds % 1000000000) << "\t";

		// RVO
    	return ss.str();
	}

public:
	Logger()
	{
		std::thread([this] { loggingThread(); }).detach();
	}

    void log(const std::string& message)
	{
		// Acquire lock
		std::lock_guard<std::mutex> lock(mutex);

		q.push(getTimestamp() + message);
		cv.notify_one();
	}

    virtual ~Logger()
	{
		stop = true;
		cv.notify_one();
	}
};

// Console logger
class ConsoleLogger : public Logger
{
private:
	void doLog(const std::string& message) override final
	{
		std::cout << message << std::endl;
	}
};

// File logger
class FileLogger : public Logger
{
private:
    std::ofstream file;

	void doLog(const std::string& message) override final
	{
		file << message << std::endl;
	}

public:
    FileLogger(const std::string& filename)
	{
        file.open(filename, std::ios_base::app);
    }

    ~FileLogger()
	{
        file.close();
    }
};

// Remote logger
class RemoteLogger : public Logger
{
private:
    std::string serverAddress;
	boost::asio::io_context ioContext;
    boost::asio::ip::udp::socket socket;
    boost::asio::ip::udp::endpoint ep;

	void doLog(const std::string& message) override final
	{
		socket.send_to(boost::asio::buffer(message + "\n"), ep);
	}

public:
	RemoteLogger(const std::string& serverAddress, unsigned short serverPort) :
		socket(ioContext),
		ep(boost::asio::ip::address::from_string(serverAddress), serverPort)
	{
        socket.open(boost::asio::ip::udp::v4());
    }

    ~RemoteLogger()
	{
        socket.close();
    }
};

