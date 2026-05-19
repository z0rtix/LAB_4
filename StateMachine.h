#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include "../LAB_2/Sequence.h"

#include "Stream.h"

#include <unordered_map>
#include <utility>
#include <stdexcept>


template <class TInput, class TOutput, class TState = int>
class StateMachine {
    public:
        StateMachine();

        void AddTransition(TState from, TInput input, TState to, TOutput output);
        void SetInitialState(TState state);

        TOutput ProcessSymbol(TInput input);

        void ProcessStream(ReadOnlyStream<TInput> &input, WriteOnlyStream<TOutput> &output);
        void Reset();

        TState GetCurrentState() const;

    private:
        std::unordered_map<TState, std::unordered_map<TInput, std::pair<TState, TOutput>>> transitions;

        TState currentState;
        TState initialState;
};


template <class TInput, class TOutput, class TState>
StateMachine<TInput, TOutput, TState>::StateMachine() : currentState(TState{}), initialState(TState{}) {}

template <class TInput, class TOutput, class TState>
void StateMachine<TInput, TOutput, TState>::AddTransition(TState from, TInput input, TState to, TOutput output) {
    transitions[from][input] = std::make_pair(to, output);
}

template <class TInput, class TOutput, class TState>
void StateMachine<TInput, TOutput, TState>::SetInitialState(TState state) {
    initialState = state;
    currentState = state;
}

template <class TInput, class TOutput, class TState>
TOutput StateMachine<TInput, TOutput, TState>::ProcessSymbol(TInput input) {
    auto fromIt = transitions.find(currentState);

    if (fromIt == transitions.end()) {
        throw std::logic_error("No transitions from current state");
    }

    auto inputIt = fromIt->second.find(input);

    if (inputIt == fromIt->second.end()) {
        throw std::logic_error("No transition for this input from current state");
    }

    TOutput output = inputIt->second.second;
    currentState = inputIt->second.first;

    return output;
}

template <class TInput, class TOutput, class TState>
void StateMachine<TInput, TOutput, TState>::ProcessStream(ReadOnlyStream<TInput> &input, WriteOnlyStream<TOutput> &output) {
    input.Open();
    output.Open();
    while (true) {
        try {
            TInput in = input.Read();
            TOutput outVal = ProcessSymbol(in);
            output.Write(outVal);
        } catch (const std::out_of_range &) {
            break;
        }
    }
    output.Close();
    input.Close();
}

template <class TInput, class TOutput, class TState>
void StateMachine<TInput, TOutput, TState>::Reset() {
    currentState = initialState;
}

template <class TInput, class TOutput, class TState>
TState StateMachine<TInput, TOutput, TState>::GetCurrentState() const {
    return currentState;
}


#endif