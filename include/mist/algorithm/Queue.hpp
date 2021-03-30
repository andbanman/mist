#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>

namespace mist {
namespace algorithm {

template <class Element>
class Queue {
private:
    std::queue<Element> queue;
    std::mutex m;
    std::condition_variable queue_empty;
    std::condition_variable queue_full;
    std::size_t queue_size;
    std::size_t max_queue_size;
    bool finished;

public:
    Queue() : queue_size(0), max_queue_size(-1), finished(false) { };
    Queue(std::size_t max_queue_size) : queue_size(0), max_queue_size(max_queue_size), finished(false) { };

    void push(Element const& ts) {
        std::unique_lock<std::mutex> queue_lock(m);

        if (this->finished)
            return;

        while (this->queue_size == this->max_queue_size)
            this->queue_full.wait(queue_lock);

        this->queue.push(ts);
        this->queue_size++;
        this->queue_empty.notify_one();
    }

    bool check_and_pop(Element &tuple_stream) {
        std::unique_lock<std::mutex> queue_lock(m);

        while (!this->queue_size) {
            if (this->finished)
                return false;
            this->queue_empty.wait(queue_lock);
            // can wake up to an empty queue when producer finishes
            if (!this->queue_size && this->finished)
                return false;
        }

        tuple_stream = queue.front();
        this->queue.pop();
        this->queue_size--;
        queue_full.notify_one();

        return true;
    }

    void finish() {
        std::unique_lock<std::mutex> queue_lock(m);
        this->finished = true;
        queue_empty.notify_all(); // wakeup anybody waiting on the queue
    }
};

} // algorithm
} // mist
