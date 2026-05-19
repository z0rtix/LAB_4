#include "tests.h"

#include "../LazySequence.h"
#include "../Cardinal.h"

#include <iostream>
#include <chrono>
#include <stdexcept>


template <class T>
void TestLazySequence(const char* typeName) {
    using namespace std::chrono;
    std::cout << "=========================================\n";
    std::cout << "✅ TESTS FOR LazySequence<" << typeName << ">\n";

    LazySequence<T> emptySeq;
    if (emptySeq.isEmpty() && emptySeq.getLength() == 0)
        std::cout << "✅ Empty sequence\n";
    else
        std::cout << "❌ Empty sequence FAILED\n";

    T arr[] = {T(1), T(2), T(3)};
    LazySequence<T> fromArray(arr, 3);
    if (fromArray.getLength() == 3 && fromArray.get(0) == T(1) && fromArray.get(2) == T(3))
        std::cout << "✅ From array\n";
    else
        std::cout << "❌ From array FAILED\n";

    if (fromArray.getFirst() == T(1) && fromArray.getLast() == T(3))
        std::cout << "✅ getFirst/getLast\n";
    else
        std::cout << "❌ getFirst/getLast FAILED\n";

    LazySequence<T> *tmp = fromArray.append(T(4));
    if (tmp->getLength() == 4 && tmp->get(3) == T(4))
        std::cout << "✅ append\n";
    else
        std::cout << "❌ append FAILED\n";
    delete tmp;

    tmp = fromArray.prepend(T(0));
    if (tmp->getLength() == 4 && tmp->get(0) == T(0))
        std::cout << "✅ prepend\n";
    else
        std::cout << "❌ prepend FAILED\n";
    delete tmp;

    tmp = fromArray.insertAt(T(99), 1);
    if (tmp->getLength() == 4 && tmp->get(1) == T(99))
        std::cout << "✅ insertAt\n";
    else
        std::cout << "❌ insertAt FAILED\n";
    delete tmp;

    tmp = fromArray.set(T(100), 1);
    if (tmp->get(1) == T(100))
        std::cout << "✅ set\n";
    else
        std::cout << "❌ set FAILED\n";
    delete tmp;

    tmp = fromArray.getSubsequence(0, 1);
    if (tmp->getLength() == 2 && tmp->get(0) == T(1) && tmp->get(1) == T(2))
        std::cout << "✅ getSubsequence\n";
    else
        std::cout << "❌ getSubsequence FAILED\n";
    delete tmp;

    if constexpr (std::is_same<T, int>::value) {
        MutableArraySequence<int> init;
        init.append(1); init.append(1);

        auto fib = [](const Sequence<int>& s) -> int {
            return s.get(s.getLength()-1) + s.get(s.getLength()-2);
        };

        LazySequence<int> fibSeq(fib, init, Cardinal::Infinite());

        if (fibSeq.get(0) == 1 && fibSeq.get(1) == 1 && fibSeq.get(2) == 2 && fibSeq.get(5) == 8)
            std::cout << "✅ Infinite Fibonacci\n";
        else
            std::cout << "❌ Infinite Fibonacci FAILED\n";

        try {
            fibSeq.getLast();
            std::cout << "❌ Exception expected on getLast of infinite\n";
        } catch (const std::logic_error&) {
            std::cout << "✅ Exception on getLast of infinite\n";
        }

        try {
            fibSeq.getLength();
            std::cout << "❌ Exception expected on getLength of infinite\n";
        } catch (const std::logic_error&) {
            std::cout << "✅ Exception on getLength of infinite\n";
        }

        try {
            fibSeq.append(T(0));
            std::cout << "❌ Exception expected on append to infinite\n";
        } catch (const std::logic_error&) {
            std::cout << "✅ Exception on append to infinite\n";
        }

        size_t matBefore = fibSeq.getMaterializedCount();
        fibSeq.get(100);
        if (fibSeq.getMaterializedCount() > matBefore)
            std::cout << "✅ Materialization grows\n";
        else
            std::cout << "❌ Materialization didn't grow\n";

        std::cout << "⏳ Stress test (1M Fibonacci numbers)...\n";
        auto start = steady_clock::now();

        volatile T sum = 0;

        for (int i = 0; i < 1000000; i++) {
            sum += fibSeq.get(i);
        }

        auto end = steady_clock::now();
        auto ms = duration_cast<milliseconds>(end - start).count();
        std::cout << "✅ Stress test: " << ms << " ms (sum=" << sum << ")\n";
    }

    LazySequence<T> *copy = fromArray.copy();

    if (copy->getLength() == fromArray.getLength() && copy->get(0) == fromArray.get(0)) {
        std::cout << "✅ copy\n";
    } else {
        std::cout << "❌ copy FAILED\n";
    }

    delete copy;

    T arr2[] = {T(10), T(20)};
    LazySequence<T> other(arr2, 2);
    LazySequence<T> *conc = fromArray.concat(&other);

    if (conc->getLength() == 5 && conc->get(3) == T(10)) {
        std::cout << "✅ concat\n";
    } else {
        std::cout << "❌ concat FAILED\n";
    }
    
    delete conc;

    LazySequence<T> *cleared = fromArray.clear();
    if (cleared->isEmpty()) {
        std::cout << "✅ clear\n";
    } else {
        std::cout << "❌ clear FAILED\n";
    }
    
    delete cleared;

    LazySequence<T> remSeq(arr, 3);
    LazySequence<T> *rem = remSeq.removeFirst();

    if (rem->getLength() == 2 && rem->get(0) == T(2)) {
        std::cout << "✅ removeFirst\n";
    } else {
        std::cout << "❌ removeFirst FAILED\n";
    }
    
    delete rem;

    rem = remSeq.removeLast();
    if (rem->getLength() == 2 && rem->get(1) == T(2)) {
        std::cout << "✅ removeLast\n";
    } else {
        std::cout << "❌ removeLast FAILED\n";
    }
    
    delete rem;

    rem = remSeq.removeAt(1);
    if (rem->getLength() == 2 && rem->get(0) == T(1) && rem->get(1) == T(3)) {
        std::cout << "✅ removeAt\n";
    } else {
        std::cout << "❌ removeAt FAILED\n";
    }
    
    delete rem;

    std::cout << "=========================================\n\n";
}


template void TestLazySequence<int>(const char*);
template void TestLazySequence<double>(const char*);