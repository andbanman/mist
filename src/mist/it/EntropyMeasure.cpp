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
    auto e0 = ecalc.entropy({vars[0]});
    res[(int) sub1::entropy0]  = e0;
    return res;
}

static EntropyMeasure::result_type compute_2d(EntropyCalculator & ecalc,
        Variable::indexes const& vars) {
    EntropyMeasure::result_type res((std::size_t) sub2::size);

    auto e01  = ecalc.entropy({vars[0], vars[1]});
    res[(int) sub2::entropy01] = e01;

    return res;
}

static void recompute_1d(EntropyCalculator & ecalc,
        Variable::indexes const& vars, EntropyMeasure::result_type & sub) {
    auto e0 = ecalc.entropy({vars[0]});
    sub[(int) sub1::entropy0] = e0;
}

static void recompute_2d(EntropyCalculator & ecalc,
        Variable::indexes const& vars, EntropyMeasure::result_type & sub) {
    auto e01  = ecalc.entropy({vars[0], vars[1]});
    sub[(int) sub2::entropy01] = e01;
}

static EntropyMeasure::result_type compute_3d(EntropyCalculator & ecalc,
        Variable::indexes const& vars) {
    EntropyMeasure::result_type res((std::size_t) sub3::size);

    auto e012 = ecalc.entropy({vars[0], vars[1], vars[2]});
    res[(int) sub3::entropy012] = e012;
    return res;
}

static void recompute_3d(EntropyCalculator & ecalc, Variable::indexes const& vars,
        EntropyMeasure::result_type & sub) {

    auto e012 = ecalc.entropy({vars[0], vars[1], vars[2]});
    sub[(int) sub3::entropy012]   = e012;
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

void EntropyMeasure::recomputeLastIndex(EntropyCalculator &ecalc, Variable::indexes const& tuple,
                                  EntropyMeasure::result_type & sub) const
{
    auto size = tuple.size();
    switch (size) {
        case 1: recompute_1d(ecalc, tuple, sub); break;
        case 2: recompute_2d(ecalc, tuple, sub); break;
        case 3: recompute_3d(ecalc, tuple, sub); break;
        default:
            throw EntropyMeasureException("recomputeLastIndex", "Unsupported tuple size " + std::to_string(size) + ", valid range [1,3]");
            break;
    }
}
