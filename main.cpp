#include "tests/tests.h"

void runAllTests() {
    TestCardinal();
    TestLazySequence<int>("int");
    TestLazySequence<double>("double");
    TestStream<int>("int");
    TestStream<double>("double");
    TestStateMachine<int, int>("int,int");
    TestStateMachine<int, double>("int,double");
}

int main() {
    runAllTests();
    return 0;
}