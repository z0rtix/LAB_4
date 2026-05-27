#ifndef TESTS_H
#define TESTS_H


void TestCardinal();

template <class T> void TestLazySequence(const char *typeName);
template <class T> void TestStream(const char *typeName);
template <class TInput, class TOutput> void TestStateMachine(const char *typeName);

inline void runAllTests() {
    TestCardinal();
    TestLazySequence<int>("int");
    TestLazySequence<double>("double");
    TestStream<int>("int");
    TestStream<double>("double");
    TestStateMachine<int, int>("int,int");
    TestStateMachine<int, double>("int,double");
}


#endif