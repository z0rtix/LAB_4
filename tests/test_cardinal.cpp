#include "tests.h"

#include "../Cardinal.h"

#include <cassert>
#include <iostream>


void TestCardinal() {
    Cardinal c = Cardinal::Finite(5);
    assert(c.IsFinite());
    assert(c.GetFiniteValue() == 5);

    Cardinal inf = Cardinal::Infinite();
    assert(inf.IsInfinite());
    assert(inf.GetOmegaCoeff() == 1);
    assert(inf.GetFinitePart() == 0);

    Cardinal inf10 = Cardinal::InfinitePlus(10);
    assert(inf10.IsInfinite());
    assert(inf10.GetFinitePart() == 10);
    assert(inf10.GetOmegaCoeff() == 1);

    Cardinal w25 = Cardinal::Omega(2, 5);
    assert(w25.IsInfinite());
    assert(w25.GetOmegaCoeff() == 2);
    assert(w25.GetFinitePart() == 5);

    assert(inf == Cardinal::Infinite());
    assert(inf < inf10);

    Cardinal sum = inf + Cardinal::Finite(3);
    assert(sum.IsInfinite());
    assert(sum.GetFinitePart() == 3);

    Cardinal sum2 = inf10 + w25;
    assert(sum2.IsInfinite());
    assert(sum2.GetOmegaCoeff() == 3);
    assert(sum2.GetFinitePart() == 15);

    Cardinal diff = inf10 - size_t(5);
    assert(diff.IsInfinite());
    assert(diff.GetFinitePart() == 5);

    bool caught = false;
    try { Cardinal::Finite(3) - size_t(5); }
    catch (const std::logic_error&) { caught = true; }
    assert(caught);

    caught = false;
    try { Cardinal::InfinitePlus(0) - size_t(1); }
    catch (const std::logic_error&) { caught = true; }
    assert(caught);

    std::cout << "✅ TESTS FOR Cardinal\n\n";
}