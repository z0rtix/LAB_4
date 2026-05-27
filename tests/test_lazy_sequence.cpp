#include "tests.h"

#include "../LazySequence.h"
#include "../Cardinal.h"

#include <iostream>
#include <chrono>
#include <stdexcept>


template <class T>
void TestLazySequence(const char *typeName) {
    using namespace std::chrono;
    std::cout << "=========================================\n";
    std::cout << "TESTS FOR LazySequence<" << typeName << ">\n";

    LazySequence<T> empty;
    if (empty.isEmpty() && empty.getLength() == 0)
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
        std::cout << "✅ append (finite)\n";
    else
        std::cout << "❌ append (finite) FAILED\n";
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

    LazySequence<T> remSeq(arr, 3);
    tmp = remSeq.removeFirst();
    if (tmp->getLength() == 2 && tmp->get(0) == T(2))
        std::cout << "✅ removeFirst\n";
    else
        std::cout << "❌ removeFirst FAILED\n";
    delete tmp;

    tmp = remSeq.removeLast();
    if (tmp->getLength() == 2)
        std::cout << "✅ removeLast\n";
    else
        std::cout << "❌ removeLast FAILED\n";
    delete tmp;

    tmp = remSeq.removeAt(1);
    if (tmp->getLength() == 2 && tmp->get(0) == T(1) && tmp->get(1) == T(3))
        std::cout << "✅ removeAt\n";
    else
        std::cout << "❌ removeAt FAILED\n";
    delete tmp;

    LazySequence<T> *copied = fromArray.copy();
    if (copied->getLength() == fromArray.getLength() && copied->get(0) == fromArray.get(0))
        std::cout << "✅ copy\n";
    else
        std::cout << "❌ copy FAILED\n";
    delete copied;

    tmp = fromArray.getSubsequence(0, 1);
    if (tmp->getLength() == 2 && tmp->get(0) == T(1) && tmp->get(1) == T(2))
        std::cout << "✅ getSubsequence\n";
    else
        std::cout << "❌ getSubsequence FAILED\n";
    delete tmp;

    if constexpr (std::is_same<T, int>::value) {
        MutableArraySequence<int> init;
        init.append(1);
        init.append(1);

        auto fibGen = [](const Sequence<int>& s) -> int {
            return s.get(s.getLength()-1) + s.get(s.getLength()-2);
        };

        LazySequence<int> fib(fibGen, init, Cardinal::Infinite());

        if (fib.get(0) == 1 && fib.get(1) == 1 && fib.get(2) == 2 && fib.get(5) == 8)
            std::cout << "✅ Infinite Fibonacci\n";
        else
            std::cout << "❌ Infinite Fibonacci FAILED\n";

        try {
            fib.getLast();
            std::cout << "❌ Exception expected on getLast of infinite\n";
        } catch (const std::logic_error&) {
            std::cout << "✅ Exception on getLast of infinite\n";
        }

        try {
            fib.getLength();
            std::cout << "❌ Exception expected on getLength of infinite\n";
        } catch (const std::logic_error&) {
            std::cout << "✅ Exception on getLength of infinite\n";
        }

        LazySequence<int> *app1 = fib.append(100);
        LazySequence<int> *app2 = app1->append(200);
        delete app1;

        LazySequence<int> *app3 = app2->append(300);
        delete app2;

        if (app3->get(Cardinal::InfinitePlus(0)) == 100 &&
            app3->get(Cardinal::InfinitePlus(1)) == 200 &&
            app3->get(Cardinal::InfinitePlus(2)) == 300)
            std::cout << "✅ Append to infinite (ordinal indices)\n";
        else
            std::cout << "❌ Append to infinite FAILED\n";

        LazySequence<int> *mapped = app3->map([](int x) { return x * 2; });
        if (mapped->get(0) == 2 && mapped->get(5) == 16 &&
            mapped->get(Cardinal::InfinitePlus(0)) == 200)
            std::cout << "✅ Map\n";
        else
            std::cout << "❌ Map FAILED\n";

        LazySequence<int> finiteSeq(arr, 3);
        LazySequence<int> *filtered = finiteSeq.where([](int x) { return x % 2 != 0; });

        if (filtered->getLength() == 2 && filtered->get(0) == 1 && filtered->get(1) == 3)
            std::cout << "✅ Where\n";
        else
            std::cout << "❌ Where FAILED\n";
        delete filtered;

        MutableArraySequence<int> otherData;
        otherData.append(10);
        otherData.append(20);

        LazySequence<int> otherSeq(otherData);
        LazySequence<int> *concated = app3->concat(&otherSeq);

        if (concated->get(Cardinal::InfinitePlus(3)) == 10 && concated->get(Cardinal::InfinitePlus(4)) == 20)
            std::cout << "✅ Concat with finite sequence\n";
        else
            std::cout << "❌ Concat with finite sequence FAILED\n";
        delete concated;

        MutableArraySequence<int> init2;
        init2.append(99);
        init2.append(99);

        auto constGen = [](const Sequence<int>&) -> int {
            return 99;
        };

        LazySequence<int> constSeq(constGen, init2, Cardinal::Infinite());
        LazySequence<int> *concatInf = fib.concat(&constSeq);

        if (concatInf->get(Cardinal::Infinite()) == 99 &&
            concatInf->get(Cardinal::InfinitePlus(5)) == 99)
            std::cout << "✅ Concat two infinite sequences\n";
        else
            std::cout << "❌ Concat two infinite sequences FAILED\n";
        delete concatInf;

        std::cout << "⏳ Stress test (1M Fibonacci numbers)...\n";
        auto start = steady_clock::now();
        volatile T sum = 0;

        for (int i = 0; i < 1000000; i++)
            sum += fib.get(i);

        auto end = steady_clock::now();
        auto ms = duration_cast<milliseconds>(end - start).count();

        std::cout << "✅ Stress test: " << ms << " ms (sum=" << sum << ")\n";

        delete mapped;
        delete app3;
    }

    std::cout << "=========================================\n\n";
}


template void TestLazySequence<int>(const char*);
template void TestLazySequence<double>(const char*);