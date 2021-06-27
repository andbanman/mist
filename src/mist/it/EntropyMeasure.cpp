#include <stdexcept>

#include "it/EntropyMeasure.hpp"

using namespace mist;
using namespace mist::it;

using sub1 = EntropyMeasure::sub_calc_1d;
using sub2 = EntropyMeasure::sub_calc_2d;
using sub3 = EntropyMeasure::sub_calc_3d;

static EntropyMeasure::result_type compute_1d(EntropyCalculator & ecalc,
        Variable::indexes const& vars) {
    EntropyMeasure::result_type res((std::size_t) sub1::size);
    res[(int) sub1::entropy0] = ecalc.entropy({vars[0]});
    return res;
}

static EntropyMeasure::result_type compute_2d(EntropyCalculator & ecalc,
        Variable::indexes const& vars) {
    EntropyMeasure::result_type res((std::size_t) sub2::size);
    res[(int) sub2::entropy01] = ecalc.entropy({vars[0], vars[1]});
    return res;
}

static EntropyMeasure::result_type compute_3d(EntropyCalculator & ecalc,
        Variable::indexes const& vars) {
    EntropyMeasure::result_type res((std::size_t) sub3::size);
    res[(int) sub3::entropy012] = ecalc.entropy({vars[0], vars[1], vars[2]});
    return res;
}

EntropyMeasure::result_type EntropyMeasure::compute(EntropyCalculator &ecalc, Variable::indexes const& tuple) const
{
    EntropyMeasure::result_type ret;
    auto size = tuple.size();
    switch (size) {
        case 1: ret = compute_1d(ecalc, tuple); break;
        case 2: ret = compute_2d(ecalc, tuple); break;
        case 3: ret = compute_3d(ecalc, tuple); break;
        default:
            throw EntropyMeasureException("compute", "Unsupported tuple size " + std::to_string(size) + ", valid range [1,3]");
    }
    return ret;
}

EntropyMeasure::result_type EntropyMeasure::compute(EntropyCalculator &ecalc, Variable::indexes const& tuple, Entropy const& e) const
{
    return compute(ecalc, tuple);
}

std::string EntropyMeasure::header(int d, bool full_output) const {
    std::string h;
    switch (d) {
        case 1: h = "v0,entropy0"; break;
        case 2: h = "v0,v1,entropy01"; break;
        case 3: h = "v0,v1,v2,entropy012"; break;
    }
    return h;
}
