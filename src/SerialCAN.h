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

#ifndef SERIALCAN_SRC_SERIALCAN_H_
#define SERIALCAN_SRC_SERIALCAN_H_

#define __ASSERT_USE_STDERR

#include <assert.h>
#include "Utils.hpp"
#include "Frame.hpp"

namespace serial_can {

/**
 * SerialCAN class for CAN communication over Serial bus.
 */
class SerialCAN {
 public:
    /**
     * Reason for a fault in the SerialCAN class.
     */
    enum fault_reason {
        none,                   /**< No fault. */
        timeout,                /**< Timeout occurred. */
        no_incoming_data,       /**< No incoming data. */
        crc_mismatch,           /**< CRC mismatch. */
        missing_end_delimeter    /**< Missing end delimiter. */
    };

    /**
     * Constructor for SerialCAN class.
     * @param streamObject The HardwareSerial object for serial communication.
     */
    explicit SerialCAN(HardwareSerial *streamObject) :
        _streamRef{streamObject}, _fault_reason{none} {}


    /**
     * Initializes the SerialCAN communication.
     * @param baud_rate The baud rate for serial communication.
     */
    void begin(uint32_t baud_rate);

    /**
     * Sends a CAN frame over the SerialCAN bus.
     * @param outgoing_frame The outgoing CAN frame to be sent.
     * @param timestamp The timestamp of the CAN frame.
     */
    void send(Frame *outgoing_frame, uint32_t timestamp);

    /**
     * Receives a CAN frame from the SerialCAN bus.
     * @param incoming_frame The incoming CAN frame to be received.
     * @param timeout_ms The timeout in milliseconds for receiving the frame.
     * @return True if a frame was received successfully, false otherwise.
     */
    bool receive(Frame *incoming_frame, uint32_t timeout_ms);

    /**
     * Get the reason for the fault in the SerialCAN class.
     * @return The fault reason.
     */
    fault_reason getFaultReason(void) { return _fault_reason; }

 private:
    uint8_t can_frame_buffer[19] = {};  /**< Buffer for the CAN frame. */
    fault_reason _fault_reason = none; /**< Reason for a fault in the SerialCAN class. */
    HardwareSerial* _streamRef;        /**< Pointer to the HardwareSerial object. */
    bool _has_begun = false;           /**< Flag indicating if SerialCAN has been initialized. */

    /**
     * Calculates the CRC8 value for a given message.
     * @param message The message for which to calculate the CRC8 value.
     * @param nBytes The number of bytes in the message.
     * @return The CRC8 value.
     */
    uint8_t getCRC8(uint8_t const message[], int nBytes);
};

constexpr uint8_t crcTable[256] = {
    // CRC8 lookup table
    0x00, 0x07, 0x0e, 0x09, 0x1c, 0x1b, 0x12, 0x15,
    0x38, 0x3f, 0x36, 0x31, 0x24, 0x23, 0x2a, 0x2d,
    0x70, 0x77, 0x7e, 0x79, 0x6c, 0x6b, 0x62, 0x65,
    0x48, 0x4f, 0x46, 0x41, 0x54, 0x53, 0x5a, 0x5d,
    0xe0, 0xe7, 0xee, 0xe9, 0xfc, 0xfb, 0xf2, 0xf5,
    0xd8, 0xdf, 0xd6, 0xd1, 0xc4, 0xc3, 0xca, 0xcd,
    0x90, 0x97, 0x9e, 0x99, 0x8c, 0x8b, 0x82, 0x85,
    0xa8, 0xaf, 0xa6, 0xa1, 0xb4, 0xb3, 0xba, 0xbd,
    0xc7, 0xc0, 0xc9, 0xce, 0xdb, 0xdc, 0xd5, 0xd2,
    0xff, 0xf8, 0xf1, 0xf6, 0xe3, 0xe4, 0xed, 0xea,
    0xb7, 0xb0, 0xb9, 0xbe, 0xab, 0xac, 0xa5, 0xa2,
    0x8f, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9d, 0x9a,
    0x27, 0x20, 0x29, 0x2e, 0x3b, 0x3c, 0x35, 0x32,
    0x1f, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0d, 0x0a,
    0x57, 0x50, 0x59, 0x5e, 0x4b, 0x4c, 0x45, 0x42,
    0x6f, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7d, 0x7a,
    0x89, 0x8e, 0x87, 0x80, 0x95, 0x92, 0x9b, 0x9c,
    0xb1, 0xb6, 0xbf, 0xb8, 0xad, 0xaa, 0xa3, 0xa4,
    0xf9, 0xfe, 0xf7, 0xf0, 0xe5, 0xe2, 0xeb, 0xec,
    0xc1, 0xc6, 0xcf, 0xc8, 0xdd, 0xda, 0xd3, 0xd4,
    0x69, 0x6e, 0x67, 0x60, 0x75, 0x72, 0x7b, 0x7c,
    0x51, 0x56, 0x5f, 0x58, 0x4d, 0x4a, 0x43, 0x44,
    0x19, 0x1e, 0x17, 0x10, 0x05, 0x02, 0x0b, 0x0c,
    0x21, 0x26, 0x2f, 0x28, 0x3d, 0x3a, 0x33, 0x34,
    0x4e, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5c, 0x5b,
    0x76, 0x71, 0x78, 0x7f, 0x6a, 0x6d, 0x64, 0x63,
    0x3e, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2c, 0x2b,
    0x06, 0x01, 0x08, 0x0f, 0x1a, 0x1d, 0x14, 0x13,
    0xae, 0xa9, 0xa0, 0xa7, 0xb2, 0xb5, 0xbc, 0xbb,
    0x96, 0x91, 0x98, 0x9f, 0x8a, 0x8d, 0x84, 0x83,
    0xde, 0xd9, 0xd0, 0xd7, 0xc2, 0xc5, 0xcc, 0xcb,
    0xe6, 0xe1, 0xe8, 0xef, 0xfa, 0xfd, 0xf4, 0xf3
};

}  // namespace serial_can

#endif  // SERIALCAN_SRC_SERIALCAN_H_
