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

#ifndef SERIALCAN_SRC_FRAME_HPP_
#define SERIALCAN_SRC_FRAME_HPP_

#define __ASSERT_USE_STDERR

#include <assert.h>
#include <string.h>
#include "Arduino.h"
#include "Utils.hpp"

namespace serial_can {

/**
 * Represents a CAN frame for communication over a Serial bus.
 */
class Frame {
 public:
    /**
     * CRC checksum settings.
     */
    enum crc_settings {
        no_crc,                   /**< Do not use CRC */
        crc8                /**< Use CRC8 as checksum */
    };

    /**
     * CAN frame arbitration/message ID.
     */
    uint32_t arbitration_id;

    /**
     * CAN frame DLC (payload size in bytes, maximum 8).
     */
    uint8_t dlc;

    /**
     * Indicates whether to activate CRC calculations for end-to-end protection.
     * Warning: Using CRC will limit the effective payload size to max 6 bytes.
     */
    crc_settings use_crc;

    /**
     * Timestamp of the CAN frame (only used for incoming frames).
     */
    uint32_t timestamp = {};

    /**
     * Stored CAN frame payload.
     */
    uint8_t payload[8] = {};

    /**
     * Counter used for CRC calculations.
     */
    uint8_t counter = {};

    /**
     * Counter used for CRC calculations.
     */
    uint8_t crc = {};

    /**
     * Constructs a new Frame object.
     */
    Frame() : arbitration_id{0x00}, dlc{8}, use_crc{crc_settings::no_crc} {}

    /**
     * Constructs a new Frame object.
     *
     * @param _use_crc Enum indicating whether to use CRC calculations.
     */
    Frame(crc_settings _use_crc) : arbitration_id{0x00}, dlc{8}, use_crc{_use_crc} explicit {}

     /**
     * Constructs a new Frame object.
     *
     * @param _arbitration_id The CAN frame arbitration ID.
     * @param _dlc The CAN frame DLC.
     * @pre The maximum allowed DLC is 8.
     */
    Frame(uint32_t _arbitration_id, uint8_t _dlc) :
      arbitration_id{_arbitration_id}, dlc{_dlc}, use_crc{crc_settings::no_crc} {
        // Maximum allowed DLC is 8.
        assert(dlc <= 8);
    }

    /**
     * Constructs a new Frame object.
     *
     * @param _arbitration_id The CAN frame arbitration ID.
     * @param _dlc The CAN frame DLC.
     * @param _use_crc Enum indicating whether to use CRC calculations.
     * @pre The maximum allowed DLC is 8.
     */
    Frame(uint32_t _arbitration_id, uint8_t _dlc, crc_settings _use_crc) :
      arbitration_id{_arbitration_id}, dlc{_dlc}, use_crc{_use_crc} {
        // Maximum allowed DLC is 8.
        assert(dlc <= 8);
    }

    /**
     * Encodes and packs the given data into the payload using the specified initializer list.
     * The size of each data element must not exceed 6 bytes if CRC is enabled, or 8 bytes otherwise.
     *
     * @param data_list The initializer list of data elements to encode and pack.
     * @tparam T The type of data elements.
     * @pre The maximum allowed number of bytes is 6 when use_crc is crc8.
     * @pre The maximum allowed number of bytes is 8 when use_crc is no_crc.
     */
    template<typename T>
    void encode(std::initializer_list<T> data_list) {
        const size_t T_size = sizeof(T);

        // Maximum allowed number of bytes is 6 if use_crc is crc8
        assert((use_crc && data_list.size() * T_size <= 6) || !use_crc);
        // Maximum allowed number of bytes is 8 if use_crc is no_crc
        assert(data_list.size() * T_size <= 8);

        size_t current_start_byte = 0;
        for (auto elem : data_list) {
            packData_<T>(elem, current_start_byte);
            current_start_byte += T_size;
        }
    }

    /**
     * Encodes and packs the given string into the payload.
     * The length of the string must not exceed 6 bytes if CRC is enabled, or 8 bytes otherwise.
     *
     * @param string The string to encode and pack.
     * @pre The maximum allowed number of bytes is 6 when use_crc is crc8.
     * @pre The maximum allowed number of bytes is 8 when use_crc is no_crc.
     */
    void encode(const char* string) {
        size_t string_len = strlen(string);

        // Maximum allowed number of bytes is 6 if use_crc is crc8
        assert((use_crc && string_len <= 6) || !use_crc);
        // Maximum allowed number of bytes is 8 if use_crc is no_crc
        assert(string_len <= 8);

        size_t current_start_byte = 0;
        for (size_t i = 0; i < string_len; i++)
            packData_<char>(string[i], current_start_byte++);
    }

 private:
    /**
     * Packs the given data into the payload starting from the specified byte index.
     *
     * @param data The data to pack.
     * @param start_byte The byte index to start packing from.
     * @tparam T The type of the data.
     */
    template<typename T>
    void packData_(T data, int start_byte) {
        for (int i = 0; i < sizeof(data); i++) {
            payload[i+start_byte] = data >> (i * 8);
        }
    }
};

}  // namespace serial_can

#endif  // SERIALCAN_SRC_FRAME_HPP_
