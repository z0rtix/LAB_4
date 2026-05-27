#include "tests.h"

#include "../Stream.h"
#include "../LazySequence.h"
#include "../Cardinal.h"

#include <cassert>
#include <iostream>
#include <chrono>
#include <stdexcept>


template <class T>
void TestStream(const char *typeName) {
    using namespace std::chrono;
    T arr[] = {T(1), T(2), T(3), T(4), T(5)};
    LazySequence<T> seq(arr, 5);

    ReadOnlyStream<T> input(&seq);
    input.Open();
    T sum = T(0);
    int cnt = 0;

    while (true) {
        try {
            sum = sum + input.Read();
            cnt++;
        } catch (const std::out_of_range&) {
            break;
        }
    }

    assert(cnt == 5);
    assert(sum == T(15));
    input.Close();

    input.Open();
    input.Seek(2);
    assert(input.Read() == T(3));
    input.Close();

    MutableArraySequence<T> dest;
    WriteOnlyStream<T> output(&dest);
    output.Open();

    for (int i = 0; i < 3; i++)
        output.Write(T(i * 10));

    output.Close();
    assert(dest.getLength() == 3);
    assert(dest.get(0) == T(0));
    assert(dest.get(2) == T(20));

    if constexpr (std::is_same<T, int>::value) {
        MutableArraySequence<int> init;
        init.append(1); init.append(1);

        auto fibGen = [](const Sequence<int>& s) -> int {
            return s.get(s.getLength()-1) + s.get(s.getLength()-2);
        };

        LazySequence<int> infSeq(fibGen, init, Cardinal::Infinite());
        ReadOnlyStream<int> fibStream(&infSeq);
        auto start = steady_clock::now();
        fibStream.Open();

        for (int i = 0; i < 1000000; i++)
            fibStream.Read();

        fibStream.Close();
        auto end = steady_clock::now();
        std::cout << "⏳ Stress test (Stream): " << duration_cast<milliseconds>(end - start).count() << " ms\n";
    }

    std::cout << "✅ Stream<" << typeName << "> passed\n\n";
}


template void TestStream<int>(const char*);
template void TestStream<double>(const char*);