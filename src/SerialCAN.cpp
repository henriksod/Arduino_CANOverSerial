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

#include "SerialCAN.h"

using serial_can::SerialCAN;
using serial_can::Frame;

void SerialCAN::begin(uint32_t baud_rate) {
    _streamRef->begin(baud_rate);
    _has_begun = true;
}

void SerialCAN::send(Frame *outgoing_frame, uint32_t timestamp) {
    // Check if SerialCAN has not been initialized with begin().
    assert(_has_begun);

    // Start byte
    can_frame_buffer[0] = 0xAA;

    // Timestamp
    for (int i = 0; i < 4; i++) {
        can_frame_buffer[i+1] = timestamp >> (i * 8);
    }

    // DLC
    can_frame_buffer[5] = outgoing_frame->dlc;

    // Arbitration ID
    for (int i = 0; i < 4; i++) {
        can_frame_buffer[i+6] = outgoing_frame->arbitration_id >> (i * 8);
    }

    // Calculate CRC if use_crc is Frame::crc8
    if (outgoing_frame->use_crc == Frame::crc8) {
        // Set counter in next last byte in payload
        outgoing_frame->payload[outgoing_frame->dlc-2] = outgoing_frame->counter;

        // Calculate CRC and put in last byte in payload
        outgoing_frame->payload[outgoing_frame->dlc-1] = getCRC8(
            outgoing_frame->payload, outgoing_frame->dlc-1);
    }

    // Payload
    for (int i = 0; i < outgoing_frame->dlc; i++) {
        can_frame_buffer[10 + i] = outgoing_frame->payload[i];
    }

    // End byte
    can_frame_buffer[10+outgoing_frame->dlc] = 0xBB;

    // Send Frame
    for (int i = 0; i <= 10 + outgoing_frame->dlc; i++) {
        _streamRef->write(can_frame_buffer[i]);
    }

    outgoing_frame->counter++;
}


bool SerialCAN::receive(Frame *incoming_frame, uint32_t timeout_ms) {
    uint8_t data_byte;
    uint32_t time_since_byte;
    uint32_t time_delta;

    if (_streamRef->available()) {
        data_byte = _streamRef->read();

        // Check for frame start byte
        if (data_byte == 0xAA) {
            can_frame_buffer[0] = data_byte;
            // Incoming CAN Frame
            bool got_delimeter_byte = false;
            for (int i = 0; i < 18; i++) {
                time_since_byte = millis();
                while (!_streamRef->available()) {
                    time_delta = millis() - time_since_byte;
                    if (time_delta > timeout_ms) {
                        _fault_reason = timeout;
                        return false;
                    }
                }

                // Read data
                data_byte = _streamRef->read();
                can_frame_buffer[i+1] = data_byte;

                // Parse Header
                if (i < 4) {
                    incoming_frame->timestamp = incoming_frame->timestamp >> 8;
                    incoming_frame->timestamp = incoming_frame->timestamp | (data_byte << 24);
                }

                if (i == 4) {
                    incoming_frame->dlc = constrain(data_byte, 0, 8);
                }

                if (i > 4 && i < 9) {
                    incoming_frame->arbitration_id = incoming_frame->arbitration_id >> 8;
                    incoming_frame->arbitration_id =
                        incoming_frame->arbitration_id | (data_byte << 24);
                }

                // Parse payload
                if (i >= 9 && i < 9 + incoming_frame->dlc) {
                    incoming_frame->payload[i-9] = data_byte;
                }

                if (i >= 9 + incoming_frame->dlc) {
                    if (data_byte == 0xBB) {
                        got_delimeter_byte = true;
                        break;
                    }
                }
            }

            if (got_delimeter_byte == true) {
                // Check crc match if use_crc is Frame::crc8
                if (incoming_frame->use_crc == Frame::crc8) {
                    // Store counter
                    incoming_frame->counter = incoming_frame->payload[incoming_frame->dlc-2];

                    // Calculate CRC, excluding CRC byte in payload
                    uint8_t crc_value = getCRC8(incoming_frame->payload, incoming_frame->dlc-1);
                    incoming_frame->crc = crc_value;

                    // Check if CRC is a match between calculated and payload CRC
                    if (crc_value != incoming_frame->payload[incoming_frame->dlc-1]) {
                        _fault_reason = crc_mismatch;
                        return false;
                    }
                }

                _fault_reason = none;
                return true;
            } else {
                _fault_reason = missing_end_delimeter;
                return false;
            }
        }
    }

    _fault_reason = no_incoming_data;
    return false;
}

uint8_t SerialCAN::getCRC8(uint8_t const message[], int nBytes) {
    uint8_t data;
    uint8_t remainder = 0x00;

    for (int byte = 0; byte < nBytes; byte++) {
        remainder = crcTable[remainder^message[byte]];
    }

    return (remainder);
}
