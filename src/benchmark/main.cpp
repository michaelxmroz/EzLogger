#include "..\Logger.h"

#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <atomic>
#include <windows.h>

constexpr int NUM_PRODUCERS = 16;
constexpr int WARMUP_MS = 500;
constexpr int BENCHMARK_MS = 1000;

void producer(std::atomic<size_t>& counter, std::atomic<bool>& running) 
{
    while (running.load(std::memory_order_relaxed)) 
	{
        LOG_ERROR("This is a message of average length, but lets make it a bit longer. Sometimes there are recommendations to resolve the error.");
        counter.fetch_add(1, std::memory_order_relaxed);
    }
}

int main() 
{
	std::vector<std::thread> producers;
    std::atomic<size_t> message_count{0};
    std::atomic<bool> running{true};

    Logger::FileOutput::Init("log.txt");

    // Start producers
    for (int i = 0; i < NUM_PRODUCERS; ++i) {
        producers.emplace_back(producer, std::ref(message_count), std::ref(running));
    }

    // Warmup period
    std::this_thread::sleep_for(std::chrono::milliseconds(WARMUP_MS));
    message_count.store(0, std::memory_order_relaxed);

    // Benchmark period
    auto start = std::chrono::high_resolution_clock::now();
    std::this_thread::sleep_for(std::chrono::milliseconds(BENCHMARK_MS));
    auto end = std::chrono::high_resolution_clock::now();
    running.store(false, std::memory_order_relaxed);

    // Join producers
    for (auto& thread : producers) thread.join();

    // Calculate throughput
    size_t total_messages = message_count.load(std::memory_order_relaxed);
    double duration = std::chrono::duration<double>(end - start).count();
    std::cout << "Throughput: " << (total_messages / duration) << " messages/sec" << std::endl;
	
    return 0;
}