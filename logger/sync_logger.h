#pragma once

#include <iostream>
#include <fstream>
#include <memory>
#include <mutex>
#include <boost/asio.hpp>
//#include <iomanip>

// Abstract logger interface
class Logger
{
protected:
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
    virtual void log(const std::string& message) = 0;
    virtual ~Logger() {}
};

// Console logger
class ConsoleLogger : public Logger
{
private:
	std::mutex mutex;

public:
    void log(const std::string& message) override
	{
		std::lock_guard<std::mutex> lock(mutex);
        std::cout << "[Console] " << getTimestamp() << message << std::endl;
    }
};

// File logger
class FileLogger : public Logger
{
private:
    std::ofstream file;
	std::mutex mutex;

public:
    FileLogger(const std::string& filename)
	{
        file.open(filename, std::ios_base::app);
    }

    ~FileLogger()
	{
        file.close();
    }

    void log(const std::string& message) override
	{
		std::lock_guard<std::mutex> lock(mutex);
        file << "[File] " << getTimestamp() << message << std::endl;
    }
};

// Remote logger
class RemoteLogger : public Logger
{
private:
    std::string serverAddress;
	boost::asio::io_context ioContext;
    boost::asio::ip::udp::socket socket;
    boost::asio::ip::udp::endpoint remoteEndpoint;
	std::mutex mutex;

public:
	RemoteLogger(const std::string& serverAddress, unsigned short serverPort) :
		socket(ioContext),
		remoteEndpoint(boost::asio::ip::address::from_string(serverAddress), serverPort)
	{
        socket.open(boost::asio::ip::udp::v4());
    }

    ~RemoteLogger()
	{
        socket.close();
    }

    void log(const std::string& message) override
	{
		std::lock_guard<std::mutex> lock(mutex);
        socket.send_to(boost::asio::buffer(getTimestamp() + message + "\n"), remoteEndpoint);
    }
};

