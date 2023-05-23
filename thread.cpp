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
int data = 0;

void write(int size) {
    for(int i=0; i<size; i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::unique_lock<std::mutex> lock(mutex_);
        std::cout << "Enter data: ";
        std::cin >> data;
        queue_.push(data);
        cv_.notify_one();
    }
    isFinished = true;
}

void read() {
    std::unique_lock<std::mutex> lock(mutex_);
    while (true) {
        cv_.wait(lock, [&] { return !queue_.empty() || isFinished.load(); });

        while (!queue_.empty()) {
            data = queue_.front();
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


int main()
{
    int size = 0;
    std::cout << "Enter the queue size: ";
    std::cin >> size;
    std::thread writer(write, size);
    std::thread reader(read);
    writer.join();
    reader.join();
    return 0;
}
