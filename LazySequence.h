#ifndef LAZY_SEQUENCE_H
#define LAZY_SEQUENCE_H

#include "../LAB_2/ArraySequence.h"

#include "Cardinal.h"

#include <functional>
#include <stdexcept>
#include <vector>
#include <memory>


template <class T>
class LazySequence : public Sequence<T>
{
    private:
        struct Segment
        {
            enum Type
            {
                FiniteData,
                Generator
            };

            Type type;
            Sequence<T>* data;
            std::function<T(const Sequence<T>&)> generator;
            Cardinal length;

            Segment()
                : type(FiniteData), data(nullptr), generator(nullptr), length(Cardinal::Finite(0))
            {}

            Segment(const Sequence<T>& finiteData, const Cardinal& len)
                : type(FiniteData), data(finiteData.copy()), generator(nullptr), length(len)
            {}

            Segment(std::function<T(const Sequence<T>&)> gen,
                const Sequence<T>& initial,
                const Cardinal& len)
                : type(Generator), data(initial.copy()), generator(gen), length(len)
            {}

            Segment(const Segment& other)
                : type(other.type), data(nullptr), generator(other.generator), length(other.length)
            {
                if (other.data)
                    data = other.data->copy();
            }

            Segment& operator=(const Segment& other)
            {
                if (this != &other)
                {
                    delete data;
                    type = other.type;
                    generator = other.generator;
                    length = other.length;
                    data = other.data ? other.data->copy() : nullptr;
                }
                return *this;
            }

            ~Segment()
            {
                delete data;
            }
        };

        mutable std::vector<Segment> segments;
        Cardinal totalLength;

        static Cardinal ordinalAdd(const Cardinal& a, const Cardinal& b)
        {
            if (a.IsFinite() && b.IsFinite())
                return Cardinal::Finite(a.GetFiniteValue() + b.GetFiniteValue());
            if (a.IsFinite() && b.IsInfinite())
                return Cardinal::Omega(b.GetOmegaCoeff(), b.GetFinitePart());
            if (a.IsInfinite() && b.IsFinite())
                return Cardinal::Omega(a.GetOmegaCoeff(), a.GetFinitePart() + b.GetFiniteValue());
            return Cardinal::Omega(a.GetOmegaCoeff() + b.GetOmegaCoeff(), b.GetFinitePart());
        }

        static Cardinal ordinalSub(const Cardinal& a, const Cardinal& b)
        {
            if (a.IsFinite() && b.IsFinite())
            {
                if (b.GetFiniteValue() > a.GetFiniteValue())
                    throw std::logic_error("Subtraction would make finite cardinal negative");
                return Cardinal::Finite(a.GetFiniteValue() - b.GetFiniteValue());
            }
            if (a.IsInfinite() && b.IsFinite())
                return Cardinal::Omega(a.GetOmegaCoeff(), a.GetFinitePart() - b.GetFiniteValue());
            if (a.IsInfinite() && b.IsInfinite() && a.GetOmegaCoeff() == b.GetOmegaCoeff())
            {
                if (b.GetFinitePart() > a.GetFinitePart())
                    throw std::logic_error("Subtraction would make infinite cardinal's finite part negative");
                return Cardinal::Finite(a.GetFinitePart() - b.GetFinitePart());
            }
            throw std::logic_error("Cannot subtract these cardinals");
        }

        void ensureMaterialized(Segment& seg, size_t index) const
        {
            if (seg.type != Segment::Generator)
                return;
            while (static_cast<size_t>(seg.data->getLength()) <= index)
            {
                T next = seg.generator(*seg.data);
                seg.data->append(next);
            }
        }

        const Segment* findSegment(const Cardinal& index, size_t& localIdx) const
        {
            Cardinal prefix = Cardinal::Finite(0);
            for (const auto& seg : segments)
            {
                Cardinal segEnd = ordinalAdd(prefix, seg.length);
                if (prefix <= index && index < segEnd)
                {
                    Cardinal diff = ordinalSub(index, prefix);
                    if (diff.IsFinite())
                        localIdx = diff.GetFiniteValue();
                    else
                        localIdx = diff.GetFinitePart();
                    return &seg;
                }
                prefix = segEnd;
            }
            return nullptr;
        }

        Cardinal computePrefix(size_t segIndex) const
        {
            Cardinal prefix = Cardinal::Finite(0);
            for (size_t i = 0; i < segIndex; ++i)
                prefix = ordinalAdd(prefix, segments[i].length);
            return prefix;
        }

    public:
        LazySequence()
        {
            totalLength = Cardinal::Finite(0);
        }

        explicit LazySequence(const Cardinal& len)
            : totalLength(len)
        {}

        LazySequence(const T* items, int count)
            : totalLength(Cardinal::Finite(count))
        {
            MutableArraySequence<T> arr(const_cast<T*>(items), count);
            Segment seg(arr, Cardinal::Finite(count));
            segments.push_back(seg);
        }

        explicit LazySequence(const Sequence<T>& seq)
            : totalLength(Cardinal::Finite(seq.getLength()))
        {
            Segment seg(seq, Cardinal::Finite(seq.getLength()));
            segments.push_back(seg);
        }

        LazySequence(std::function<T(const Sequence<T>&)> gen,
            const Sequence<T>& initial,
            Cardinal len = Cardinal::Infinite())
            : totalLength(len)
        {
            Segment seg(gen, initial, len);
            segments.push_back(seg);
        }

        ~LazySequence() override
        {}

        T getFirst() const override
        {
            return get(Cardinal::Finite(0));
        }

        T getLast() const override
        {
            if (totalLength.IsInfinite())
                throw std::logic_error("Cannot get last of infinite lazy sequence");
            return get(Cardinal::Finite(totalLength.GetFiniteValue() - 1));
        }

        T get(int index) const override
        {
            return get(Cardinal::Finite(index));
        }

        T get(Cardinal index) const
        {
            size_t localIdx = 0;
            const Segment* seg = findSegment(index, localIdx);
            if (!seg)
                throw std::out_of_range("Index out of range");
            ensureMaterialized(const_cast<Segment&>(*seg), localIdx);
            return seg->data->get(localIdx);
        }

        int getLength() const override
        {
            if (totalLength.IsInfinite())
                throw std::logic_error("Length of infinite sequence is not representable as int");
            return totalLength.GetFiniteValue();
        }

        bool isEmpty() const override
        {
            return totalLength.IsFinite() && totalLength.GetFiniteValue() == 0;
        }

        LazySequence<T>* append(T item) override
        {
            LazySequence<T>* newSeq = copy();
            if (newSeq->totalLength.IsInfinite())
            {
                MutableArraySequence<T> block;
                block.append(item);
                Segment newSeg(block, Cardinal::Finite(1));
                newSeq->segments.push_back(newSeg);
                newSeq->totalLength = ordinalAdd(newSeq->totalLength, Cardinal::Finite(1));
            }
            else
            {
                if (newSeq->segments.empty())
                {
                    MutableArraySequence<T> block;
                    block.append(item);
                    newSeq->segments.emplace_back(block, Cardinal::Finite(1));
                    newSeq->totalLength = Cardinal::Finite(1);
                }
                else
                {
                    Segment& lastSeg = newSeq->segments.back();
                    if (lastSeg.type == Segment::FiniteData)
                    {
                        lastSeg.data->append(item);
                        lastSeg.length = Cardinal::Finite(lastSeg.data->getLength());
                        newSeq->totalLength = ordinalAdd(newSeq->totalLength, Cardinal::Finite(1));
                    }
                    else
                    {
                        MutableArraySequence<T> block;
                        block.append(item);
                        Segment newSeg(block, Cardinal::Finite(1));
                        newSeq->segments.push_back(newSeg);
                        newSeq->totalLength = ordinalAdd(newSeq->totalLength, Cardinal::Finite(1));
                    }
                }
            }
            return newSeq;
        }

        LazySequence<T>* prepend(T item) override
        {
            if (totalLength.IsInfinite())
                throw std::logic_error("Cannot prepend to infinite sequence");
            MutableArraySequence<T> newData;
            newData.append(item);
            for (size_t i = 0; i < static_cast<size_t>(getLength()); ++i)
                newData.append(get(i));
            return new LazySequence<T>(newData);
        }

        LazySequence<T>* insertAt(T item, int index) override
        {
            Cardinal idx = Cardinal::Finite(index);
            if (totalLength.IsInfinite())
                throw std::logic_error("Cannot insert into infinite sequence");
            if (idx < Cardinal::Finite(0) || idx >= totalLength)
                throw std::out_of_range("Index out of range");

            LazySequence<T>* newSeq = new LazySequence<T>();
            bool inserted = false;
            Cardinal prefix = Cardinal::Finite(0);

            for (const auto& seg : segments)
            {
                Cardinal segEnd = ordinalAdd(prefix, seg.length);
                if (!inserted && prefix <= idx && idx < segEnd)
                {
                    Cardinal diff = ordinalSub(idx, prefix);
                    size_t localIdx = diff.GetFiniteValue();

                    if (localIdx > 0)
                    {
                        Sequence<T>* leftPart = seg.data->getSubsequence(0, localIdx - 1);
                        Segment left(*leftPart, Cardinal::Finite(localIdx));
                        delete leftPart;
                        newSeq->segments.push_back(left);
                    }

                    MutableArraySequence<T> itemBlock;
                    itemBlock.append(item);
                    newSeq->segments.emplace_back(itemBlock, Cardinal::Finite(1));

                    if (localIdx < static_cast<size_t>(seg.data->getLength()))
                    {
                        Sequence<T>* rightPart = seg.data->getSubsequence(localIdx, seg.data->getLength() - 1);
                        Segment right(*rightPart, Cardinal::Finite(seg.data->getLength() - localIdx));
                        delete rightPart;
                        newSeq->segments.push_back(right);
                    }

                    inserted = true;
                }
                else
                {
                    Segment copy(seg);
                    newSeq->segments.push_back(copy);
                }
                prefix = segEnd;
            }

            if (!inserted)
                throw std::out_of_range("Index out of range");

            newSeq->totalLength = ordinalAdd(totalLength, Cardinal::Finite(1));
            return newSeq;
        }

        LazySequence<T>* set(T item, int index) override
        {
            Cardinal idx = Cardinal::Finite(index);
            if (totalLength.IsInfinite())
                throw std::logic_error("Cannot set on infinite sequence");
            LazySequence<T>* newSeq = copy();
            size_t localIdx = 0;
            Segment* seg = const_cast<Segment*>(newSeq->findSegment(idx, localIdx));
            if (!seg)
                throw std::out_of_range("Index out of range");
            seg->data->set(item, localIdx);
            return newSeq;
        }

        LazySequence<T>* removeFirst() override
        {
            if (isEmpty())
                throw std::out_of_range("Empty sequence");
            if (totalLength.IsInfinite())
                throw std::logic_error("Cannot remove from infinite sequence");
            return getSubsequence(1, totalLength.GetFiniteValue() - 1);
        }

        LazySequence<T>* removeLast() override
        {
            if (isEmpty())
                throw std::out_of_range("Empty sequence");
            if (totalLength.IsInfinite())
                throw std::logic_error("Cannot remove from infinite sequence");
            return getSubsequence(0, totalLength.GetFiniteValue() - 2);
        }

        LazySequence<T>* removeAt(int index) override
        {
            Cardinal idx = Cardinal::Finite(index);
            if (totalLength.IsInfinite())
                throw std::logic_error("Cannot remove from infinite sequence");
            if (idx < Cardinal::Finite(0) || idx >= totalLength)
                throw std::out_of_range("Index out of range");

            LazySequence<T>* newSeq = new LazySequence<T>();
            bool removed = false;
            Cardinal prefix = Cardinal::Finite(0);

            for (const auto& seg : segments)
            {
                Cardinal segEnd = ordinalAdd(prefix, seg.length);
                if (!removed && prefix <= idx && idx < segEnd)
                {
                    Cardinal diff = ordinalSub(idx, prefix);
                    size_t localIdx = diff.GetFiniteValue();

                    if (localIdx > 0)
                    {
                        Sequence<T>* leftPart = seg.data->getSubsequence(0, localIdx - 1);
                        Segment left(*leftPart, Cardinal::Finite(localIdx));
                        delete leftPart;
                        newSeq->segments.push_back(left);
                    }

                    if (localIdx + 1 < static_cast<size_t>(seg.data->getLength()))
                    {
                        Sequence<T>* rightPart = seg.data->getSubsequence(localIdx + 1, seg.data->getLength() - 1);
                        Segment right(*rightPart, Cardinal::Finite(seg.data->getLength() - localIdx - 1));
                        delete rightPart;
                        newSeq->segments.push_back(right);
                    }

                    removed = true;
                }
                else
                {
                    Segment copy(seg);
                    newSeq->segments.push_back(copy);
                }
                prefix = segEnd;
            }

            if (!removed)
                throw std::out_of_range("Index out of range");

            newSeq->totalLength = ordinalSub(totalLength, Cardinal::Finite(1));
            return newSeq;
        }

        LazySequence<T>* clear() override
        {
            return new LazySequence<T>();
        }

        IEnumerator<T>* getEnumerator() const override
        {
            throw std::logic_error("Not implemented");
        }

        LazySequence<T>* copy() const override
        {
            LazySequence<T>* newSeq = new LazySequence<T>();
            newSeq->segments = segments;
            newSeq->totalLength = totalLength;
            return newSeq;
        }

        LazySequence<T>* concat(Sequence<T>* sequence) const override
        {
            LazySequence<T>* newSeq = copy();
            Sequence<T>* otherCopy = sequence->copy();
            Segment newSeg(*otherCopy, Cardinal::Finite(sequence->getLength()));
            delete otherCopy;
            newSeq->segments.push_back(newSeg);
            newSeq->totalLength = ordinalAdd(newSeq->totalLength, Cardinal::Finite(sequence->getLength()));
            return newSeq;
        }

        LazySequence<T>* concat(LazySequence<T>* lazySeq) const
        {
            LazySequence<T>* newSeq = copy();
            for (const auto& seg : lazySeq->segments)
            {
                Segment copy(seg);
                newSeq->segments.push_back(copy);
            }
            newSeq->totalLength = ordinalAdd(newSeq->totalLength, lazySeq->totalLength);
            return newSeq;
        }

        LazySequence<T>* getSubsequence(int startIndex, int endIndex) const override
        {
            return getSubsequence(Cardinal::Finite(startIndex), Cardinal::Finite(endIndex + 1));
        }

        LazySequence<T>* getSubsequence(Cardinal start, Cardinal end) const
        {
            if (start < Cardinal::Finite(0) || end <= start)
                throw std::out_of_range("Invalid range");
            if (totalLength.IsFinite() && end > totalLength)
                throw std::out_of_range("Index out of range");

            LazySequence<T>* newSeq = new LazySequence<T>();
            Cardinal current = Cardinal::Finite(0);
            Cardinal prefix = Cardinal::Finite(0);

            for (const auto& seg : segments)
            {
                Cardinal segEnd = ordinalAdd(prefix, seg.length);
                if (segEnd <= start)
                {
                    prefix = segEnd;
                    continue;
                }
                if (segEnd >= end)
                {
                    Cardinal segStartInRange = prefix < start ? start : prefix;
                    Cardinal segEndInRange = segEnd < end ? segEnd : end;

                    Cardinal diffStart = ordinalSub(segStartInRange, prefix);
                    Cardinal diffEnd = ordinalSub(segEndInRange, prefix);

                    size_t localStart = diffStart.IsFinite() ? diffStart.GetFiniteValue() : diffStart.GetFinitePart();
                    size_t localEnd = (diffEnd.IsFinite() ? diffEnd.GetFiniteValue() : diffEnd.GetFinitePart()) - 1;

                    Sequence<T>* subData = seg.data->getSubsequence(localStart, localEnd);
                    Segment subSeg(*subData, Cardinal::Finite(localEnd - localStart + 1));
                    delete subData;
                    newSeq->segments.push_back(subSeg);
                    current = ordinalAdd(current, Cardinal::Finite(localEnd - localStart + 1));
                    prefix = segEnd;
                    break;
                }

                Cardinal segStartInRange = prefix < start ? start : prefix;

                Cardinal diffStart = ordinalSub(segStartInRange, prefix);
                size_t localStart = diffStart.IsFinite() ? diffStart.GetFiniteValue() : diffStart.GetFinitePart();
                size_t localEnd = static_cast<size_t>(seg.data->getLength()) - 1;

                Sequence<T>* subData = seg.data->getSubsequence(localStart, localEnd);
                Segment subSeg(*subData, Cardinal::Finite(localEnd - localStart + 1));
                delete subData;
                newSeq->segments.push_back(subSeg);
                current = ordinalAdd(current, Cardinal::Finite(localEnd - localStart + 1));
                prefix = segEnd;
            }

            newSeq->totalLength = current;
            return newSeq;
        }

        LazySequence<T>* map(std::function<T(T)> func) const
        {
            LazySequence<T>* newSeq = new LazySequence<T>();
            Cardinal prefix = Cardinal::Finite(0);

            for (size_t i = 0; i < segments.size(); ++i)
            {
                const auto& seg = segments[i];
                if (seg.type == Segment::FiniteData)
                {
                    MutableArraySequence<T> mapped;
                    for (size_t j = 0; j < static_cast<size_t>(seg.data->getLength()); ++j)
                        mapped.append(func(seg.data->get(j)));
                    newSeq->segments.emplace_back(mapped, seg.length);
                }
                else
                {
                    auto thisPtr = this;
                    Cardinal capturedPrefix = prefix;
                    auto newGen = [thisPtr, func, capturedPrefix](const Sequence<T>& hist) -> T
                    {
                        size_t nextIdx = hist.getLength();
                        Cardinal globalIdx = ordinalAdd(capturedPrefix, Cardinal::Finite(nextIdx));
                        T val = thisPtr->get(globalIdx);
                        return func(val);
                    };
                    MutableArraySequence<T> initEmpty;
                    Segment newSeg(newGen, initEmpty, seg.length);
                    newSeq->segments.push_back(newSeg);
                }
                prefix = ordinalAdd(prefix, seg.length);
            }

            newSeq->totalLength = totalLength;
            return newSeq;
        }

        LazySequence<T>* where(std::function<bool(T)> pred) const
        {
            LazySequence<T>* newSeq = new LazySequence<T>();
            MutableArraySequence<T> filtered;
            size_t limit = totalLength.IsFinite() ? totalLength.GetFiniteValue() : 1000;

            for (size_t i = 0; i < limit; ++i)
            {
                Cardinal idx = Cardinal::Finite(i);
                T val = get(idx);
                if (pred(val))
                    filtered.append(val);
            }

            if (!filtered.isEmpty())
            {
                newSeq->segments.emplace_back(filtered, Cardinal::Finite(filtered.getLength()));
                newSeq->totalLength = Cardinal::Finite(filtered.getLength());
            }

            return newSeq;
        }

        Cardinal cardinalLength() const
        {
            return totalLength;
        }

        size_t getMaterializedCount() const
        {
            size_t cnt = 0;
            for (const auto& seg : segments)
                cnt += seg.data->getLength();
            return cnt;
        }
};


#endif