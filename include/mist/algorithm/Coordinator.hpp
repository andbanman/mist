#pragma once

#include <memory>
#include <thread>
#include <vector>

#include "TupleProducer.hpp"
#include "TupleConsumer.hpp"
#include "BatchTupleProducer.hpp"
#include "BatchTupleConsumer.hpp"
#include "CompletionTupleProducer.hpp"
#include "CompletionTupleConsumer.hpp"

namespace mist {
namespace algorithm {

class Coordinator {
private:
    using producer_type = TupleProducer;
    using producer_ptr  = std::shared_ptr<producer_type>;
    using consumer_type = TupleConsumer;
    using consumer_ptr  = std::shared_ptr<consumer_type>;
    producer_ptr producer;
    std::vector<consumer_ptr> consumers;
    void start_single();
    void start_multi();

public:
    Coordinator(producer_ptr const& producer, consumer_ptr const& consumer);
    Coordinator(producer_ptr const& producer, std::vector<consumer_ptr> const& consumers);
    void start();
};

} // algorithm
} // mist
