#include "tests.h"

#include "../LazySequence.h"
#include "../Cardinal.h"

#include <cassert>
#include <iostream>
#include <chrono>
#include <stdexcept>


template <class T>
void TestLazySequence(const char *typeName) {
    using namespace std::chrono;

    LazySequence<T> empty;
    assert(empty.isEmpty());
    assert(empty.getLength() == 0);

    T arr[] = {T(1), T(2), T(3)};
    LazySequence<T> fromArray(arr, 3);
    assert(fromArray.getLength() == 3);
    assert(fromArray.get(0) == T(1));
    assert(fromArray.get(2) == T(3));
    assert(fromArray.getFirst() == T(1));
    assert(fromArray.getLast() == T(3));

    LazySequence<T> *tmp = fromArray.append(T(4));
    assert(tmp->getLength() == 4);
    assert(tmp->get(3) == T(4));
    delete tmp;

    tmp = fromArray.prepend(T(0));
    assert(tmp->getLength() == 4);
    assert(tmp->get(0) == T(0));
    delete tmp;

    tmp = fromArray.insertAt(T(99), 1);
    assert(tmp->getLength() == 4);
    assert(tmp->get(1) == T(99));
    delete tmp;

    tmp = fromArray.set(T(100), 1);
    assert(tmp->get(1) == T(100));
    delete tmp;

    LazySequence<T> remSeq(arr, 3);
    tmp = remSeq.removeFirst();
    assert(tmp->getLength() == 2);
    assert(tmp->get(0) == T(2));
    delete tmp;

    tmp = remSeq.removeLast();
    assert(tmp->getLength() == 2);
    delete tmp;

    tmp = remSeq.removeAt(1);
    assert(tmp->getLength() == 2);
    assert(tmp->get(0) == T(1));
    assert(tmp->get(1) == T(3));
    delete tmp;

    LazySequence<T> *copied = fromArray.copy();
    assert(copied->getLength() == fromArray.getLength());
    assert(copied->get(0) == fromArray.get(0));
    delete copied;

    tmp = fromArray.getSubsequence(0, 1);
    assert(tmp->getLength() == 2);
    assert(tmp->get(0) == T(1));
    assert(tmp->get(1) == T(2));
    delete tmp;

    if constexpr (std::is_same<T, int>::value) {
        MutableArraySequence<int> init;
        init.append(1);
        init.append(1);
        auto fibGen = [](const Sequence<int>& s) -> int {
            return s.get(s.getLength()-1) + s.get(s.getLength()-2);
        };
        LazySequence<int> fib(fibGen, init, Cardinal::Infinite());
        assert(fib.get(0) == 1);
        assert(fib.get(1) == 1);
        assert(fib.get(2) == 2);
        assert(fib.get(5) == 8);

        bool caught = false;
        try { fib.getLast(); }
        catch (const std::logic_error&) { caught = true; }
        assert(caught);

        caught = false;
        try { fib.getLength(); }
        catch (const std::logic_error&) { caught = true; }
        assert(caught);

        LazySequence<int> *app1 = fib.append(100);
        LazySequence<int> *app2 = app1->append(200);
        delete app1;
        LazySequence<int> *app3 = app2->append(300);
        delete app2;
        assert(app3->get(Cardinal::InfinitePlus(0)) == 100);
        assert(app3->get(Cardinal::InfinitePlus(1)) == 200);
        assert(app3->get(Cardinal::InfinitePlus(2)) == 300);

        LazySequence<int> *mapped = app3->map([](int x) { return x * 2; });
        assert(mapped->get(0) == 2);
        assert(mapped->get(5) == 16);
        assert(mapped->get(Cardinal::InfinitePlus(0)) == 200);
        delete mapped;

        LazySequence<int> finiteSeq(arr, 3);
        LazySequence<int> *filtered = finiteSeq.where([](int x) { return x % 2 != 0; });
        assert(filtered->getLength() == 2);
        assert(filtered->get(0) == 1);
        assert(filtered->get(1) == 3);
        delete filtered;

        MutableArraySequence<int> otherData;
        otherData.append(10);
        otherData.append(20);
        LazySequence<int> otherSeq(otherData);
        LazySequence<int> *concated = app3->concat(&otherSeq);
        assert(concated->get(Cardinal::InfinitePlus(3)) == 10);
        assert(concated->get(Cardinal::InfinitePlus(4)) == 20);
        delete concated;

        MutableArraySequence<int> init2;
        init2.append(99);
        init2.append(99);
        auto constGen = [](const Sequence<int>&) -> int { return 99; };
        LazySequence<int> constSeq(constGen, init2, Cardinal::Infinite());
        LazySequence<int> *concatInf = fib.concat(&constSeq);
        assert(concatInf->get(Cardinal::Infinite()) == 99);
        assert(concatInf->get(Cardinal::InfinitePlus(5)) == 99);
        delete concatInf;

        auto start = steady_clock::now();
        volatile T sum = 0;

        for (int i = 0; i < 1000000; i++)
            sum += fib.get(i);
            
        auto end = steady_clock::now();
        auto ms = duration_cast<milliseconds>(end - start).count();
        std::cout << "⏳ Stress test: " << ms << " ms (sum=" << sum << ")\n";

        delete app3;
    }

    std::cout << "✅ LazySequence<" << typeName << "> passed\n\n";
}


template void TestLazySequence<int>(const char*);
template void TestLazySequence<double>(const char*);