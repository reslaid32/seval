/* seval.hpp */
#pragma once

#include <iostream>
#include <type_traits>
#include <cmath>

#if !defined(SEVAL_INLINE)
#define SEVAL_INLINE inline
#endif // SEVAL_INLINE


namespace seval {
enum Sign : int8_t {
    SIGN_NEGATIVE = -1,
    SIGN_POSITIVE =  1,
    SIGN_NONE     =  0,
};

SEVAL_INLINE void skip_(size_t& i, size_t cnt) {
    i += cnt;
}

SEVAL_INLINE void next_(size_t& i) {
    skip_(i, 1);
}

template <typename StrT>
Sign get_sign(StrT str, size_t& i) {
    if (str[i] == '-') {
        return SIGN_NEGATIVE;
    } else if (str[i] == '+') {
        return SIGN_POSITIVE;
    } else {
        return SIGN_NONE;
    }
}

SEVAL_INLINE bool is_decimal_ch(const char literal) {
    return (literal >= '0' && literal <= '9');
}

SEVAL_INLINE bool is_lower_hex_ch(const char literal) {
    return (literal >= 'a' && literal <= 'f');
}

SEVAL_INLINE bool is_upper_hex_ch(const char literal) {
    return (literal >= 'A' && literal <= 'F');
}

SEVAL_INLINE bool is_hexadecimal_ch(const char literal) {
    return (is_decimal_ch(literal) || is_lower_hex_ch(literal) || is_upper_hex_ch(literal));
}

template <typename T>
SEVAL_INLINE T evaluate_decimal_ch(const char ch) {
    // if (!is_decimal_ch(ch)) return 0;
    return (ch - '0');
}

template <typename T>
SEVAL_INLINE T evaluate_lower_hex_ch(const char ch) {
    return (ch - 'a' + 10);
}

template <typename T>
SEVAL_INLINE T evaluate_upper_hex_ch(const char ch) {
    return (ch - 'A' + 10);
}

template <typename T>
SEVAL_INLINE T evaluate_hexadecimal_ch(const char ch) {
    if (is_decimal_ch(ch)) return evaluate_decimal_ch<T>(ch);
    if (is_lower_hex_ch(ch)) return evaluate_lower_hex_ch<T>(ch);
    if (is_upper_hex_ch(ch)) return evaluate_upper_hex_ch<T>(ch);
    return 0;
}

template <typename T, typename StrT>
SEVAL_INLINE void evaluate_decimal_literal(StrT str, T& number, size_t& i) {
    while (str[i] != '\0' && is_decimal_ch(str[i])) {
        number = number * 10 + evaluate_decimal_ch<T>(str[i]);
        next_(i);
    }
}

template <typename T, typename StrT>
SEVAL_INLINE void evaluate_floatpoint_literal(StrT str, T& number, size_t& i, T decimalPlace) {
    while (str[i] != '\0' && is_decimal_ch(str[i])) {
        number += evaluate_decimal_ch<T>(str[i]) * decimalPlace;
        decimalPlace /= static_cast<T>(10);
        next_(i);
    }
}

template <typename T, typename StrT>
SEVAL_INLINE void evaluate_exponent_literal(StrT str, T& number, size_t& i) {
    if (str[i] == 'e' || str[i] == 'E') {
        next_(i);
        Sign expSign = SIGN_POSITIVE;

        if (str[i] == '-') {
            expSign = SIGN_NEGATIVE;
            next_(i);
        } else if (str[i] == '+') {
            next_(i);
        }

        int exponent = 0;
        while (str[i] != '\0' && is_decimal_ch(str[i])) {
            exponent = exponent * 10 + evaluate_decimal_ch<int>(str[i]);
            next_(i);
        }
        number *= std::pow(static_cast<T>(10), static_cast<T>(expSign * exponent));
    }
}

template <typename T, typename StrT>
SEVAL_INLINE void evaluate_hexadecimal_literal(StrT str, T& number, size_t& i) {
    while (str[i] != '\0' && is_hexadecimal_ch(str[i])) {
        number = number * 16 + evaluate_hexadecimal_ch<T>(str[i]);
        next_(i);
    }
}

template <typename T, typename StrT>
SEVAL_INLINE void evaluate_binary_literal(StrT str, T& number, size_t& i) {
    if constexpr (std::is_integral<T>::value) {
        while (str[i] != '\0' && (str[i] == '0' || str[i] == '1')) {
            number = (number << 1) | evaluate_decimal_ch<T>(str[i]);
            next_(i);
        }
    } else {
        while (str[i] != '\0' && (str[i] == '0' || str[i] == '1')) {
            next_(i);
        }
    }
}

template <typename StrT>
SEVAL_INLINE bool has_hexadecimal_prefix(StrT literal, size_t& i) {
    return (literal[i] == '0') && (literal[i + 1] == 'x' || literal[i + 1] == 'X');
}

template <typename StrT>
SEVAL_INLINE bool has_binary_prefix(StrT literal, size_t& i) {
    return (literal[i] == '0') && (literal[i + 1] == 'b' || literal[i + 1] == 'B');
}

template <typename T, typename StrT>
SEVAL_INLINE T evaluate(StrT str, bool consideSign = true, bool consideFloatPoint = true, bool consideHex = true, bool consideBinary = true, bool consideExponent = true) {
    static_assert(std::is_arithmetic<T>::value, "Template parameter T must be an arithmetic type (integral or floating-point).");
    
    T number = 0;
    size_t i = 0;

    Sign sign = SIGN_POSITIVE;

    if (consideSign) {
        Sign intermediateSign = get_sign<StrT>(str, i);
        if (intermediateSign != SIGN_NONE) {
            sign = intermediateSign;
            skip_(i, 1); /* Eat: sigSym (+ or -) */
        }
    }

    if (consideBinary && has_binary_prefix<StrT>(str, i)) {
        skip_(i, 2); // Eat: binaryPrefix (0b or 0B) 
        evaluate_binary_literal<T, StrT>(str, number, i);
    } else if (consideHex && has_hexadecimal_prefix<StrT>(str, i)) {
        skip_(i, 2); /* Eat: hexadecimalPrefix (0x or 0X) */
        evaluate_hexadecimal_literal<T, StrT>(str, number, i);
    } else {
        evaluate_decimal_literal<T, StrT>(str, number, i);
    }

    if (std::is_floating_point<T>::value && consideFloatPoint && str[i] == '.') {
        next_(i);
        T decimalPlace = static_cast<T>(0.1);
        evaluate_floatpoint_literal<T, StrT>(str, number, i, decimalPlace);
    }

    if (std::is_floating_point<T>::value && consideFloatPoint && consideExponent) {
        evaluate_exponent_literal<T, StrT>(str, number, i);
    }

    return consideSign ? number * sign : number;
}
} /* seval */