#include "algorithm/BatchTupleConsumer.hpp"

using namespace mist;
using namespace mist::algorithm;

void BatchTupleConsumer::start() {
    std::vector<Variable::indexes> batch; //TODO typename

    while (this->queue->check_and_pop(batch)) {
        for (auto& tuple : batch) {
            auto result = this->measure->compute(*this->calculator, tuple);
            if (this->out) {
                if (this->output_all)
                    this->out->push(tuple, result);
                else
                    this->out->push(tuple, it::Measure::result_type(result.end() - 1, result.end()));
            }
        }
    }
}
