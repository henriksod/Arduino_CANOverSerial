/*  
    CAN communication over Serial bus
    
    Compatible with python-can SerialBus

    Author: Henrik Söderlund, 2022
*/

#ifndef SERIAL_CAN_FRAME_H
#define SERIAL_CAN_FRAME_H

#include <assert.h>
#include "Arduino.h"
#include "ArxTypeTraits.h"

#define __ASSERT_USE_STDERR
#define m_assert(expr, msg) assert(( (void)(msg), (expr) ))

namespace serial_can
{

using namespace arx;

class Frame
{
  public:
    // CAN frame arbitration/message ID
    uint32_t arbitration_id;
    // CAN frame DLC (payload size in bytes, maximum 8)
    uint8_t dlc;
    /// Activate crc calculations for end-to-end protection.
    /// Warning: Using CRC will limit effective payload size to 6 bytes.
    bool use_crc;
    // Timestamp of CAN frame, only used for incoming frames
    uint32_t timestamp = {};

    // Stored CAN Frame payload
    uint8_t payload[8] = {};
    // Counter used for CRC calculations
    uint8_t counter = {};

    Frame(
      uint32_t _arbitration_id, uint8_t _dlc, bool _use_crc
    ) : arbitration_id{_arbitration_id}, dlc{_dlc}, use_crc{_use_crc} {
        m_assert("Maximum allowed DLC is 8.", dlc <= 8);
    }

    template<typename T>
    void encode(std::initializer_list<T> data_list);

    void encode(const char* string);

  private:
    template<typename T>
    void packData_(T data, int start_byte);
};

}

#endif