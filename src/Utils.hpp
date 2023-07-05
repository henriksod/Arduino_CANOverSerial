/**********************************************************************************************
 * SerialCAN, CAN communication over Serial bus - Version 0.1.0
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

namespace serial_can {
namespace utils {

template<class T>
class initializer_list {
 private:
    const T* array;
    size_t len;
    initializer_list(const T* a, size_t l) : array(a), len(l) {}
 public:
    initializer_list() : array(nullptr), len(0) {}
    size_t size() const { return len; }
    const T *begin() const { return array; }
    const T *end() const { return array + len; }
};

/**
 * Used for testing.
 */
class DummySerial : public HardwareSerial
{
  public:
    static const size_t buffer_size = 19;

    size_t out_buffer_idx = 0;
    uint8_t dummy_buffer[buffer_size] = {};

    size_t in_buffer_idx = 0;
    uint8_t read_buffer[buffer_size] = {
      0xAA, 0x00, 0x00, 0x00, 0x01, 0x08, 0xFF, 0xFF, 0xFF, 0xFF,
      0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xBB
    };

    void begin(unsigned long baud) { }
    int available(void) override { return in_buffer_idx < buffer_size; }
    int peek(void) override { return 0; }
    int availableForWrite(void) override { return 0; }
    void flush(void) override { return; }
    int read(void) override {
      if (available())
        return read_buffer[in_buffer_idx++];
      return 0;
    }
    size_t write(uint8_t val) override {
      dummy_buffer[out_buffer_idx++] = val;
      out_buffer_idx = out_buffer_idx >= buffer_size ? 0 : out_buffer_idx; 
      return 0;
    }
};

}  // namespace utils
}  // namespace serial_can

#endif  // SERIALCAN_SRC_UTILS_HPP_
