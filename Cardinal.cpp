#include "Cardinal.h"

#include <stdexcept>


Cardinal::Cardinal(CardinalType t, size_t v) : type(t), value(v) {}


Cardinal Cardinal::Finite(size_t value) {
	return Cardinal(CardinalType::Finite, value);
}

Cardinal Cardinal::Infinite() {
	return Cardinal(CardinalType::Infinite);
}

bool Cardinal::IsFinite() const {
	return type == CardinalType::Finite;
}

size_t Cardinal::GetValue() const {
	if (!IsFinite()) throw std::logic_error("Cannot get value of infinite cardinal");

	return value;
}