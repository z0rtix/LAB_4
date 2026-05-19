#ifndef CARDINAL_H
#define CARDINAL_H

#include <cstddef>


enum class CardinalType {
	Finite,
	Infinite
};


class Cardinal {
    public:
        static Cardinal Finite(size_t value);
        static Cardinal Infinite();

        bool IsFinite() const;
        size_t GetValue() const;

    private:
        Cardinal(CardinalType type, size_t value = 0);
        
        CardinalType type;
        size_t value;
};


#endif