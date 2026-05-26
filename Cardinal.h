#ifndef CARDINAL_H
#define CARDINAL_H

#include <cstddef>
#include <stdexcept>


class Cardinal {
    public:
        Cardinal();

        static Cardinal Finite(size_t value);
        static Cardinal Infinite();
        static Cardinal InfinitePlus(size_t n);
        static Cardinal Omega(size_t omegaCoeff, size_t finitePart);

        bool IsFinite() const;
        bool IsInfinite() const;

        size_t GetFiniteValue() const;
        size_t GetOmegaCoeff() const;
        size_t GetFinitePart() const;

        Cardinal operator+(const Cardinal& other) const;
        Cardinal operator+(size_t n) const;
        Cardinal operator-(const Cardinal& other) const;
        Cardinal operator-(size_t n) const;

        bool operator==(const Cardinal& other) const;
        bool operator!=(const Cardinal& other) const;
        bool operator<(const Cardinal& other) const;
        bool operator<=(const Cardinal& other) const;
        bool operator>(const Cardinal& other) const;
        bool operator>=(const Cardinal& other) const;

    private:
        Cardinal(bool finite, size_t omegaCoeff, size_t finitePart);

        bool finite;
        
        size_t omegaCoeff;
        size_t finitePart;
};


#endif