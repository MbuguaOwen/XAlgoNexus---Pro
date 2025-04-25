#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <memory>
#include "core/messaging/MessageBus.hpp"
#include "core/messaging/Tick.hpp"

using namespace XAlgo;

int main() {
    EventQueue queue;
    std::atomic<bool> running{true};

    // Producer thread: simulates a data source
    std::thread producer([&]() {
        for (int i = 0; i < 10; ++i) {
            Tick tick;
            tick.price = 1.1000 + 0.0001 * i;
            tick.timestamp = std::chrono::high_resolution_clock::now();

            auto event = std::make_shared<MarketEvent>(
                MarketEvent{MarketEventType::TICK, tick.timestamp, tick}
            );

            queue.enqueue(event);
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        running = false;
    });

    // Consumer thread: simulates a downstream module
    std::thread consumer([&]() {
        while (running || queue.size_approx() > 0) {
            std::shared_ptr<MarketEvent> event;
            if (queue.try_dequeue(event)) {
                if (event->type == MarketEventType::TICK) {
                    Tick tick = std::get<Tick>(event->payload);
                    std::cout << "Received Tick: " << tick.price << std::endl;
                }
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
    });

    producer.join();
    consumer.join();

    std::cout << "MessageBus test complete." << std::endl;
    return 0;
}
