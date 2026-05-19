#ifndef LAZY_SEQUENCE_H
#define LAZY_SEQUENCE_H

#include "../LAB_2/ArraySequence.h"

#include "Cardinal.h"

#include <functional>
#include <stdexcept>


template <class T>
class LazySequence : public Sequence<T> {
    public:
        LazySequence();
        LazySequence(T *items, int count);
        explicit LazySequence(const Sequence<T> &seq);
        LazySequence(std::function<T(const Sequence<T> &)> generator, const Sequence<T> &initial, Cardinal length = Cardinal::Infinite());

        ~LazySequence() override;


        T getFirst() const override;
        T getLast() const override;
        T get(int index) const override;

        int getLength() const override;
        bool isEmpty() const override;

        LazySequence<T> *append(T item) override;
        LazySequence<T> *prepend(T item) override;
        LazySequence<T> *insertAt(T item, int index) override;
        LazySequence<T> *set(T item, int index) override;

        LazySequence<T> *removeFirst() override;
        LazySequence<T> *removeLast() override;
        LazySequence<T> *removeAt(int index) override;
        LazySequence<T> *clear() override;

        IEnumerator<T> *getEnumerator() const override;
        LazySequence<T> *copy() const override;
        LazySequence<T> *concat(Sequence<T> *sequence) const override;
        LazySequence<T> *getSubsequence(int startIndex, int endIndex) const override;


        Cardinal cardinalLength() const;
        size_t getMaterializedCount() const;

    private:
        mutable Sequence<T> *materialized;
        std::function<T(const Sequence<T> &)> generator;
        Cardinal length;

        bool generatorActive;

        void materializeUpTo(int index) const;
        void requireFinite(const char *msg) const;

        LazySequence<T> *cloneEmpty() const;
        Sequence<T> *cloneMaterialized() const;
};


template <class T>
LazySequence<T>::LazySequence() : materialized(new MutableArraySequence<T>()), generator(nullptr), length(Cardinal::Finite(0)), generatorActive(false) {}

template <class T>
LazySequence<T>::LazySequence(T *items, int count) : materialized(new MutableArraySequence<T>(items, count)), generator(nullptr), length(Cardinal::Finite(count)), generatorActive(false) {}

template <class T>
LazySequence<T>::LazySequence(const Sequence<T> &seq) : materialized(seq.copy()), generator(nullptr), length(Cardinal::Finite(seq.getLength())), generatorActive(false) {}

template <class T>
LazySequence<T>::LazySequence(std::function<T(const Sequence<T> &)> gen, const Sequence<T> &initial, Cardinal len) : materialized(initial.copy()), generator(gen), length(len), generatorActive(true) {}


template <class T>
LazySequence<T>::~LazySequence() {
    delete materialized;
}


template <class T>
Sequence<T> *LazySequence<T>::cloneMaterialized() const {
    return materialized->copy();
}

template <class T>
void LazySequence<T>::materializeUpTo(int index) const {
    if (index < materialized->getLength()) {
        return;
    } else if (!generatorActive || !generator) {
        throw std::out_of_range("Index out of range of finite sequence");
    }

    while (materialized->getLength() <= index) {
        T next = generator(*materialized);
        materialized->append(next);
    }
}

template <class T>
void LazySequence<T>::requireFinite(const char *msg) const {
    if (!length.IsFinite()) {
        throw std::logic_error(msg);
    }
}

template <class T>
LazySequence<T> *LazySequence<T>::cloneEmpty() const {
    return new LazySequence<T>();
}

template <class T>
T LazySequence<T>::get(int index) const {
    materializeUpTo(index);

    return materialized->get(index);
}

template <class T>
T LazySequence<T>::getFirst() const {
    if (isEmpty()) {
        throw std::out_of_range("Empty sequence");
    }

    return get(0);
}

template <class T>
T LazySequence<T>::getLast() const {
    requireFinite("Cannot get last of infinite lazy sequence");

    if (length.GetValue() == 0) {
        throw std::out_of_range("Empty sequence");
    }

    return get(length.GetValue() - 1);
}

template <class T>
int LazySequence<T>::getLength() const {
    requireFinite("Length of infinite sequence is not representable as int");

    return length.GetValue();
}

template <class T>
bool LazySequence<T>::isEmpty() const {
    return length.IsFinite() && length.GetValue() == 0;
}


template <class T>
LazySequence<T> *LazySequence<T>::append(T item) {
    requireFinite("Cannot append to infinite lazy sequence");
    LazySequence<T> *newSeq = new LazySequence<T>();
    delete newSeq->materialized;

    newSeq->materialized = cloneMaterialized();
    newSeq->materialized->append(item);
    newSeq->length = Cardinal::Finite(newSeq->materialized->getLength());
    newSeq->generatorActive = false;

    return newSeq;
}

template <class T>
LazySequence<T> *LazySequence<T>::prepend(T item) {
    requireFinite("Cannot prepend to infinite lazy sequence");
    LazySequence<T> *newSeq = new LazySequence<T>();
    delete newSeq->materialized;

    newSeq->materialized = cloneMaterialized();
    newSeq->materialized->prepend(item);
    newSeq->length = Cardinal::Finite(newSeq->materialized->getLength());
    newSeq->generatorActive = false;

    return newSeq;
}

template <class T>
LazySequence<T> *LazySequence<T>::insertAt(T item, int index) {
    requireFinite("Cannot insert into infinite lazy sequence");
    materializeUpTo(index);
    LazySequence<T> *newSeq = new LazySequence<T>();
    delete newSeq->materialized;

    newSeq->materialized = cloneMaterialized();
    newSeq->materialized->insertAt(item, index);
    newSeq->length = Cardinal::Finite(newSeq->materialized->getLength());
    newSeq->generatorActive = false;

    return newSeq;
}

template <class T>
LazySequence<T> *LazySequence<T>::set(T item, int index) {
    materializeUpTo(index);
    LazySequence<T> *newSeq = new LazySequence<T>();
    delete newSeq->materialized;

    newSeq->materialized = cloneMaterialized();
    newSeq->materialized->set(item, index);
    newSeq->length = Cardinal::Finite(newSeq->materialized->getLength());
    newSeq->generatorActive = false;

    return newSeq;
}

template <class T>
LazySequence<T> *LazySequence<T>::removeFirst() {
    requireFinite("Cannot remove from infinite lazy sequence");
    if (isEmpty()) throw std::out_of_range("Empty sequence");
    LazySequence<T> *newSeq = new LazySequence<T>();
    delete newSeq->materialized;

    newSeq->materialized = cloneMaterialized();
    newSeq->materialized->removeFirst();
    newSeq->length = Cardinal::Finite(newSeq->materialized->getLength());
    newSeq->generatorActive = false;

    return newSeq;
}

template <class T>
LazySequence<T> *LazySequence<T>::removeLast() {
    requireFinite("Cannot remove from infinite lazy sequence");
    if (isEmpty()) throw std::out_of_range("Empty sequence");
    LazySequence<T> *newSeq = new LazySequence<T>();
    delete newSeq->materialized;

    newSeq->materialized = cloneMaterialized();
    newSeq->materialized->removeLast();
    newSeq->length = Cardinal::Finite(newSeq->materialized->getLength());
    newSeq->generatorActive = false;

    return newSeq;
}

template <class T>
LazySequence<T> *LazySequence<T>::removeAt(int index) {
    requireFinite("Cannot remove from infinite lazy sequence");
    materializeUpTo(index);
    LazySequence<T> *newSeq = new LazySequence<T>();
    delete newSeq->materialized;

    newSeq->materialized = cloneMaterialized();
    newSeq->materialized->removeAt(index);
    newSeq->length = Cardinal::Finite(newSeq->materialized->getLength());
    newSeq->generatorActive = false;

    return newSeq;
}

template <class T>
LazySequence<T> *LazySequence<T>::clear() {
    return new LazySequence<T>();
}

template <class T>
IEnumerator<T> *LazySequence<T>::getEnumerator() const {
    return materialized->getEnumerator();
}

template <class T>
LazySequence<T> *LazySequence<T>::copy() const {
    LazySequence<T> *newSeq = new LazySequence<T>();
    delete newSeq->materialized;

    newSeq->materialized = cloneMaterialized();
    newSeq->generator = this->generator;
    newSeq->length = this->length;
    newSeq->generatorActive = this->generatorActive;

    return newSeq;
}

template <class T>
LazySequence<T> *LazySequence<T>::concat(Sequence<T> *sequence) const {
    requireFinite("Cannot concat to infinite lazy sequence");
    LazySequence<T> *newSeq = new LazySequence<T>();
    delete newSeq->materialized;

    newSeq->materialized = cloneMaterialized();
    for (int i = 0; i < sequence->getLength(); i++) {
        newSeq->materialized->append(sequence->get(i));
    }

    newSeq->length = Cardinal::Finite(newSeq->materialized->getLength());
    newSeq->generatorActive = false;

    return newSeq;
}

template <class T>
LazySequence<T> *LazySequence<T>::getSubsequence(int startIndex, int endIndex) const {
    materializeUpTo(endIndex);
    LazySequence<T> *newSeq = new LazySequence<T>();
    delete newSeq->materialized;

    newSeq->materialized = materialized->getSubsequence(startIndex, endIndex);
    newSeq->length = Cardinal::Finite(newSeq->materialized->getLength());

    return newSeq;
}

template <class T>
Cardinal LazySequence<T>::cardinalLength() const {
    return length;
}

template <class T>
size_t LazySequence<T>::getMaterializedCount() const {
    return materialized->getLength();
}


#endif