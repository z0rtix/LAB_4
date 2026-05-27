#ifndef LAZY_SEQUENCE_H
#define LAZY_SEQUENCE_H

#include "../LAB_2/ArraySequence.h"

#include "Cardinal.h"

#include <functional>
#include <stdexcept>


template <class T>
class LazySequence : public Sequence<T> {
	private:
		struct Segment {
			enum Type { FiniteData, Generator };

			Type type;
			Sequence<T>* data;
			std::function<T(const Sequence<T>&)> generator;
			Cardinal length;

			Segment();
			Segment(const Sequence<T>& finiteData, const Cardinal& len);
			Segment(std::function<T(const Sequence<T>&)> gen, const Sequence<T>& initial, const Cardinal& len);
			Segment(const Segment& other);

			Segment& operator=(const Segment& other);

			~Segment();
		};

		mutable MutableArraySequence<Segment*> segments;
		Cardinal totalLength;

		static Cardinal ordinalAdd(const Cardinal& a, const Cardinal& b);
		static Cardinal ordinalSub(const Cardinal& a, const Cardinal& b);

		void ensureMaterialized(Segment* seg, size_t index) const;
		int findSegment(const Cardinal& index, size_t& localIdx) const;

	public:
		LazySequence();
		explicit LazySequence(const Cardinal& len);
		LazySequence(const T* items, int count);
		explicit LazySequence(const Sequence<T>& seq);
		LazySequence(std::function<T(const Sequence<T>&)> gen, const Sequence<T>& initial, Cardinal len = Cardinal::Infinite());
		~LazySequence() override;

		T getFirst() const override;
		T getLast() const override;
		T get(int index) const override;
		T get(Cardinal index) const;

		int getLength() const override;
		bool isEmpty() const override;

		LazySequence<T>* append(T item) override;
		LazySequence<T>* prepend(T item) override;
		LazySequence<T>* insertAt(T item, int index) override;
		LazySequence<T>* set(T item, int index) override;
		LazySequence<T>* removeFirst() override;
		LazySequence<T>* removeLast() override;
		LazySequence<T>* removeAt(int index) override;
		LazySequence<T>* clear() override;

		IEnumerator<T>* getEnumerator() const override;

		LazySequence<T>* copy() const override;
		LazySequence<T>* concat(Sequence<T>* sequence) const override;
		LazySequence<T>* concat(LazySequence<T>* lazySeq) const;

		LazySequence<T>* getSubsequence(int startIndex, int endIndex) const override;
		LazySequence<T>* getSubsequence(Cardinal start, Cardinal end) const;

		LazySequence<T>* map(std::function<T(T)> func) const;
		LazySequence<T>* where(std::function<bool(T)> pred) const;

		Cardinal cardinalLength() const;
		size_t getMaterializedCount() const;
	};


template <class T>
LazySequence<T>::Segment::Segment() : type(FiniteData), data(nullptr), generator(nullptr), length(Cardinal::Finite(0)) {}

template <class T>
LazySequence<T>::Segment::Segment(const Sequence<T>& finiteData, const Cardinal& len) : type(FiniteData), data(finiteData.copy()), generator(nullptr), length(len) {}

template <class T>
LazySequence<T>::Segment::Segment(std::function<T(const Sequence<T>&)> gen, const Sequence<T>& initial, const Cardinal& len) : type(Generator), data(initial.copy()), generator(gen), length(len) {}

template <class T>
LazySequence<T>::Segment::Segment(const Segment& other) : type(other.type), data(nullptr), generator(other.generator), length(other.length) {
	if (other.data) data = other.data->copy();
}

template <class T>
typename LazySequence<T>::Segment& LazySequence<T>::Segment::operator=(const Segment& other) {
	if (this != &other) {
		delete data;

		type = other.type;
		generator = other.generator;
		length = other.length;

		data = other.data ? other.data->copy() : nullptr;
	}

	return *this;
}

template <class T>
LazySequence<T>::Segment::~Segment() {
	delete data;
}


template <class T>
Cardinal LazySequence<T>::ordinalAdd(const Cardinal& a, const Cardinal& b) {
	if (a.IsFinite() && b.IsFinite()) {
		return Cardinal::Finite(a.GetFiniteValue() + b.GetFiniteValue());
	} else if (a.IsFinite() && b.IsInfinite()) {
		return Cardinal::Omega(b.GetOmegaCoeff(), b.GetFinitePart());
	} else if (a.IsInfinite() && b.IsFinite()) {
		return Cardinal::Omega(a.GetOmegaCoeff(), a.GetFinitePart() + b.GetFiniteValue());
	} else {
		return Cardinal::Omega(a.GetOmegaCoeff() + b.GetOmegaCoeff(), b.GetFinitePart());
	}
}

template <class T>
Cardinal LazySequence<T>::ordinalSub(const Cardinal& a, const Cardinal& b) {
	if (a.IsFinite() && b.IsFinite()) {
		if (b.GetFiniteValue() > a.GetFiniteValue()) throw std::logic_error("Negative finite");

		return Cardinal::Finite(a.GetFiniteValue() - b.GetFiniteValue());
	} else if (a.IsInfinite() && b.IsFinite()) {
		return Cardinal::Omega(a.GetOmegaCoeff(), a.GetFinitePart() - b.GetFiniteValue());
	} else if (a.IsInfinite() && b.IsInfinite() && a.GetOmegaCoeff() == b.GetOmegaCoeff()) {
		if (b.GetFinitePart() > a.GetFinitePart()) throw std::logic_error("Negative infinite");

		return Cardinal::Finite(a.GetFinitePart() - b.GetFinitePart());
	} else {
		throw std::logic_error("Invalid subtraction");
	}
}


template <class T>
void LazySequence<T>::ensureMaterialized(Segment* seg, size_t index) const {
	if (seg->type != Segment::Generator) return;

	while (static_cast<size_t>(seg->data->getLength()) <= index) {
		T next = seg->generator(*seg->data);
		seg->data->append(next);
	}
}

template <class T>
int LazySequence<T>::findSegment(const Cardinal& index, size_t& localIdx) const {
	Cardinal prefix = Cardinal::Finite(0);

	for (int i = 0; i < segments.getLength(); i++) {
		Segment* seg = segments.get(i);

		Cardinal segEnd = ordinalAdd(prefix, seg->length);

		if (prefix <= index && index < segEnd) {
			Cardinal diff = ordinalSub(index, prefix);

			localIdx = diff.IsFinite() ? diff.GetFiniteValue() : diff.GetFinitePart();

			return i;
		}

		prefix = segEnd;
	}

	return -1;
}


template <class T>
LazySequence<T>::LazySequence() : totalLength(Cardinal::Finite(0)) {}

template <class T>
LazySequence<T>::LazySequence(const Cardinal& len) : totalLength(len) {}

template <class T>
LazySequence<T>::LazySequence(const T* items, int count) : totalLength(Cardinal::Finite(count)) {
	MutableArraySequence<T> arr(const_cast<T*>(items), count);
	segments.append(new Segment(arr, Cardinal::Finite(count)));
}

template <class T>
LazySequence<T>::LazySequence(const Sequence<T>& seq) : totalLength(Cardinal::Finite(seq.getLength())) {
	segments.append(new Segment(seq, Cardinal::Finite(seq.getLength())));
}

template <class T>
LazySequence<T>::LazySequence(std::function<T(const Sequence<T>&)> gen, const Sequence<T>& initial, Cardinal len) : totalLength(len) {
	segments.append(new Segment(gen, initial, len));
}

template <class T>
LazySequence<T>::~LazySequence() {
	for (int i = 0; i < segments.getLength(); i++) {
		delete segments.get(i);
	}
}


template <class T>
T LazySequence<T>::getFirst() const {
	return get(Cardinal::Finite(0));
}

template <class T>
T LazySequence<T>::getLast() const {
	if (totalLength.IsInfinite()) throw std::logic_error("Infinite sequence");

	return get(Cardinal::Finite(totalLength.GetFiniteValue() - 1));
}

template <class T>
T LazySequence<T>::get(int index) const {
	return get(Cardinal::Finite(index));
}

template <class T>
T LazySequence<T>::get(Cardinal index) const {
	size_t localIdx = 0;
	int segIdx = findSegment(index, localIdx);

	if (segIdx == -1) throw std::out_of_range("Index out of range");

	Segment* seg = segments.get(segIdx);
	ensureMaterialized(seg, localIdx);

	return seg->data->get(localIdx);
}

template <class T>
int LazySequence<T>::getLength() const {
	if (totalLength.IsInfinite()) throw std::logic_error("Infinite length");

	return totalLength.GetFiniteValue();
}

template <class T>
bool LazySequence<T>::isEmpty() const {
	return totalLength.IsFinite() && totalLength.GetFiniteValue() == 0;
}


template <class T>
LazySequence<T>* LazySequence<T>::append(T item) {
	LazySequence<T>* newSeq = copy();

	if (newSeq->segments.getLength() == 0) {
		MutableArraySequence<T> block;
		block.append(item);

		newSeq->segments.append(new Segment(block, Cardinal::Finite(1)));
		newSeq->totalLength = ordinalAdd(newSeq->totalLength, Cardinal::Finite(1));

		return newSeq;
	}

	int lastIdx = newSeq->segments.getLength() - 1;
	Segment* lastSeg = newSeq->segments.get(lastIdx);

	if (lastSeg->type == Segment::FiniteData) {
		lastSeg->data->append(item);
		lastSeg->length = Cardinal::Finite(lastSeg->data->getLength());
	} else {
		MutableArraySequence<T> block;
		block.append(item);

		newSeq->segments.append(new Segment(block, Cardinal::Finite(1)));
	}

	newSeq->totalLength = ordinalAdd(newSeq->totalLength, Cardinal::Finite(1));

	return newSeq;
}

template <class T>
LazySequence<T>* LazySequence<T>::prepend(T item) {
	if (totalLength.IsInfinite()) throw std::logic_error("Infinite sequence");

	MutableArraySequence<T> arr;
	arr.append(item);

	for (int i = 0; i < getLength(); i++) {
		arr.append(get(i));
	}

	return new LazySequence<T>(arr);
}

template <class T>
LazySequence<T>* LazySequence<T>::insertAt(T item, int index) {
	if (totalLength.IsInfinite()) throw std::logic_error("Infinite sequence");
	if (index < 0 || index > getLength()) throw std::out_of_range("Index out of range");

	MutableArraySequence<T> arr;

	for (int i = 0; i < index; i++)
		arr.append(get(i));

	arr.append(item);

	for (int i = index; i < getLength(); i++)
		arr.append(get(i));

	return new LazySequence<T>(arr);
}

template <class T>
LazySequence<T>* LazySequence<T>::set(T item, int index) {
	if (index < 0) throw std::out_of_range("Index out of range");

	LazySequence<T>* newSeq = copy();
	size_t localIdx = 0;
	int segIdx = newSeq->findSegment(Cardinal::Finite(index), localIdx);

	if (segIdx == -1) throw std::out_of_range("Index out of range");

	Segment* seg = newSeq->segments.get(segIdx);
	newSeq->ensureMaterialized(seg, localIdx);
	seg->data->set(item, localIdx);

	return newSeq;
}

template <class T>
LazySequence<T>* LazySequence<T>::removeFirst() {
	if (isEmpty()) throw std::out_of_range("Empty");

	return removeAt(0);
}

template <class T>
LazySequence<T>* LazySequence<T>::removeLast() {
	if (isEmpty()) throw std::out_of_range("Empty");

	return removeAt(getLength() - 1);
}

template <class T>
LazySequence<T>* LazySequence<T>::removeAt(int index) {
	if (totalLength.IsInfinite()) throw std::logic_error("Infinite sequence");
	if (index < 0 || index >= getLength()) throw std::out_of_range("Index out of range");

	MutableArraySequence<T> arr;

	for (int i = 0; i < getLength(); i++) {
		if (i != index) arr.append(get(i));
	}

	return new LazySequence<T>(arr);
}

template <class T>
LazySequence<T>* LazySequence<T>::clear() {
	return new LazySequence<T>();
}


template <class T>
IEnumerator<T>* LazySequence<T>::getEnumerator() const {
	throw std::logic_error("Not implemented");
}


template <class T>
LazySequence<T>* LazySequence<T>::copy() const {
	LazySequence<T>* newSeq = new LazySequence<T>();

	for (int i = 0; i < segments.getLength(); i++) {
		newSeq->segments.append(new Segment(*segments.get(i)));
	}

	newSeq->totalLength = totalLength;

	return newSeq;
}

template <class T>
LazySequence<T>* LazySequence<T>::concat(Sequence<T>* sequence) const {
	LazySequence<T>* newSeq = copy();
	Sequence<T>* copied = sequence->copy();

	newSeq->segments.append(new Segment(*copied, Cardinal::Finite(sequence->getLength())));

	delete copied;

	newSeq->totalLength = ordinalAdd(newSeq->totalLength, Cardinal::Finite(sequence->getLength()));

	return newSeq;
}

template <class T>
LazySequence<T>* LazySequence<T>::concat(LazySequence<T>* lazySeq) const {
	LazySequence<T>* newSeq = copy();

	for (int i = 0; i < lazySeq->segments.getLength(); i++)
		newSeq->segments.append(new Segment(*lazySeq->segments.get(i)));

	newSeq->totalLength = ordinalAdd(newSeq->totalLength, lazySeq->totalLength);

	return newSeq;
}


template <class T>
LazySequence<T>* LazySequence<T>::getSubsequence(int startIndex, int endIndex) const {
	return getSubsequence(Cardinal::Finite(startIndex), Cardinal::Finite(endIndex + 1));
}

template <class T>
LazySequence<T>* LazySequence<T>::getSubsequence(Cardinal start, Cardinal end) const {
	if (start < Cardinal::Finite(0) || end <= start) throw std::out_of_range("Invalid range");
	if (totalLength.IsFinite() && end > totalLength) throw std::out_of_range("Index out of range");

	MutableArraySequence<T> arr;
	Cardinal cur = start;

	while (cur < end) {
		arr.append(get(cur));
		cur = ordinalAdd(cur, Cardinal::Finite(1));
	}

	return new LazySequence<T>(arr);
}


template <class T>
LazySequence<T>* LazySequence<T>::map(std::function<T(T)> func) const {
	LazySequence<T>* newSeq = new LazySequence<T>();

	Cardinal prefix = Cardinal::Finite(0);

	for (int i = 0; i < segments.getLength(); i++) {
		Segment* seg = segments.get(i);

		if (seg->type == Segment::FiniteData) {
			MutableArraySequence<T> mapped;

			for (int j = 0; j < seg->data->getLength(); j++)
				mapped.append(func(seg->data->get(j)));

			newSeq->segments.append(new Segment(mapped, seg->length));
		} else {
			auto self = this;
			Cardinal capturedPrefix = prefix;

			auto gen = [self, func, capturedPrefix](const Sequence<T>& hist) -> T {
				size_t nextIdx = hist.getLength();

				Cardinal globalIdx = ordinalAdd(
					capturedPrefix,
					Cardinal::Finite(nextIdx)
				);

				return func(self->get(globalIdx));
			};

			MutableArraySequence<T> empty;

			newSeq->segments.append(
				new Segment(gen, empty, seg->length)
			);
		}

		prefix = ordinalAdd(prefix, seg->length);
	}

	newSeq->totalLength = totalLength;

	return newSeq;
}

template <class T>
LazySequence<T>* LazySequence<T>::where(std::function<bool(T)> pred) const {
	MutableArraySequence<T> filtered;

	size_t limit = totalLength.IsFinite() ? totalLength.GetFiniteValue() : 1000;

	for (size_t i = 0; i < limit; i++) {
		T value = get(Cardinal::Finite(i));

		if (pred(value)) filtered.append(value);
	}

	return new LazySequence<T>(filtered);
}


template <class T>
Cardinal LazySequence<T>::cardinalLength() const {
	return totalLength;
}

template <class T>
size_t LazySequence<T>::getMaterializedCount() const {
	size_t count = 0;

	for (int i = 0; i < segments.getLength(); i++) {
		Segment* seg = segments.get(i);

		if (seg->data) count += seg->data->getLength();
	}

	return count;
}


#endif