#include <iostream>
#include <queue>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

std::queue<int> queue_;
std::mutex mutex_;
std::condition_variable cv_;
std::atomic<bool> isFinished(false);
int data;

void write(int size) {
    for (int i = 0; i < size; i++)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::lock_guard<std::mutex> lock(mutex_);
        std::cout << "Enter Data: ";
        std::cin >> data;
        queue_.push(std::move(data));
        cv_.notify_one();
    }
    isFinished = true;
    cv_.notify_one();
}
void read() {
    std::unique_lock<std::mutex> lock(mutex_);
    while (true) {
        cv_.wait(lock, [] { return !queue_.empty() || isFinished.load(); });

        while (!queue_.empty()) {
            int data = queue_.front();
            queue_.pop();
            lock.unlock();
            std::cout << "Read Data: " << data << std::endl;
            lock.lock();
        }

        if (isFinished.load() && queue_.empty()) {
            break;
        }
    }
}


int main() {
    int size;
    std::cout << "Enter the size of queue: ";
    std::cin >> size;

    std::thread writeThread(write, size);
    std::thread readThread(read);
    writeThread.join();
    readThread.join();

    return 0;
}
