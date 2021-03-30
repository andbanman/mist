#include "algorithm/CompletionTupleProducer.hpp"
#include "Variable.hpp"

using namespace mist;
using namespace mist::algorithm;

void CompletionTupleProducer::start_d2() {
    int nvar = this->nvar;
    int size = this->tuple_size;

    Variable::indexes tuple(size);

    for (int ii = 0; ii < nvar - 1; ii++) {
        tuple[0] = ii;
        tuple[1] = ii + 1;
        this->queue->push(tuple);
    }
}

void CompletionTupleProducer::start_d3() {
    int nvar = this->nvar;
    int size = this->tuple_size;

    Variable::indexes tuple(size);

    for (int ii = 0; ii < nvar - 2; ii++) {
        tuple[0] = ii;
        for (int jj = ii + 1; jj < nvar - 1; jj++) {
            tuple[1] = jj;
            tuple[2] = jj + 1;
            this->queue.get()->push(tuple);
        }
    }
}


void CompletionTupleProducer::start() {
    int size = this->tuple_size;
    switch (size) {
    case 2: this->start_d2(); break;
    case 3: this->start_d3(); break;
    default:
        throw CompletionTupleProducerException("start", "Unsupported tuple size " + std::to_string(size) + ", valid range [2,3]");
    }
    this->queue.get()->finish();
}

void CompletionTupleProducer::registerConsumer(TupleConsumer & consumer) {
    try {
        auto completionconsumer = dynamic_cast<CompletionTupleConsumer*>(&consumer);
        completionconsumer->set_queue(this->queue);
    } catch (std::bad_cast &e) {
        throw CompletionTupleProducerException("registerConsumer", "Failed to cast TupleConsumer to CompletionTupleConsumer");
    }
}
