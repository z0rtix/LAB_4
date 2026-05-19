#include "tests.h"

#include "../StateMachine.h"
#include "../Stream.h"
#include "../LazySequence.h"
#include "../Cardinal.h"

#include <iostream>
#include <chrono>


template <class TInput, class TOutput>
void TestStateMachine(const char* typeName) {
    using namespace std::chrono;
    std::cout << "=========================================\n";
    std::cout << "✅ TESTS FOR StateMachine<" << typeName << ">\n";

    StateMachine<TInput, TOutput> sm;
    sm.AddTransition(0, TInput(0), 0, TOutput(10));
    sm.AddTransition(0, TInput(1), 1, TOutput(20));
    sm.AddTransition(1, TInput(0), 1, TOutput(30));
    sm.AddTransition(1, TInput(1), 0, TOutput(40));
    sm.SetInitialState(0);

    TOutput out = sm.ProcessSymbol(TInput(0));
    if (out == TOutput(10) && sm.GetCurrentState() == 0)
        std::cout << "✅ Transition 0->0\n";
    else
        std::cout << "❌ Transition 0->0 FAILED\n";

    out = sm.ProcessSymbol(TInput(1));
    if (out == TOutput(20) && sm.GetCurrentState() == 1)
        std::cout << "✅ Transition 0->1\n";
    else
        std::cout << "❌ Transition 0->1 FAILED\n";

    sm.Reset();
    if (sm.GetCurrentState() == 0)
        std::cout << "✅ Reset\n";
    else
        std::cout << "❌ Reset FAILED\n";

    TInput arr[] = {TInput(0), TInput(1), TInput(0)};
    LazySequence<TInput> seq(arr, 3);
    ReadOnlyStream<TInput> inputStream(&seq);
    MutableArraySequence<TOutput> outSeq;
    WriteOnlyStream<TOutput> outputStream(&outSeq);

    sm.Reset();
    sm.ProcessStream(inputStream, outputStream);

    if (outSeq.getLength() == 3 && outSeq.get(0) == TOutput(10) && outSeq.get(1) == TOutput(20) && outSeq.get(2) == TOutput(30)) {
        std::cout << "✅ ProcessStream\n";
    } else {
        std::cout << "❌ ProcessStream FAILED\n";
    }

    if constexpr (std::is_same<TInput, int>::value) {
        MutableArraySequence<int> init;
        init.append(0); init.append(1);

        auto alternating = [](const Sequence<int>& s) -> int {
            return (s.get(s.getLength()-1) + 1) % 2;
        };

        LazySequence<int> infSeq(alternating, init, Cardinal::Infinite());
        ReadOnlyStream<int> bigInput(&infSeq);
        MutableArraySequence<int> bigOutput;
        WriteOnlyStream<int> bigOutputStream(&bigOutput);

        sm.Reset();
        std::cout << "⏳ Stress test (1M symbols through FSM)...\n";

        auto start = steady_clock::now();
        bigInput.Open();
        bigOutputStream.Open();

        for (int i = 0; i < 1000000; i++) {
            TInput in = bigInput.Read();
            TOutput outVal = sm.ProcessSymbol(in);
            bigOutputStream.Write(outVal);
        }

        bigInput.Close();
        bigOutputStream.Close();

        auto end = steady_clock::now();
        std::cout << "✅ Stress test: " << duration_cast<milliseconds>(end - start).count() << " ms\n";
    }

    std::cout << "=========================================\n\n";
}


template void TestStateMachine<int, int>(const char*);
template void TestStateMachine<int, double>(const char*);