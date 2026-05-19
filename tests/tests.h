#ifndef TESTS_H
#define TESTS_H

void TestCardinal();
template <class T> void TestLazySequence(const char* typeName);
template <class T> void TestStream(const char* typeName);
template <class TInput, class TOutput> void TestStateMachine(const char* typeName);

#endif