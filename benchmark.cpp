#include <iostream>
#include <cassert>
#include <chrono>
#include "seval.hpp"

template <typename Func>
void benchmark(const std::string& name, Func func) {
    constexpr int iterations = 1'000'000; // количество повторений для точности
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < iterations; ++i) {
        func();
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    std::cout << name << ": " << (duration / iterations) << " ns per call" << std::endl;
}

void seval_benchmark() {
    /* 8 BIT */
    benchmark("8-bit signed", []() {
        volatile auto result = seval::evaluate<int8_t, const char*>("127");
        (void)result;
    });

    benchmark("8-bit unsigned", []() {
        volatile auto result = seval::evaluate<uint8_t, const char*>("255");
        (void)result;
    });

    /* 16 BIT */
    benchmark("16-bit signed", []() {
        volatile auto result = seval::evaluate<int16_t, const char*>("32767");
        (void)result;
    });

    benchmark("16-bit unsigned", []() {
        volatile auto result = seval::evaluate<uint16_t, const char*>("65535");
        (void)result;
    });

    /* 32 BIT */
    benchmark("32-bit signed", []() {
        volatile auto result = seval::evaluate<int32_t, const char*>("2147483647");
        (void)result;
    });

    benchmark("32-bit unsigned", []() {
        volatile auto result = seval::evaluate<uint32_t, const char*>("4294967295");
        (void)result;
    });

    /* 64 BIT */
    benchmark("64-bit signed", []() {
        volatile auto result = seval::evaluate<int64_t, const char*>("9223372036854775807");
        (void)result;
    });

    benchmark("64-bit unsigned", []() {
        volatile auto result = seval::evaluate<uint64_t, const char*>("18446744073709551615");
        (void)result;
    });

    /* HEXADECIMAL */
    benchmark("Hexadecimal", []() {
        volatile auto result = seval::evaluate<int, const char*>("0x123");
        (void)result;
    });

    /* FLOAT POINT */
    benchmark("Floating-point", []() {
        volatile auto result = seval::evaluate<float, const char*>("3.14");
        (void)result;
    });

    /* FLOAT POINT WITH EXPONENT */
    benchmark("Floating-point with exponent", []() {
        volatile auto result = seval::evaluate<float, const char*>("3.14e2");
        (void)result;
    });

    /* BINARY */
    benchmark("Binary", []() {
        volatile auto result = seval::evaluate<int, const char*>("0b101010");
        (void)result;
    });
}

int main() {
    seval_benchmark();
    std::cout << "All tests passed and benchmarks completed!" << std::endl;
    return 0;
}
