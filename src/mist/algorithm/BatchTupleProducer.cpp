#include "algorithm/BatchTupleProducer.hpp"
#include <vector>

using namespace mist;
using namespace mist::algorithm;

void BatchTupleProducer::start_d1() {
    int nvar = this->nvar;
    int size = this->tuple_size;
    int batchsize = this->batchsize;

    std::vector<Variable::indexes> batch;
    Variable::indexes tuple(size);

    for (int ii = 0; ii < nvar; ii++) {
        tuple[0] = ii;
        batch.push_back(tuple);
        if (batch.size() == batchsize) {
            this->queue->push(batch);
            batch.clear();
        }
    }
    if (!batch.empty())
        this->queue->push(batch);
}

void BatchTupleProducer::start_d2() {
    int nvar = this->nvar;
    int size = this->tuple_size;
    int batchsize = this->batchsize;

    std::vector<Variable::indexes> batch;
    Variable::indexes tuple(size);

    for (int ii = 0; ii < nvar - 1; ii++) {
        tuple[0] = ii;
        for (int jj = ii + 1; jj < nvar; jj++) {
            tuple[1] = jj;
            batch.push_back(tuple);
            if (batch.size() == batchsize) {
                this->queue->push(batch);
                batch.clear();
            }
        }
    }
    if (!batch.empty())
        this->queue->push(batch);
}

void BatchTupleProducer::start_d3() {
    int nvar = this->nvar;
    int size = this->tuple_size;
    int batchsize = this->batchsize;

    std::vector<Variable::indexes> batch;
    Variable::indexes tuple(size);

    for (int ii = 0; ii < nvar - 2; ii++) {
        tuple[0] = ii;
        for (int jj = ii + 1; jj < nvar - 1; jj++) {
            tuple[1] = jj;
            for (int kk = jj + 1; kk < nvar; kk++) {
                tuple[2] = kk;
                batch.push_back(tuple);
                if (batch.size() == batchsize) {
                    this->queue->push(batch);
                    batch.clear();
                }
            }
        }
    }
    if (!batch.empty())
        this->queue->push(batch);
}

void BatchTupleProducer::start() {
    int size = this->tuple_size;
    switch (size) {
    case 1: this->start_d1(); break;
    case 2: this->start_d2(); break;
    case 3: this->start_d3(); break;
    default:
        throw BatchTupleProducerException("start", "Unsupported tuple size " + std::to_string(size) + ", valid range [1,3]");
    }
    this->queue->finish();
}

void BatchTupleProducer::registerConsumer(TupleConsumer & consumer) {
    try {
        auto batchconsumer = dynamic_cast<BatchTupleConsumer*>(&consumer);
        batchconsumer->set_queue(this->queue);
    } catch (std::bad_cast &e) {
        throw BatchTupleProducerException("registerConsumer", "Failed to cast TupleConsumer to BatchTupleConsumer");
    }
}
