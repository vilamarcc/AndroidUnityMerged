#pragma once

#include <type_traits>
#include <limits>
#include "Internal/TypeTraits.h"

namespace baselib
{
    BASELIB_CPP_INTERFACE
    {
        namespace Algorithm
        {
            // Index of the most significant bit in a 32bit mask. Returns -1 if no bits are set.
            inline int HighestBit(uint32_t value);
            // Index of the most significant bit in a 64bit mask. Returns -1 if no bits are set.
            inline int HighestBit(uint64_t value);

            // Index of the most significant bit in a 32bit mask. Unspecified result if no bits are set.
            inline int HighestBitNonZero(uint32_t value);
            // Index of the most significant bit in a 64bit mask. Unspecified result if no bits are set.
            inline int HighestBitNonZero(uint64_t value);

            // Index of the least significant bit in a 64bit mask. Returns -1 if no bits are set.
            inline int LowestBit(uint32_t value);
            // Index of the least significant bit in a 64bit mask. Returns -1 if no bits are set.
            inline int LowestBit(uint64_t value);

            // Index of the least significant bit in a 64bit mask. Unspecified result if no bits are set.
            inline int LowestBitNonZero(uint32_t value);
            // Index of the least significant bit in a 64bit mask. Unspecified result if no bits are set.
            inline int LowestBitNonZero(uint64_t value);

            // Returns number of set bits in a 64 bit mask.
            inline int BitsInMask(uint64_t mask);
            // Returns number of set bits in a 32 bit mask.
            inline int BitsInMask(uint32_t mask);
            // Returns number of set bits in a 16 bit mask.
            inline int BitsInMask(uint16_t mask);
            // Returns number os set bits in a 8 bit mask.
            inline int BitsInMask(uint8_t mask);

            // Number of set bits (population count) in an array of known size.
            // Using Robert Harley and David Seal's algorithm from Hacker's Delight,
            // variant that does 4 words in a loop iteration.
            // http://www.hackersdelight.org/revisions.pdf
            // http://www.hackersdelight.org/HDcode/newCode/pop_arrayHS.cc
            template<typename WordT, int WordCount>
            inline int BitsInArray(const WordT* data)
            {
    #define HarleySealCSAStep(h, l, a, b, c) {\
        WordT u = a ^ b; \
        h = (a & b) | (u & c); l = u ^ c; \
    }
                WordT ones, twos, twosA, twosB, fours;

                int i = 0;
                int tot = 0;
                twos = ones = 0;
                for (; i <= WordCount - 4; i = i + 4)
                {
                    HarleySealCSAStep(twosA, ones, ones, data[i], data[i + 1])
                    HarleySealCSAStep(twosB, ones, ones, data[i + 2], data[i + 3])
                    HarleySealCSAStep(fours, twos, twos, twosA, twosB)
                    tot = tot + BitsInMask(fours);
                }
                tot = 4 * tot + 2 * BitsInMask(twos) + BitsInMask(ones);

                for (; i < WordCount; i++) // Simply add in the last
                    tot = tot + BitsInMask(data[i]); // 0 to 3 elements.

                return tot;
    #undef HarleySealCSAStep
            }

            // Checks if one integers is a multiple of another.
            template<typename T>
            constexpr inline bool AreIntegersMultiple(T a, T b)
            {
                static_assert(std::is_integral<T>::value, "AreIntegersMultiple requires integral types.");
                return a != 0 && b != 0 && // if at least one integer is 0, consider false (avoid div by 0 of the following modulo)
                    ((a % b) == 0 || (b % a) == 0);
            }

            // Checks if value is a power-of-two.
            template<typename T>
            constexpr inline bool IsPowerOfTwo(T value)
            {
                static_assert(std::is_integral<T>::value, "IsPowerOfTwo works only with an integral type.");
                using T_unsigned = typename std::make_unsigned<T>::type;
                return (static_cast<T_unsigned>(value) & (static_cast<T_unsigned>(value) - 1)) == 0;
            }

            // Returns the next power-of-two of a 32bit number or the current value if it is a power two.
            inline uint32_t CeilPowerOfTwo(uint32_t value)
            {
                value -= 1;
                value |= value >> 16;
                value |= value >> 8;
                value |= value >> 4;
                value |= value >> 2;
                value |= value >> 1;
                return value + 1;
            }

            // Returns the next power-of-two of a 64bit number or the current value if it is a power two.
            inline uint64_t CeilPowerOfTwo(uint64_t value)
            {
                value -= 1;
                value |= value >> 32;
                value |= value >> 16;
                value |= value >> 8;
                value |= value >> 4;
                value |= value >> 2;
                value |= value >> 1;
                return value + 1;
            }

            // Returns the closest power-of-two of a 32bit number.
            template<typename T>
            inline T RoundPowerOfTwo(T value)
            {
                static_assert(std::is_unsigned<T>::value, "RoundPowerOfTwo works only with an unsigned integral type.");
                const T nextPower = CeilPowerOfTwo(value);
                const T prevPower = nextPower >> 1;
                if (value - prevPower < nextPower - value)
                    return prevPower;
                else
                    return nextPower;
            }

            // Returns true if addition of two given operands leads to an integer overflow.
            template<typename T>
            constexpr inline bool DoesAdditionOverflow(T a, T b)
            {
                static_assert(std::is_unsigned<T>::value, "Overflow checks apply only work on unsigned integral types.");
                return std::numeric_limits<T>::max() - a < b;
            }

            // Returns true if multiplication of two given operands leads to an integer overflow.
            template<typename T>
            constexpr inline bool DoesMultiplicationOverflow(T a, T b)
            {
                static_assert(std::is_unsigned<T>::value, "Overflow checks apply only work on unsigned integral types.");
                return b != 0 && std::numeric_limits<T>::max() / b < a;
            }

            // Clamp
            //
            // This function can be used with different types - `value` vs. `lo`, `hi`.
            // If `lo` if larger than `hi` this function has undefined bahavior.
            //
            // Return: clamped `value` of the same type as `lo`, `hi`.
            //
            COMPILER_WARNINGS_PUSH
    #ifdef _MSC_VER
            COMPILER_WARNINGS_DISABLE(4756)
    #endif
            template<typename RT, typename VT, typename std::enable_if<
                baselib::is_of_same_signedness<RT, VT>::value
                || !std::is_integral<RT>::value
                || !std::is_integral<VT>::value
                , bool>::type = 0>
            inline RT Clamp(VT value, RT lo, RT hi)
            {
                if (value < lo) return lo;
                if (value > hi) return hi;
                return static_cast<RT>(value);
            }

            COMPILER_WARNINGS_POP

            template<typename RT, typename VT, typename std::enable_if<
                std::is_integral<RT>::value && std::is_unsigned<RT>::value &&
                std::is_integral<VT>::value && std::is_signed<VT>::value
                , bool>::type = 0>
            inline RT Clamp(VT value, RT lo, RT hi)
            {
                if (value < 0)
                    return lo;
                using UnsignedVT = typename std::make_unsigned<VT>::type;
                return Clamp(static_cast<UnsignedVT>(value), lo, hi);
            }

            template<typename RT, typename VT, typename std::enable_if<
                std::is_integral<RT>::value && std::is_signed<RT>::value &&
                std::is_integral<VT>::value && std::is_unsigned<VT>::value
                , bool>::type = 0>
            inline RT Clamp(VT value, RT lo, RT hi)
            {
                if (hi < 0)
                    return hi;
                if (lo < 0)
                    lo = 0;
                using UnsignedRT = typename std::make_unsigned<RT>::type;
                return static_cast<RT>(Clamp(value, static_cast<UnsignedRT>(lo), static_cast<UnsignedRT>(hi)));
            }

            // Clamp `value` by lowest and highest value of RT.
            //
            // Return: clamped `value` of the type RT.
            //
            template<typename RT, typename VT, typename std::enable_if<
                !(std::numeric_limits<RT>::has_infinity && std::numeric_limits<VT>::has_infinity)
                , bool>::type = 0>
            inline RT ClampToType(VT value)
            {
                return Clamp(value, std::numeric_limits<RT>::lowest(), std::numeric_limits<RT>::max());
            }

            // Clamp `value` by lowest and highest value of RT.
            //
            // This function is guaranteed to only return infinity values if the source value was already an infinity number.
            //
            // Return: clamped `value` of the type RT.
            //
            template<typename RT, typename VT, typename std::enable_if<
                (std::numeric_limits<RT>::has_infinity && std::numeric_limits<VT>::has_infinity)
                , bool>::type = 0>
            inline RT ClampToType(VT value)
            {
                if (value == std::numeric_limits<VT>::infinity() || value == -std::numeric_limits<VT>::infinity())
                    return static_cast<RT>(value);
                return Clamp(value, std::numeric_limits<RT>::lowest(), std::numeric_limits<RT>::max());
            }
        }
    }
}

#if _MSC_VER
    #include "Internal/Compiler/Msvc/AlgorithmMsvc.inl.h"
#elif __clang__ || __GNUC__ || __GCC__
    #include "Internal/Compiler/ClangOrGcc/AlgorithmClangOrGcc.inl.h"
#else
    #error "Unknown Compiler"
#endif
