#include "tests/tests.h"

int main() {
    TestCardinal();
    TestLazySequence<int>("int");
    TestLazySequence<double>("double");
    TestStream<int>("int");
    TestStream<double>("double");
    TestStateMachine<int, int>("int,int");
    TestStateMachine<int, double>("int,double");
    
    return 0;
}