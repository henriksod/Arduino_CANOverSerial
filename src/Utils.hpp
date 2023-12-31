/**********************************************************************************************
 * SerialCAN, CAN communication over Serial bus - Version 1.0.0
 * by Henrik Söderlund <henrik.a.soderlund@gmail.com>
 *
 * Copyright (c) 2023 Henrik Söderlund

 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 **********************************************************************************************/

#ifndef SERIALCAN_SRC_UTILS_HPP_
#define SERIALCAN_SRC_UTILS_HPP_

#include "Arduino.h"
#include "HardwareSerial.h"

// The following code is made by Hideaki Tai, with some slight modifications
// https://github.com/hideakitai/ArxTypeTraits
//
// Check whether __has_include is available, but also check the GCC
// version (__has_include was introduced in gcc 5) to catch
// environments (such as ESP8266) where gcc is old, but some system
// header provides a fake __has_include. We also need to check
// against __clang__ here, since clang pretends to be GCC
// 4.something and would otherwise be detected incorrectly here...
#if !defined(__has_include) || defined(__GNUC__) && __GNUC__ < 5 && !defined(__clang__)
    #if defined(ARDUINO_ARCH_ESP8266)
        // ESP8266 does not have a working __has_include, but we
        // know it does have a working libstdc++ with all the
        // headers we care about, so provide a fake has_include
        #define SERIALCAN_HAS_INCLUDE(x) 1
    #elif defined(ARDUINO_SAM_DUE)
        // Arduino DUE's GCC version is 4.8.3 (GCC < 5.0).
        // If libstdc++ is used, std::function causes error
        // so currently we disable libstdc++ and use our
        // own implementations.
        #define SERIALCAN_HAS_INCLUDE(x) 0
    #else
        #error "Compiler does not support __has_include."
    #endif
#else
    #define SERIALCAN_HAS_INCLUDE(x) __has_include(x)
#endif

// Initializer_list *must* be defined in std, so take extra care to only
// define it when <initializer_list> is really not available (e.g.
// ArduinoSTL is C++98 but *does* define <initializer_list>) and not
// already defined.
#if SERIALCAN_HAS_INCLUDE(<initializer_list>)
#include <initializer_list>
#else
namespace std {

template<class _Ep>
class initializer_list {
    const _Ep* __begin_;
    size_t    __size_;

    inline
    constexpr
    initializer_list(const _Ep* __b, size_t __s) noexcept
        : __begin_(__b),
          __size_(__s)
    {}

 public:
    typedef _Ep        value_type;
    typedef const _Ep& reference;
    typedef const _Ep& const_reference;
    typedef size_t    size_type;

    typedef const _Ep* iterator;
    typedef const _Ep* const_iterator;

    inline
    constexpr
    initializer_list() noexcept : __begin_(nullptr), __size_(0) {}

    inline
    constexpr
    size_t    size()  const noexcept {return __size_;}

    inline
    constexpr
    const _Ep* begin() const noexcept {return __begin_;}

    inline
    constexpr
    const _Ep* end()   const noexcept {return __begin_ + __size_;}
};

template<class _Ep>
inline
constexpr
const _Ep*
begin(initializer_list<_Ep> __il) noexcept {
    return __il.begin();
}

template<class _Ep>
inline
constexpr
const _Ep*
end(initializer_list<_Ep> __il) noexcept {
    return __il.end();
}

}  // namespace std
#endif

#endif  // SERIALCAN_SRC_UTILS_HPP_
