#include "tests.h"
#include "../Cardinal.h"

#include <iostream>
#include <stdexcept>


void TestCardinal() {
    std::cout << "=========================================\n";
    std::cout << "✅ TESTS FOR Cardinal\n";

    Cardinal c5 = Cardinal::Finite(5);
    if (c5.IsFinite() && c5.GetValue() == 5) {
        std::cout << "✅ Finite(5) works\n";
    } else {
        std::cout << "❌ Finite(5) FAILED\n";
    }

    Cardinal inf = Cardinal::Infinite();
    if (!inf.IsFinite()) {
        std::cout << "✅ Infinite works\n";
    } else {
        std::cout << "❌ Infinite FAILED\n";
    }
    
    try {
        inf.GetValue();
        std::cout << "❌ Exception expected for GetValue on Infinite\n";
    } catch (const std::logic_error&) {
        std::cout << "✅ Exception on GetValue of Infinite\n";
    }

    std::cout << "=========================================\n\n";
}