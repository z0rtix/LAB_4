#include "Cardinal.h"

#include <stdexcept>


Cardinal::Cardinal() : finite(true), omegaCoeff(0), finitePart(0) {}

Cardinal::Cardinal(bool finite, size_t omegaCoeff, size_t finitePart) : finite(finite), omegaCoeff(omegaCoeff), finitePart(finitePart) {
    if (!finite && omegaCoeff == 0) {
        throw std::logic_error("Infinite Cardinal must have omegaCoeff > 0");
	} else if (finite && omegaCoeff != 0) {
        throw std::logic_error("Finite Cardinal must have omegaCoeff == 0");
	}
}


Cardinal Cardinal::Finite(size_t value) {
    return Cardinal(true, 0, value);
}

Cardinal Cardinal::Infinite() {
    return Cardinal(false, 1, 0);
}

Cardinal Cardinal::InfinitePlus(size_t n) {
    return Cardinal(false, 1, n);
}

Cardinal Cardinal::Omega(size_t omegaCoeff, size_t finitePart) {
    return Cardinal(false, omegaCoeff, finitePart);
}

bool Cardinal::IsFinite() const {
	return finite;
}

bool Cardinal::IsInfinite() const {
	return !finite;
}

size_t Cardinal::GetFiniteValue() const {
    if (!finite) throw std::logic_error("Cardinal is not finite");

    return finitePart;
}

size_t Cardinal::GetOmegaCoeff() const {
	return omegaCoeff;
}

size_t Cardinal::GetFinitePart() const {
	return finitePart;
}


Cardinal Cardinal::operator+(const Cardinal& other) const {
    if (finite && other.finite) {
        return Finite(finitePart + other.finitePart);
	}

    return Omega(omegaCoeff + other.omegaCoeff, finitePart + other.finitePart);
}

Cardinal Cardinal::operator+(size_t n) const {
    if (finite) {
        return Finite(finitePart + n);
	}

    return Omega(omegaCoeff, finitePart + n);
}

Cardinal Cardinal::operator-(const Cardinal& other) const {
    if (finite && other.finite) {
        if (other.finitePart > finitePart) {
            throw std::logic_error("Subtraction would make finite cardinal negative");
		}
        return Finite(finitePart - other.finitePart);
    } else if (!finite && !other.finite && omegaCoeff == other.omegaCoeff) {
        if (other.finitePart > finitePart) {
            throw std::logic_error("Subtraction would make infinite cardinal's finite part negative");
		}
		return Omega(omegaCoeff, finitePart - other.finitePart);
    } else {
        throw std::logic_error("Cannot subtract these cardinals");
    }
}

Cardinal Cardinal::operator-(size_t n) const {
    if (finite) {
        if (n > finitePart) {
            throw std::logic_error("Subtraction would make finite cardinal negative");
		}
		return Finite(finitePart - n);
    }
    if (n > finitePart) {
        throw std::logic_error("Cannot subtract beyond finite part of infinite cardinal");
	}
	return Omega(omegaCoeff, finitePart - n);
}

bool Cardinal::operator==(const Cardinal& other) const {
    return finite == other.finite && omegaCoeff == other.omegaCoeff && finitePart == other.finitePart;
}

bool Cardinal::operator!=(const Cardinal& other) const {
    return !(*this == other);
}

bool Cardinal::operator<(const Cardinal& other) const {
    if (finite != other.finite) return finite;
    if (finite) return finitePart < other.finitePart;
    if (omegaCoeff != other.omegaCoeff) return omegaCoeff < other.omegaCoeff;
    return finitePart < other.finitePart;
}

bool Cardinal::operator<=(const Cardinal& other) const {
    return *this < other || *this == other;
}

bool Cardinal::operator>(const Cardinal& other) const {
    return !(*this <= other);
}

bool Cardinal::operator>=(const Cardinal& other) const {
    return !(*this < other);
}