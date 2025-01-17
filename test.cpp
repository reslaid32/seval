#include <iostream>
#include <cassert>
#include <stdint.h>
#include "seval.hpp"

template <typename T>
bool floatpoint_compare(T f1, T f2, T deviation = 1e-6) {
    return (f1 - f2) <= deviation && (f2 - f1) <= deviation;
}

void seval_test() {
    /* 8 BIT */
    {
        assert((seval::evaluate<int8_t, const char*>("127")) == 127);  /* 127 because int8_t is signed number */
        assert((seval::evaluate<uint8_t, const char*>("255")) == 255);
        assert((seval::evaluate<int8_t, const char*>("-128")) == -128);
        assert((seval::evaluate<uint8_t, const char*>("0")) == 0);
    }

    /* 16 BIT */
    {
        assert((seval::evaluate<int16_t, const char*>("32767")) == 32767);
        assert((seval::evaluate<uint16_t, const char*>("65535")) == 65535);
        assert((seval::evaluate<int16_t, const char*>("-32768")) == -32768);
        assert((seval::evaluate<uint16_t, const char*>("0")) == 0);
    }

    /* 32 BIT */
    {
        assert((seval::evaluate<int32_t, const char*>("2147483647")) == 2147483647);
        assert((seval::evaluate<uint32_t, const char*>("4294967295")) == 4294967295U);
        assert((seval::evaluate<int32_t, const char*>("-2147483648")) == -2147483648);
        assert((seval::evaluate<uint32_t, const char*>("0")) == 0);
    }

    /* 64 BIT */
    {
        assert((seval::evaluate<int64_t, const char*>("9223372036854775807")) == 9223372036854775807LL);
        assert((seval::evaluate<uint64_t, const char*>("18446744073709551615")) == 18446744073709551615ULL);
        assert((seval::evaluate<int64_t, const char*>("-9223372036854775808")) == -9223372036854775807LL - 1);
        assert((seval::evaluate<uint64_t, const char*>("0")) == 0);
    }
    /* HEXADECIMAL */
    {
        assert((seval::evaluate<int, const char*>("123") == 123)); /* without prefix */
        assert((seval::evaluate<int, const char*>("0x123") == 0x123)); /* with prefix */

        assert((seval::evaluate<int, const char*>("-123") == -123)); /* without prefix */
        assert((seval::evaluate<int, const char*>("-0x123") == -0x123)); /* with prefix */

        assert((seval::evaluate<int, const char*>("0xA3F") == 0xA3F));
        assert((seval::evaluate<int, const char*>("0x1aF") == 0x1AF));
    }
    /* FLOAT POINT */
    {
        assert(floatpoint_compare(seval::evaluate<float, const char*>("3.14"), 3.14f));
        assert(floatpoint_compare(seval::evaluate<float, const char*>("3.14"), 3.14f));
        assert(floatpoint_compare(seval::evaluate<float, const char*>("-3.14"), -3.14f));
        assert(floatpoint_compare(seval::evaluate<double, const char*>("2.718281828459045"), 2.718281828459045));
        assert(floatpoint_compare(seval::evaluate<double, const char*>("-2.718281828459045"), -2.718281828459045));
        assert(floatpoint_compare(seval::evaluate<long double, const char*>("1.618033988749895"), 1.618033988749895L));
        assert(floatpoint_compare(seval::evaluate<long double, const char*>("-1.618033988749895"), -1.618033988749895L));
    }
    /* FLOAT POINT WITH EXPONENT */
    {
        assert(floatpoint_compare(seval::evaluate<float, const char*>("3.14e2"), 314.0f));
        assert(floatpoint_compare(seval::evaluate<float, const char*>("-3.14E-2"), -0.0314f));
    }
    /* BINARY */
    {
        assert((seval::evaluate<int, const char*>("0b1101") == 0b1101));
        assert((seval::evaluate<int, const char*>("0b101010") == 0b101010));
        assert((seval::evaluate<int, const char*>("0b100000000") == 0b100000000));
        assert((seval::evaluate<int, const char*>("0b11111111") == 0b11111111));
        assert((seval::evaluate<int, const char*>("-0b1101") == -0b1101));
        assert((seval::evaluate<int, const char*>("-0b101010") == -0b101010));
    }
}

int main() {
    seval_test();
    std::cout << "All tests passed!" << std::endl;
    return 0;
}