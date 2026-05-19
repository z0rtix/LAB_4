#ifndef STREAM_H
#define STREAM_H

#include "../LAB_2/Sequence.h"

#include <stdexcept>


template <class T>
class ReadOnlyStream {
    public:
        explicit ReadOnlyStream(const Sequence<T> *source) : source(source), position(0), open(false), ended(false) {}

        void Open() {
            open = true;
            ended = false;
        }

        void Close() {
            open = false;
        }

        bool IsEndOfStream() const {
            if (!open) return true;
            return ended;
        }

        T Read() {
            if (!open) {
                throw std::logic_error("Stream is not open");
            } else if (ended) {
                throw std::out_of_range("End of stream reached");
            }

            try {
                T value = source->get(position);
                position++;
                return value;
            } catch (const std::out_of_range &) {
                ended = true;
                throw std::out_of_range("End of stream reached");
            }
        }

        size_t GetPosition() const { return position; }
        bool IsCanSeek() const { return true; }

        size_t Seek(size_t index) {
            if (!open) {
                throw std::logic_error("Stream is not open");
            } else if (ended) {
                throw std::out_of_range("Cannot seek after end");
            }

            position = index;
            ended = false;

            return position;
        }

    private:
        const Sequence<T> *source;
        size_t position;

        bool open;
        bool ended;
};


template <class T>
class WriteOnlyStream {
    public:
        explicit WriteOnlyStream(Sequence<T> *dest) : destination(dest), position(0), open(false) {}

        void Open() { open = true; }
        void Close() { open = false; }

        size_t Write(const T &item) {
            if (!open) throw std::logic_error("Stream is not open");

            destination->append(item);
            position++;

            return position;
        }

        size_t GetPosition() const { return position; }

    private:
        Sequence<T> *destination;
        
        size_t position;
        bool open;
};


#endif