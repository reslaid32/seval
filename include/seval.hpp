/**
 * @file seval.hpp
 * @brief A header file providing functionality for evaluating numerical literals from a string.
 * 
 * This header contains several utility functions and templates for parsing and evaluating
 * decimal, hexadecimal, binary, and floating-point numbers, including handling signs, exponents, and prefixes.
 * The functions are designed to support both integral and floating-point types.
 */

#pragma once

#if !defined(SEVAL_HPP_LOADED)
#define SEVAL_HPP_LOADED

#include <iostream>

#include <cmath>

#if !defined(SEVAL_INLINE)
#define SEVAL_INLINE inline
#endif // SEVAL_INLINE

namespace seval {

namespace compatibility {

namespace static_assertion {
template <bool B>
struct StaticAssert;

template <>
struct StaticAssert<true> {};

#define compatibility_static_assert(expr, msg) \
    typedef compatibility::static_assertion::StaticAssert<(expr)> static_assert_failed_at_##__LINE__
} /* static_assertion */

namespace type_traits {
template <typename T> struct is_integral {
    static const bool value = false;
};
template <> struct is_integral<char> { static const bool value = true; };
template <> struct is_integral<signed char> { static const bool value = true; };
template <> struct is_integral<unsigned char> { static const bool value = true; };
template <> struct is_integral<short> { static const bool value = true; };
template <> struct is_integral<unsigned short> { static const bool value = true; };
template <> struct is_integral<int> { static const bool value = true; };
template <> struct is_integral<unsigned int> { static const bool value = true; };
template <> struct is_integral<long> { static const bool value = true; };
template <> struct is_integral<unsigned long> { static const bool value = true; };
template <> struct is_integral<long long> { static const bool value = true; };
template <> struct is_integral<unsigned long long> { static const bool value = true; };
template <> struct is_integral<bool> { static const bool value = true; };
#ifdef __SIZEOF_INT128__
template <> struct is_integral<__int128> { static const bool value = true; };
template <> struct is_integral<unsigned __int128> { static const bool value = true; };
#endif
template <> struct is_integral<wchar_t> { static const bool value = true; };

template <typename T> struct is_floating_point {
    static const bool value = false;
};
template <> struct is_floating_point<float> { static const bool value = true; };
template <> struct is_floating_point<double> { static const bool value = true; };
template <> struct is_floating_point<long double> { static const bool value = true; };
#if defined(__SIZEOF_FLOAT128__)
template <> struct is_floating_point<__float128> { static const bool value = true; };
#endif // __SIZEOF_FLOAT128__

template <typename T>
struct is_arithmetic {
    static const bool value = is_integral<T>::value || is_floating_point<T>::value;
};
} /* type_traits */

#if __cplusplus < 201103L
#pragma message("C++98/03 compatibility mode enabled")
    #include <stdint.h>
    #define _TypeTraitsSpace compatibility::type_traits
    #define _StatAssert compatibility_static_assert
#else
    #include <cstdint>
    #include <type_traits>
    #define _TypeTraitsSpace std
    #define _StatAssert static_assert
#endif
}

namespace internal {
/**
 * @enum Sign
 * @brief Represents the sign of a number (positive, negative, or none).
 */
enum Sign : int8_t {
    SIGN_NEGATIVE = -1, /**< Negative sign */
    SIGN_POSITIVE =  1, /**< Positive sign */
    SIGN_NONE     =  0, /**< No sign */
};

/**
 * @brief Decreases the index `i` by the specified count `cnt`.
 * @param i The index to be decreased.
 * @param cnt The number of characters to subtract from `i`.
 * This function adjusts the index `i` by subtracting `cnt` from it.
 */
SEVAL_INLINE void dec_(size_t& i, size_t cnt) {
    if (i >= cnt) {
        i -= cnt;
    } else {
        i = 0; // Prevent index from going below 0
    }
}

/**
 * @brief Increments the value of the given index by the specified count.
 * @param i The index to be incremented.
 * @param cnt The count to increment the index by.
 */
SEVAL_INLINE void inc_(size_t& i, size_t cnt) {
    if (cnt >= 0) { // Ensure the count is non-negative before incrementing
        i += cnt;
    }
}

/**
 * @brief Conditionally increments the value of the given index by the specified count.
 * @param cond The condition that determines whether the increment will happen.
 * @param i The index to be incremented.
 * @param cnt The count to increment the index by.
 * @details If the condition `cond` is true, the index `i` is incremented by `cnt` using `inc_`.
 */
SEVAL_INLINE void inc_if_(bool cond, size_t& i, size_t cnt) {
    if (cond) return inc_(i, cnt);
}

/**
 * @brief Decreases the index `i` by the specified count `cnt` if the condition is true.
 * @param cond The condition to check.
 * @param i The index to be decreased.
 * @param cnt The number of characters to subtract from `i`.
 * This function decreases `i` by `cnt` only if the condition `cond` is true.
 */
SEVAL_INLINE void dec_if_(bool cond, size_t& i, size_t cnt) {
    if (cond) return dec_(i, cnt);
}

/**
 * @brief Skips `cnt` characters in the string by incrementing the index.
 * @param i The index to increment.
 * @param cnt The number of characters to skip.
 */
SEVAL_INLINE void skip_(size_t& i, size_t cnt) {
    return inc_(i, cnt);
}

/**
 * @brief Advances the index by 1.
 * @param i The index to increment.
 */
SEVAL_INLINE void next_(size_t& i) {
    return inc_(i, 1);
}

/**
 * @brief Decrements the index by 1.
 * @param i The index to increment.
 */
SEVAL_INLINE void prev_(size_t& i) {
    return dec_(i, 1);
}

/**
 * @brief Determines the sign of the number at the current index in the string.
 * @param str The string being parsed.
 * @param i The current index in the string.
 * @return The sign of the number (negative, positive, or none).
 */
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

/**
 * @brief Checks if the character is a binary digit.
 * @param literal The character to check.
 * @return True if the character is a binary digit ('0' or '1'), otherwise false.
 */
SEVAL_INLINE bool is_binary_ch(const char literal) {
    return (literal == '0' || literal == '1');
}


/**
 * @brief Checks if the character is a decimal digit.
 * @param literal The character to check.
 * @return True if the character is a decimal digit, otherwise false.
 */
SEVAL_INLINE bool is_decimal_ch(const char literal) {
    return (literal >= '0' && literal <= '9');
}

/**
 * @brief Checks if the character is a lowercase hexadecimal digit.
 * @param literal The character to check.
 * @return True if the character is a lowercase hexadecimal digit, otherwise false.
 */
SEVAL_INLINE bool is_lower_hex_ch(const char literal) {
    return (literal >= 'a' && literal <= 'f');
}

/**
 * @brief Checks if the character is an uppercase hexadecimal digit.
 * @param literal The character to check.
 * @return True if the character is an uppercase hexadecimal digit, otherwise false.
 */
SEVAL_INLINE bool is_upper_hex_ch(const char literal) {
    return (literal >= 'A' && literal <= 'F');
}

/**
 * @brief Checks if the character is a valid hexadecimal digit.
 * @param literal The character to check.
 * @return True if the character is a hexadecimal digit (0-9, a-f, A-F), otherwise false.
 */
SEVAL_INLINE bool is_hexadecimal_ch(const char literal) {
    return (is_decimal_ch(literal) || is_lower_hex_ch(literal) || is_upper_hex_ch(literal));
}

/**
 * @brief Converts a decimal character to its integer value.
 * @param ch The decimal character to convert.
 * @return The integer value of the decimal character.
 */
template <typename T>
SEVAL_INLINE T evaluate_decimal_ch(const char ch) {
    return (ch - '0');
}

/**
 * @brief Converts a lowercase hexadecimal character to its integer value.
 * @param ch The lowercase hexadecimal character to convert.
 * @return The integer value of the lowercase hexadecimal character.
 */
template <typename T>
SEVAL_INLINE T evaluate_lower_hex_ch(const char ch) {
    return (ch - 'a' + 10);
}

/**
 * @brief Converts an uppercase hexadecimal character to its integer value.
 * @param ch The uppercase hexadecimal character to convert.
 * @return The integer value of the uppercase hexadecimal character.
 */
template <typename T>
SEVAL_INLINE T evaluate_upper_hex_ch(const char ch) {
    return (ch - 'A' + 10);
}

/**
 * @brief Converts a hexadecimal character to its integer value.
 * @param ch The hexadecimal character to convert.
 * @return The integer value of the hexadecimal character.
 */
template <typename T>
SEVAL_INLINE T evaluate_hexadecimal_ch(const char ch) {
    if (is_decimal_ch(ch)) return evaluate_decimal_ch<T>(ch);
    if (is_lower_hex_ch(ch)) return evaluate_lower_hex_ch<T>(ch);
    if (is_upper_hex_ch(ch)) return evaluate_upper_hex_ch<T>(ch);
    return 0;
}

/**
 * @brief Converts a decimal character to its integer value.
 * @param ch The decimal character to convert.
 * @return The integer value of the decimal character.
 */
template <typename T>
SEVAL_INLINE T evaluate_binary_ch(const char ch) {
    if (is_binary_ch(ch)) {
        return evaluate_decimal_ch<T>(ch);
    }
    return 0;
}

/**
 * @brief Converts any numeric character (decimal, hexadecimal, or binary) to its integer value.
 * @param ch The character to convert. Can be a decimal ('0'-'9'), lowercase hexadecimal ('a'-'f'), uppercase hexadecimal ('A'-'F'), or binary ('0' or '1').
 * @return The integer value of the character, or 0 if the character is invalid.
 */
template <typename T>
SEVAL_INLINE T evaluate_any_ch(const char ch) {
    if (is_decimal_ch(ch)) {
        return evaluate_decimal_ch<T>(ch);
    } else if (is_lower_hex_ch(ch)) {
        return evaluate_lower_hex_ch<T>(ch);
    } else if (is_upper_hex_ch(ch)) {
        return evaluate_upper_hex_ch<T>(ch);
    } else if (is_binary_ch(ch)) {
        return evaluate_binary_ch<T>(ch);
    }
    return 0; // Return 0 for invalid characters
}

/**
 * @brief Parses a decimal literal from the string.
 * @param str The string being parsed.
 * @param number The number to update with the parsed value.
 * @param i The current index in the string.
 */
template <typename T, typename StrT>
SEVAL_INLINE void evaluate_decimal_literal(StrT str, T& number, size_t& i) {
    #if __cplusplus >= 201703L
    if constexpr (_TypeTraitsSpace::is_integral<T>::value) {
        while (str[i] != '\0' && is_hexadecimal_ch(str[i])) {
            number = (number << 3) + (number << 1) + evaluate_decimal_ch<T>(str[i]);
            next_(i);
        }
    } else {
        while (str[i] != '\0' && is_hexadecimal_ch(str[i])) {
            number = number * 10 + evaluate_decimal_ch<T>(str[i]);
            next_(i);
        }
    }
    #else
    while (str[i] != '\0' && is_decimal_ch(str[i])) {
        number = number * 10 + evaluate_decimal_ch<T>(str[i]);
        next_(i);
    }
    #endif
}

/**
 * @brief Parses a floating-point literal from the string.
 * @param str The string being parsed.
 * @param number The number to update with the parsed value.
 * @param i The current index in the string.
 * @param decimalPlace The current decimal place.
 */
template <typename T, typename StrT>
SEVAL_INLINE void evaluate_floatpoint_literal(StrT str, T& number, size_t& i, T decimalPlace) {
    while (str[i] != '\0' && is_decimal_ch(str[i])) {
        number += evaluate_decimal_ch<T>(str[i]) * decimalPlace;
        decimalPlace /= static_cast<T>(10);
        next_(i);
    }
}

/**
 * @brief Parses an exponent part of a floating-point literal.
 * @param str The string being parsed.
 * @param number The number to update with the exponent.
 * @param i The current index in the string.
 */
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
        #if __cplusplus >= 201703L
            exponent = (exponent << 3) + (exponent << 1) + evaluate_decimal_ch<int>(str[i]);
        #else
            exponent = exponent * 10 + evaluate_decimal_ch<int>(str[i]);
        #endif /* C++17 */
            next_(i);
        }
        number *= std::pow(static_cast<T>(10), static_cast<T>(expSign * exponent));
    }
}

/**
 * @brief Parses a hexadecimal literal from the string.
 * @param str The string being parsed.
 * @param number The number to update with the parsed value.
 * @param i The current index in the string.
 */
template <typename T, typename StrT>
SEVAL_INLINE void evaluate_hexadecimal_literal(StrT str, T& number, size_t& i) {
#if __cplusplus >= 201703L
    if constexpr (_TypeTraitsSpace::is_integral<T>::value) {
        while (str[i] != '\0' && is_hexadecimal_ch(str[i])) {
            number = (number << 4) | evaluate_hexadecimal_ch<T>(str[i]);
            next_(i);
        }
    } else {
        while (str[i] != '\0' && is_hexadecimal_ch(str[i])) {
            next_(i);
        }
    }
#else
    while (str[i] != '\0' && is_hexadecimal_ch(str[i])) {
        number = number * 16 + evaluate_hexadecimal_ch<T>(str[i]);
        next_(i);
    }
#endif /* C++17 */
}

/**
 * @brief Parses a binary literal from the string.
 * @param str The string being parsed.
 * @param number The number to update with the parsed value.
 * @param i The current index in the string.
 */
template <typename T, typename StrT>
SEVAL_INLINE void evaluate_binary_literal(StrT str, T& number, size_t& i) {
#if __cplusplus >= 201703L
    if constexpr (_TypeTraitsSpace::is_integral<T>::value) {
        while (str[i] != '\0' && is_binary_ch(str[i])) {
            number = (number << 1) | evaluate_binary_ch<T>(str[i]);
            next_(i);
        }
    } else {
        while (str[i] != '\0' && is_binary_ch(str[i])) {
            next_(i);
        }
    }
#else
    while (str[i] != '\0' && is_binary_ch(str[i])) {
        number = number * 2 + evaluate_binary_ch<T>(str[i]);
        next_(i);
    }
#endif /* C++17 */
}

/**
 * @brief Determines the count based on whether the sign and prefix should be considered in the maximum length.
 * @param cnt The count to update based on the current index.
 * @param i The current index in the string.
 * @param consideSignAndPrefixInMaxLength Whether to include the sign and prefix in the maximum length calculation.
 */
SEVAL_INLINE void _get_cnti(size_t& cnt, size_t& i, bool consideSignAndPrefixInMaxLength) {
    if (consideSignAndPrefixInMaxLength) {
        cnt = i;  // If considering sign and prefix, set the count to the current index
    } else {
        cnt = 0;  // If not considering sign and prefix, set the count to 0
    }
}

/**
 * @brief Checks whether the iteration can continue based on the count and maximum length.
 * 
 * @param cnt The current count or iteration index.
 * @param maxLength The maximum allowed length for the iteration.
 * 
 * @return `true` if the current count is less than the maximum length, otherwise `false`.
 * 
 * @note This function helps to ensure that an iteration does not exceed the specified maximum length.
 */
SEVAL_INLINE bool _cnti_can_iterate(size_t& cnt, size_t& maxLength) {
    // std::cout << "Called can iterate with args: cnt(" << cnt << ")" << " maxLength(" << maxLength << ")\n";
    return cnt < maxLength;
}

SEVAL_INLINE void _cnti_next(size_t& cnt, size_t& i) {
    inc_(cnt,1);
    inc_(i,1);
}

/**
 * @brief Parses a decimal literal from the string with a specified maximum length.
 * @param str The string being parsed.
 * @param number The number to update with the parsed value.
 * @param i The current index in the string.
 * @param maxLength The maximum number of characters to read.
 * @param consideSignAndPrefixInMaxLength Whether to include the sign and prefix in the maximum length calculation.
 */
template <typename T, typename StrT>
SEVAL_INLINE void evaluate_decimal_literal_n(StrT str, T& number, size_t& i, size_t maxLength, bool consideSignAndPrefixInMaxLength = true) {
    size_t cnt = 0;
    _get_cnti(cnt, i, consideSignAndPrefixInMaxLength);

    #if __cplusplus >= 201703L
    if constexpr (_TypeTraitsSpace::is_integral<T>::value) {
        while (str[i] != '\0' && is_hexadecimal_ch(str[i]) && _cnti_can_iterate(cnt, maxLength)) {
            number = (number << 3) + (number << 1) + evaluate_decimal_ch<T>(str[i]);
            // next_(i);
            _cnti_next(cnt,i);
        }
    } else {
        while (str[i] != '\0' && is_hexadecimal_ch(str[i]) && _cnti_can_iterate(cnt, maxLength)) {
            number = number * 10 + evaluate_decimal_ch<T>(str[i]);
            // next_(i);
            _cnti_next(cnt,i);
        }
    }
    #else
    while (str[i] != '\0' && is_decimal_ch(str[i]) && _cnti_can_iterate(cnt, maxLength)) {
        number = number * 10 + evaluate_decimal_ch<T>(str[i]);
        // next_(i);
        _cnti_next(cnt,i);
    }
    #endif
}

/**
 * @brief Parses a floating-point literal from the string with a specified maximum length.
 * @param str The string being parsed.
 * @param number The number to update with the parsed value.
 * @param i The current index in the string.
 * @param decimalPlace The current decimal place.
 * @param maxLength The maximum number of characters to read.
 * @param consideSignAndPrefixInMaxLength Whether to include the sign and prefix in the maximum length calculation.
 */
template <typename T, typename StrT>
SEVAL_INLINE void evaluate_floatpoint_literal_n(StrT str, T& number, size_t& i, T decimalPlace, size_t maxLength, bool consideSignAndPrefixInMaxLength = true) {
    size_t cnt = 0;
    _get_cnti(cnt, i, consideSignAndPrefixInMaxLength);

    while (str[i] != '\0' && is_decimal_ch(str[i]) && _cnti_can_iterate(cnt, maxLength)) {
        number += evaluate_decimal_ch<T>(str[i]) * decimalPlace;
        decimalPlace /= static_cast<T>(10);
        /* next_(i) */ _cnti_next(cnt,i);
    }
}

/**
 * @brief Parses the exponent part of a floating-point literal with a specified maximum length.
 * @param str The string being parsed.
 * @param number The number to update with the exponent.
 * @param i The current index in the string.
 * @param maxLength The maximum number of characters to read.
 * @param consideSignAndPrefixInMaxLength Whether to include the sign and prefix in the maximum length calculation.
 */
template <typename T, typename StrT>
SEVAL_INLINE void evaluate_exponent_literal_n(StrT str, T& number, size_t& i, size_t maxLength, bool consideSignAndPrefixInMaxLength = true) {
    size_t cnt = 0;
    _get_cnti(cnt, i, consideSignAndPrefixInMaxLength);

    if (str[i] == 'e' || str[i] == 'E') {
        /* next_(i) */ _cnti_next(cnt,i);
        Sign expSign = SIGN_POSITIVE;

        if (str[i] == '-') {
            expSign = SIGN_NEGATIVE;
            /* next_(i) */ _cnti_next(cnt,i);
        } else if (str[i] == '+') {
            /* next_(i) */ _cnti_next(cnt,i);
        }

        int exponent = 0;
        while (str[i] != '\0' && is_decimal_ch(str[i]) && _cnti_can_iterate(cnt, maxLength)) {
        #if __cplusplus >= 201703L
            exponent = (exponent << 3) + (exponent << 1) + evaluate_decimal_ch<int>(str[i]);
        #else
            exponent = exponent * 10 + evaluate_decimal_ch<int>(str[i]);
        #endif
            /* next_(i) */ _cnti_next(cnt,i);
        }
        number *= std::pow(static_cast<T>(10), static_cast<T>(expSign * exponent));
    }
}

/**
 * @brief Parses a hexadecimal literal from the string with a specified maximum length.
 * @param str The string being parsed.
 * @param number The number to update with the parsed value.
 * @param i The current index in the string.
 * @param maxLength The maximum number of characters to read.
 * @param consideSignAndPrefixInMaxLength Whether to include the sign and prefix in the maximum length calculation.
 */
template <typename T, typename StrT>
SEVAL_INLINE void evaluate_hexadecimal_literal_n(StrT str, T& number, size_t& i, size_t maxLength, bool consideSignAndPrefixInMaxLength = true) {
    size_t cnt = 0;
    _get_cnti(cnt, i, consideSignAndPrefixInMaxLength);
#if __cplusplus >= 201703L
    if constexpr (_TypeTraitsSpace::is_integral<T>::value) {
        while (str[i] != '\0' && is_hexadecimal_ch(str[i]) && _cnti_can_iterate(cnt, maxLength)) {
            number = (number << 4) | evaluate_hexadecimal_ch<T>(str[i]);
            /* next_(i) */ _cnti_next(cnt,i);
        }
    } else {
        while (str[i] != '\0' && is_hexadecimal_ch(str[i]) && _cnti_can_iterate(cnt, maxLength)) {
            /* next_(i) */ _cnti_next(cnt,i);
        }
    }
#else
    while (str[i] != '\0' && is_hexadecimal_ch(str[i]) && _cnti_can_iterate(cnt, maxLength)) {
        number = number * 16 + evaluate_hexadecimal_ch<T>(str[i]);
        /* next_(i) */ _cnti_next(cnt,i);
    }
#endif
}

/**
 * @brief Parses a binary literal from the string with a specified maximum length.
 * @param str The string being parsed.
 * @param number The number to update with the parsed value.
 * @param i The current index in the string.
 * @param maxLength The maximum number of characters to read.
 * @param consideSignAndPrefixInMaxLength Whether to include the sign and prefix in the maximum length calculation.
 */
template <typename T, typename StrT>
SEVAL_INLINE void evaluate_binary_literal_n(StrT str, T& number, size_t& i, size_t maxLength, bool consideSignAndPrefixInMaxLength = true) {
    size_t cnt = 0;
    _get_cnti(cnt, i, consideSignAndPrefixInMaxLength);
#if __cplusplus >= 201703L
    if constexpr (_TypeTraitsSpace::is_integral<T>::value) {
        while (str[i] != '\0' && is_binary_ch(str[i]) && _cnti_can_iterate(cnt, maxLength)) {
            number = (number << 1) | evaluate_binary_ch<T>(str[i]);
            /* next_(i) */ _cnti_next(cnt,i);
        }
    } else {
        while (str[i] != '\0' && is_binary_ch(str[i]) && _cnti_can_iterate(cnt, maxLength)) {
            /* next_(i) */ _cnti_next(cnt,i);
        }
    }
#else
    while (str[i] != '\0' && is_binary_ch(str[i]) && _cnti_can_iterate(cnt, maxLength)) {
        number = number * 2 + evaluate_binary_ch<T>(str[i]);
        /* next_(i) */ _cnti_next(cnt,i);
    }
#endif
}

/**
 * @brief Checks if the string has a hexadecimal prefix ("0x" or "0X").
 * @param literal The string to check.
 * @param i The current index in the string.
 * @return True if the string has a hexadecimal prefix, otherwise false.
 */
template <typename StrT>
SEVAL_INLINE bool has_hexadecimal_prefix(StrT literal, size_t& i) {
    return (literal[i] == '0') && (literal[i + 1] == 'x' || literal[i + 1] == 'X');
}

/**
 * @brief Checks if the string has a binary prefix ("0b" or "0B").
 * @param literal The string to check.
 * @param i The current index in the string.
 * @return True if the string has a binary prefix, otherwise false.
 */
template <typename StrT>
SEVAL_INLINE bool has_binary_prefix(StrT literal, size_t& i) {
    return (literal[i] == '0') && (literal[i + 1] == 'b' || literal[i + 1] == 'B');
}

/**
 * @brief Checks if the string has a sign ("+" or "-") at the current index.
 * @param literal The string to check.
 * @param i The current index in the string.
 * @return True if the string has a sign, otherwise false.
 */
template <typename StrT>
SEVAL_INLINE bool has_sign(StrT literal, size_t i) {
    return literal[i] == '-' || literal[i] == '+';
}

} /* internal */

/**
 * @brief Evaluates a number literal from the string, considering various literal types (decimal, binary, hexadecimal, floating-point, and exponent).
 *        Optionally includes the sign and prefix in the evaluation.
 * 
 * @param str The string to evaluate.
 * @param consideSign Whether to consider the sign of the number (default is `true`).
 * @param consideFloatPoint Whether to consider floating-point literals (default is `true`).
 * @param consideHex Whether to consider hexadecimal literals (default is `true`).
 * @param consideBinary Whether to consider binary literals (default is `true`).
 * @param consideExponent Whether to consider exponent literals (default is `true`).
 * 
 * @return The evaluated number, which may be an integral or floating-point type based on `T`.
 * 
 * @note The function handles both integral and floating-point types for `T`. If `consideSign` is `true`, it evaluates the sign (`+` or `-`) in the string. If `consideBinary` and `consideHex` are `true`, it recognizes binary ("0b" or "0B") and hexadecimal ("0x" or "0X") prefixes, respectively. Floating-point literals and exponent parts are handled when `consideFloatPoint` and `consideExponent` are `true`. The return value is multiplied by the sign if `consideSign` is `true`.
 * 
 * @throws _StatAssert If `T` is not an arithmetic type (integral or floating-point).
 */
template <typename T, typename StrT>
SEVAL_INLINE T evaluate(StrT str, bool consideSign = true, bool consideFloatPoint = true, bool consideHex = true, bool consideBinary = true, bool consideExponent = true) {
    _StatAssert(_TypeTraitsSpace::is_arithmetic<T>::value, "Template parameter T must be an arithmetic type (integral or floating-point).");
    
    T number = 0;
    size_t i = 0;

    internal::Sign sign = internal::SIGN_POSITIVE;

    if (consideSign) {
        internal::Sign intermediateSign = internal::get_sign<StrT>(str, i);
        if (intermediateSign != internal::SIGN_NONE) {
            sign = intermediateSign;
            internal::skip_(i, 1); /* Eat: sigSym (+ or -) */
        }
    }

    if (consideBinary && internal::has_binary_prefix<StrT>(str, i)) {
        internal::skip_(i, 2); // Eat: binaryPrefix (0b or 0B) 
        internal::evaluate_binary_literal<T, StrT>(str, number, i);
    } else if (consideHex && internal::has_hexadecimal_prefix<StrT>(str, i)) {
        internal::skip_(i, 2); /* Eat: hexadecimalPrefix (0x or 0X) */
        internal::evaluate_hexadecimal_literal<T, StrT>(str, number, i);
    } else {
        internal::evaluate_decimal_literal<T, StrT>(str, number, i);
    }

    if (_TypeTraitsSpace::is_floating_point<T>::value && consideFloatPoint && str[i] == '.') {
        internal::next_(i);
        T decimalPlace = static_cast<T>(0.1);
        internal::evaluate_floatpoint_literal<T, StrT>(str, number, i, decimalPlace);
    }

    if (_TypeTraitsSpace::is_floating_point<T>::value && consideFloatPoint && consideExponent) {
        internal::evaluate_exponent_literal<T, StrT>(str, number, i);
    }

    return consideSign ? number * sign : number;
}

/**
 * @brief Evaluates a number literal from the string with a maximum character length.
 *        It supports decimal, binary, hexadecimal, floating-point, and exponent literals.
 *        Optionally considers the sign and prefix of the number.
 * 
 * @param str The string to evaluate.
 * @param maxLength The maximum number of characters to read from the string (default is `SIZE_MAX`).
 * @param consideSignAndPrefixInMaxLength Whether to consider the sign and prefix in the `maxLength` check (default is `true`).
 * @param consideSign Whether to consider the sign of the number (default is `true`).
 * @param consideFloatPoint Whether to consider floating-point literals (default is `true`).
 * @param consideHex Whether to consider hexadecimal literals (default is `true`).
 * @param consideBinary Whether to consider binary literals (default is `true`).
 * @param consideExponent Whether to consider exponent literals (default is `true`).
 * 
 * @return The evaluated number.
 * 
 * @note The function handles both integral and floating-point types for `T`. The sign and prefix (e.g., "+" or "-") are handled if `consideSign` is `true`. The function can also handle binary and hexadecimal prefixes ("0b" or "0x") if `consideBinary` and `consideHex` are `true`, respectively. If `consideFloatPoint` is `true`, it also processes floating-point literals. Additionally, the function handles exponent parts if `consideExponent` is `true`.
 * 
 * @throws _StatAssert If `T` is not an arithmetic type (integral or floating-point).
 */
template <typename T, typename StrT>
SEVAL_INLINE T evaluate_n(StrT str, size_t maxLength = SIZE_MAX, bool consideSignAndPrefixInMaxLength = true, bool consideSign = true, bool consideFloatPoint = true, bool consideHex = true, bool consideBinary = true, bool consideExponent = true) {
    _StatAssert(_TypeTraitsSpace::is_arithmetic<T>::value, "Template parameter T must be an arithmetic type (integral or floating-point).");

    T number = 0;
    size_t i = 0;

    internal::Sign sign = internal::SIGN_POSITIVE;

    // Handle the sign of the number if considered
    if (consideSign) {
        internal::Sign intermediateSign = internal::get_sign<StrT>(str, i);
        if (intermediateSign != internal::SIGN_NONE) {
            sign = intermediateSign;
            internal::skip_(i, 1); /* Eat: sigSym (+ or -) */
        }
    }

    // Process binary or hexadecimal literals if the corresponding flags are set
    if (consideBinary && internal::has_binary_prefix<StrT>(str, i)) {
        internal::skip_(i, 2); // Eat: binaryPrefix (0b or 0B)
        internal::evaluate_binary_literal_n<T, StrT>(str, number, i, maxLength, consideSignAndPrefixInMaxLength);
    } else if (consideHex && internal::has_hexadecimal_prefix<StrT>(str, i)) {
        internal::skip_(i, 2); /* Eat: hexadecimalPrefix (0x or 0X) */
        internal::evaluate_hexadecimal_literal_n<T, StrT>(str, number, i, maxLength, consideSignAndPrefixInMaxLength);
    } else {
        internal::evaluate_decimal_literal_n<T, StrT>(str, number, i, maxLength, consideSignAndPrefixInMaxLength);
    }

    // Process floating-point literals if required and within the maxLength
    if (_TypeTraitsSpace::is_floating_point<T>::value && consideFloatPoint && str[i] == '.' && maxLength > i) {
        internal::next_(i);
        T decimalPlace = static_cast<T>(0.1);
        internal::evaluate_floatpoint_literal_n<T, StrT>(str, number, i, decimalPlace, maxLength, consideSignAndPrefixInMaxLength);
    }

    // Process exponent literals if required and within the maxLength
    if (_TypeTraitsSpace::is_floating_point<T>::value && consideFloatPoint && consideExponent) {
        internal::evaluate_exponent_literal_n<T, StrT>(str, number, i, maxLength, consideSignAndPrefixInMaxLength);
    }

    // Return the final evaluated number, considering the sign if applicable
    return consideSign ? number * sign : number;
}

} /* seval */

#endif // SEVAL_HPP_LOADED
