#include "algorithm/ExhaustiveTupleProducer.hpp"
#include "algorithm/ExhaustiveTupleConsumer.hpp"
#include "algorithm/BatchTupleConsumer.hpp"
#include "Variable.hpp"

using namespace mist;
using namespace mist::algorithm;

ExhaustiveTupleProducer::ExhaustiveTupleProducer(int tuple_size, int nvar, algorithm alg)
    : TupleProducer(tuple_size, alg), nvar(nvar) {
    this->compl_queue = compl_queue_ptr(new ExhaustiveQueue);
    this->batch_queue = batch_queue_ptr(new BatchQueue);
};

ExhaustiveTupleProducer::ExhaustiveTupleProducer(int tuple_size, int nvar)
    : TupleProducer(tuple_size, algorithm::completion), nvar(nvar) {
    this->compl_queue = compl_queue_ptr(new ExhaustiveQueue);
    this->batch_queue = batch_queue_ptr(new BatchQueue);
};

void ExhaustiveTupleProducer::start_batch_d1() {
    int nvar = this->nvar;
    int size = this->tuple_size;
    int batch_size = this->batch_size;
    std::vector<Variable::indexes> batch;
    Variable::indexes tuple(size);
    for (int ii = 0; ii < nvar; ii++) {
        tuple[0] = ii;
        batch.push_back(tuple);
        if (batch.size() == batch_size) {
            this->batch_queue->push(batch);
            batch.clear();
        }
    }
    if (!batch.empty())
        this->batch_queue->push(batch);
}

void ExhaustiveTupleProducer::start_batch_d2() {
    int nvar = this->nvar;
    int size = this->tuple_size;
    int batch_size = this->batch_size;
    std::vector<Variable::indexes> batch;
    Variable::indexes tuple(size);
    for (int ii = 0; ii < nvar - 1; ii++) {
        tuple[0] = ii;
        for (int jj = ii + 1; jj < nvar; jj++) {
            tuple[1] = jj;
            batch.push_back(tuple);
            if (batch.size() == batch_size) {
                this->batch_queue->push(batch);
                batch.clear();
            }
        }
    }
    if (!batch.empty())
        this->batch_queue->push(batch);
}

void ExhaustiveTupleProducer::start_batch_d3() {
    int nvar = this->nvar;
    int size = this->tuple_size;
    int batch_size = this->batch_size;
    std::vector<Variable::indexes> batch;
    Variable::indexes tuple(size);
    for (int ii = 0; ii < nvar - 2; ii++) {
        tuple[0] = ii;
        for (int jj = ii + 1; jj < nvar - 1; jj++) {
            tuple[1] = jj;
            for (int kk = jj + 1; kk < nvar; kk++) {
                tuple[2] = kk;
                batch.push_back(tuple);
                if (batch.size() == batch_size) {
                    this->batch_queue->push(batch);
                    batch.clear();
                }
            }
        }
    }
    if (!batch.empty())
        this->batch_queue->push(batch);
}

void ExhaustiveTupleProducer::start_compl_d2() {
    int nvar = this->nvar;
    int size = this->tuple_size;
    Variable::indexes tuple(size);
    for (int ii = 0; ii < nvar - 1; ii++) {
        tuple[0] = ii;
        tuple[1] = ii + 1;
        this->compl_queue->push(tuple);
    }
}

void ExhaustiveTupleProducer::start_compl_d3() {
    int nvar = this->nvar;
    int size = this->tuple_size;
    Variable::indexes tuple(size);
    for (int ii = 0; ii < nvar - 2; ii++) {
        tuple[0] = ii;
        for (int jj = ii + 1; jj < nvar - 1; jj++) {
            tuple[1] = jj;
            tuple[2] = jj + 1;
            this->compl_queue.get()->push(tuple);
        }
    }
}

void ExhaustiveTupleProducer::start() {
    int size = this->tuple_size;
    switch (this->alg) {
    case algorithm::batch:
        switch (size) {
        case 1: this->start_batch_d1(); break;
        case 2: this->start_batch_d2(); break;
        case 3: this->start_batch_d3(); break;
        default:
            throw ExhaustiveTupleProducerException("start", "Unsupported tuple size " + std::to_string(size) + ", valid range [1,3]");
        }
        this->batch_queue->finish();
        break;
    case algorithm::completion:
        switch (size) {
        case 2: this->start_compl_d2(); break;
        case 3: this->start_compl_d3(); break;
        default:
            throw ExhaustiveTupleProducerException("start", "Unsupported tuple size " + std::to_string(size) + ", valid range [2,3]");
        }
        this->compl_queue->finish();
        break;
    default:
        throw ExhaustiveTupleProducerException("start", "Unknown TupleProducer::algorithm type");
    }
}

void ExhaustiveTupleProducer::registerConsumer(TupleConsumer & consumer) {
    switch (this->alg) {
    case algorithm::batch:
        try {
            auto batchconsumer = dynamic_cast<BatchTupleConsumer*>(&consumer);
            if (!batchconsumer)
                throw ExhaustiveTupleProducerException("registerConsumer", "Failed to cast TupleConsumer to BatchTupleConsumer");
            batchconsumer->set_queue(this->batch_queue);
        } catch (std::bad_cast &e) {
            throw ExhaustiveTupleProducerException("registerConsumer", "Failed to cast TupleConsumer to BatchTupleConsumer");
        }
        break;
    case algorithm::completion:
        try {
            auto completionconsumer = dynamic_cast<ExhaustiveTupleConsumer*>(&consumer);
            if (!completionconsumer)
                throw ExhaustiveTupleProducerException("registerConsumer", "Failed to cast TupleConsumer to ExhaustiveTupleConsumer");
            completionconsumer->set_queue(this->compl_queue);
        } catch (std::bad_cast &e) {
            throw ExhaustiveTupleProducerException("registerConsumer", "Failed to cast TupleConsumer to ExhaustiveTupleConsumer");
        }
        break;
    default:
        throw ExhaustiveTupleProducerException("registerConsumer", "Unknown TupleProducer::algorithm type");
    }
}
