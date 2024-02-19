#include <iostream>
#include <thread>
#include <vector>
#include "logger.h"

// netcat -l -p 1234 -u to test server side

void threadFunc(Logger& logger, int threadID)
{
    for (int i = 0; i < 5; ++i) {
        logger.log("Message from thread " + std::to_string(threadID));
    }
}

int main()
{
    // Create logger objects
    std::unique_ptr<Logger> consoleLogger = std::make_unique<ConsoleLogger>();
    std::unique_ptr<Logger> fileLogger = std::make_unique<FileLogger>("log.txt");
    std::unique_ptr<Logger> remoteLogger = std::make_unique<RemoteLogger>("127.0.0.1", 1234);

    // Create threads for each logger
    std::vector<std::thread> threads;

    for (int i = 1; i <= 5; ++i) {
        threads.emplace_back(threadFunc, std::ref(*consoleLogger), i);
        threads.emplace_back(threadFunc, std::ref(*fileLogger), i);
        threads.emplace_back(threadFunc, std::ref(*remoteLogger), i);
    }

    // Join threads
    for (auto& thread : threads) {
        thread.join();
    }

	// Sleeping to allow detached thread to complete its execution
	std::this_thread::sleep_for(std::chrono::seconds(5));

    return 0;
}

