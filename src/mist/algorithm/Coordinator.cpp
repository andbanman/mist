#include "algorithm/Coordinator.hpp"

using namespace mist;
using namespace mist::algorithm;

void Coordinator::start_single() {
    // push all tuples at once
    // TODO throw if cannot fit them all
    producer->start();
    // process all tuples
    consumers.front()->start();
}

void Coordinator::start_multi() {
    auto num_consumers = consumers.size();
    std::thread threads[num_consumers];

    // start consumers
    for (std::size_t ii = 0; ii < num_consumers; ii++)
        threads[ii] = std::thread(&TupleConsumer::start, consumers[ii]);

    // produce tuples
    producer->start(); //returns when all tuples pushed

    // join consumers
    for (auto& thread : threads)
        thread.join();
}

Coordinator::Coordinator(producer_ptr const& producer, consumer_ptr const& consumer) :
    producer(producer),
    consumers({consumer})
{
    producer->registerConsumer(*consumers.front());
};

Coordinator::Coordinator(producer_ptr const& producer, std::vector<consumer_ptr> const& consumers) :
    producer(producer),
    consumers(consumers)
{
    for (auto& c : this->consumers)
        producer->registerConsumer(*c);
};

void Coordinator::start() {
    //TODO: threaded 1 producer and 1 consumer not possible
    if (consumers.size() > 1)
        start_multi();
    else
        start_single();
};
