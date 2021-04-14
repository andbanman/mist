#include "algorithm/ExhaustiveTupleConsumer.hpp"
#include "it/Measure.hpp"

using namespace mist;
using namespace mist::algorithm;

void ExhaustiveTupleConsumer::start() {

    int nvar = this->nvar;
    Variable::indexes tuple(nvar); // TODO type name

    while (this->queue->check_and_pop(tuple)) {
        // initial computation
        auto result = this->measure->compute(*this->calculator, tuple);
        if (this->out) {
            if (this->output_all) {
                this->out->push(tuple, result);
            } else {
                this->out->push(tuple, it::Measure::result_type(result.end() - 1, result.end()));
            }
        }
        tuple.back()++;

        while (tuple.back() < nvar) {
            // do completion
            this->measure->recomputeLastIndex(*this->calculator, tuple, result);
            if (this->out) {
                if (this->output_all) {
                    this->out->push(tuple, result);
                } else {
                    this->out->push(tuple, it::Measure::result_type(result.end() - 1, result.end()));
                }
            }
            // increment tuple
            tuple.back()++;
        }
    }
}
