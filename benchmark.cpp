#include <iostream>
#include <cassert>
#include <ctime>
#include <stdint.h>

#if defined(_WIN32)
#include <Windows.h>
#define __clock GetTickCount
#else
#define __clock clock
#endif // _WIN32


#if __cplusplus >= 201103L
#include <chrono>
#endif

#include "seval.hpp"

#define BENCHMARK_ITERATIONS 1000000

struct Benchmark8BitSigned {
    SEVAL_INLINE void operator()() const {
        volatile int8_t result = seval::evaluate<int8_t, const char*>("127");
        (void)result;
    }
};

struct Benchmark8BitUnsigned {
    SEVAL_INLINE void operator()() const {
        volatile uint8_t result = seval::evaluate<uint8_t, const char*>("255");
        (void)result;
    }
};

struct Benchmark16BitSigned {
    SEVAL_INLINE void operator()() const {
        volatile int16_t result = seval::evaluate<int16_t, const char*>("32767");
        (void)result;
    }
};

struct Benchmark16BitUnsigned {
    SEVAL_INLINE void operator()() const {
        volatile uint16_t result = seval::evaluate<uint16_t, const char*>("65535");
        (void)result;
    }
};

struct Benchmark32BitSigned {
    SEVAL_INLINE void operator()() const {
        volatile int32_t result = seval::evaluate<int32_t, const char*>("2147483647");
        (void)result;
    }
};

struct Benchmark32BitUnsigned {
    SEVAL_INLINE void operator()() const {
        volatile uint32_t result = seval::evaluate<uint32_t, const char*>("4294967295");
        (void)result;
    }
};

struct Benchmark64BitSigned {
    SEVAL_INLINE void operator()() const {
        volatile int64_t result = seval::evaluate<int64_t, const char*>("9223372036854775807");
        (void)result;
    }
};

struct Benchmark64BitUnsigned {
    SEVAL_INLINE void operator()() const {
        volatile uint64_t result = seval::evaluate<uint64_t, const char*>("18446744073709551615");
        (void)result;
    }
};

struct BenchmarkHexadecimal {
    SEVAL_INLINE void operator()() const {
        volatile int result = seval::evaluate<int, const char*>("0x123");
        (void)result;
    }
};

struct BenchmarkFloatingPoint {
    SEVAL_INLINE void operator()() const {
        volatile float result = seval::evaluate<float, const char*>("3.14");
        (void)result;
    }
};

struct BenchmarkFloatingPointExponent {
    SEVAL_INLINE void operator()() const {
        volatile float result = seval::evaluate<float, const char*>("3.14e2");
        (void)result;
    }
};

struct BenchmarkBinary {
    SEVAL_INLINE void operator()() const {
        volatile int result = seval::evaluate<int, const char*>("0b101010");
        (void)result;
    }
};

template <typename Func>
void benchmark(const std::string& name, Func func) {
#if __cplusplus < 201103L
    clock_t start = __clock();

    for (int i = 0; i < BENCHMARK_ITERATIONS; ++i) {
        func();
    }

    clock_t end = __clock();
    double duration = double(end - start) / CLOCKS_PER_SEC;
    std::cout << name << ": " << (duration * 1000000000.0 / BENCHMARK_ITERATIONS) << " ns per call" << std::endl;
#elif __cplusplus >= 201103L && __cplusplus < 201703L
    std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < BENCHMARK_ITERATIONS; ++i) {
        func();
    }

    std::chrono::time_point<std::chrono::high_resolution_clock> end = std::chrono::high_resolution_clock::now();
    std::chrono::_V2::system_clock::rep duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    std::cout << name << ": " << (duration / BENCHMARK_ITERATIONS) << " ns per call" << std::endl;
#elif __cplusplus >= 201703L
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < BENCHMARK_ITERATIONS; ++i) {
        func();
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::_V2::system_clock::rep duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    std::cout << name << ": " << (duration / BENCHMARK_ITERATIONS) << " ns per call" << std::endl;
#endif
}

void seval_benchmark() {
    benchmark("8-bit signed", Benchmark8BitSigned());
    benchmark("8-bit unsigned", Benchmark8BitUnsigned());
    benchmark("16-bit signed", Benchmark16BitSigned());
    benchmark("16-bit unsigned", Benchmark16BitUnsigned());
    benchmark("32-bit signed", Benchmark32BitSigned());
    benchmark("32-bit unsigned", Benchmark32BitUnsigned());
    benchmark("64-bit signed", Benchmark64BitSigned());
    benchmark("64-bit unsigned", Benchmark64BitUnsigned());
    benchmark("Hexadecimal", BenchmarkHexadecimal());
    benchmark("Floating-point", BenchmarkFloatingPoint());
    benchmark("Floating-point with exponent", BenchmarkFloatingPointExponent());
    benchmark("Binary", BenchmarkBinary());
}

int main() {
    seval_benchmark();
    std::cout << "All benchmarks completed!" << std::endl;
    return 0;
}
