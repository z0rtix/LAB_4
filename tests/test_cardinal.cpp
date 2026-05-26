#include "tests.h"
#include "../Cardinal.h"
#include <iostream>
#include <stdexcept>

void TestCardinal() {
    std::cout << "=========================================\n";
    std::cout << "TESTS FOR Cardinal\n";

    Cardinal c0 = Cardinal::Finite(5);
    if (c0.IsFinite() && c0.GetFiniteValue() == 5)
        std::cout << "✅ Finite(5) works\n";
    else
        std::cout << "❌ Finite(5) FAILED\n";

    Cardinal c1 = Cardinal::Infinite();
    if (c1.IsInfinite() && c1.GetOmegaCoeff() == 1 && c1.GetFinitePart() == 0)
        std::cout << "✅ Infinite works\n";
    else
        std::cout << "❌ Infinite FAILED\n";

    Cardinal c2 = Cardinal::InfinitePlus(10);
    if (c2.IsInfinite() && c2.GetOmegaCoeff() == 1 && c2.GetFinitePart() == 10)
        std::cout << "✅ InfinitePlus(10) works\n";
    else
        std::cout << "❌ InfinitePlus(10) FAILED\n";

    Cardinal c3 = Cardinal::Omega(2, 5);
    if (c3.IsInfinite() && c3.GetOmegaCoeff() == 2 && c3.GetFinitePart() == 5)
        std::cout << "✅ Omega(2,5) works\n";
    else
        std::cout << "❌ Omega(2,5) FAILED\n";

    if (c1 == Cardinal::Infinite()) std::cout << "✅ Equality\n";
    else std::cout << "❌ Equality FAILED\n";

    if (c1 < c2) std::cout << "✅ Infinite < InfinitePlus\n";
    else std::cout << "❌ Infinite < InfinitePlus FAILED\n";

    Cardinal sum1 = c1 + Cardinal::Finite(3);
    if (sum1.IsInfinite() && sum1.GetFinitePart() == 3)
        std::cout << "✅ Infinite + 3\n";
    else std::cout << "❌ Infinite + 3 FAILED\n";

    Cardinal sum2 = c2 + c3;
    if (sum2.IsInfinite() && sum2.GetOmegaCoeff() == 3 && sum2.GetFinitePart() == 15)
        std::cout << "✅ InfinitePlus + Omega\n";
    else std::cout << "❌ InfinitePlus + Omega FAILED\n";

    Cardinal diff = c2 - size_t(5);
    if (diff.IsInfinite() && diff.GetFinitePart() == 5)
        std::cout << "✅ InfinitePlus - 5\n";
    else std::cout << "❌ InfinitePlus - 5 FAILED\n";

    try {
        Cardinal::Finite(3) - size_t(5);
        std::cout << "❌ Expected exception on negative finite\n";
    } catch (const std::logic_error&) {
        std::cout << "✅ Exception on negative finite\n";
    }

    try {
        Cardinal::InfinitePlus(0) - size_t(1);
        std::cout << "❌ Expected exception on negative part of infinite\n";
    } catch (const std::logic_error&) {
        std::cout << "✅ Exception on negative part of infinite\n";
    }

    std::cout << "=========================================\n\n";
}