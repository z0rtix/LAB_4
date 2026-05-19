#include "tests.h"

#include "../Stream.h"
#include "../LazySequence.h"
#include "../Cardinal.h"

#include <iostream>
#include <chrono>


template <class T>
void TestStream(const char* typeName) {
    using namespace std::chrono;
    std::cout << "=========================================\n";
    std::cout << "✅ TESTS FOR Stream<" << typeName << ">\n";

    T arr[] = {T(1), T(2), T(3), T(4), T(5)};
    LazySequence<T> seq(arr, 5);

    ReadOnlyStream<T> stream(&seq);
    stream.Open();
    T sum = T(0);
    int count = 0;

    while (true) {
        try {
            sum = sum + stream.Read();
            count++;
        } catch (const std::out_of_range&) {
            break;
        }
    }

    if (count == 5 && sum == T(15)) {
        std::cout << "✅ ReadOnlyStream read all\n";
    } else {
        std::cout << "❌ ReadOnlyStream read failed\n";
    }
    
    stream.Close();
    stream.Open();
    stream.Seek(2);

    if (stream.Read() == T(3)) {
        std::cout << "✅ Seek and read\n";
    } else {
        std::cout << "❌ Seek failed\n";
    }
    
    stream.Close();

    MutableArraySequence<T> dest;
    WriteOnlyStream<T> wstream(&dest);

    wstream.Open();

    for (int i = 0; i < 3; i++) {
        wstream.Write(T(i * 10));
    }

    wstream.Close();

    if (dest.getLength() == 3 && dest.get(0) == T(0) && dest.get(2) == T(20)) {
        std::cout << "✅ WriteOnlyStream\n";
    } else {
        std::cout << "❌ WriteOnlyStream failed\n";
    }

    if constexpr (std::is_same<T, int>::value) {
        MutableArraySequence<int> init;
        init.append(1); init.append(1);

        auto fib = [](const Sequence<int>& s) -> int {
            return s.get(s.getLength()-1) + s.get(s.getLength()-2);
        };

        LazySequence<int> infSeq(fib, init, Cardinal::Infinite());
        ReadOnlyStream<int> fibStream(&infSeq);

        std::cout << "⏳ Stress test (1M elements through stream)...\n";
        auto start = steady_clock::now();
        fibStream.Open();

        for (int i = 0; i < 1000000; i++) {
            fibStream.Read();
        }

        fibStream.Close();
        auto end = steady_clock::now();
        std::cout << "✅ Stress test: " << duration_cast<milliseconds>(end - start).count() << " ms\n";
    }

    std::cout << "=========================================\n\n";
}


template void TestStream<int>(const char*);
template void TestStream<double>(const char*);