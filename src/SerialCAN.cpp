#include "SerialCAN.h"

using namespace serial_can;

void SerialCAN::begin(uint32_t baud_rate) {
    _streamRef.begin(baud_rate);
    _has_begun = true;
}

void SerialCAN::send(Frame &outgoing_frame, const uint32_t timestamp) {
    m_assert(assert_msg_not_initialized_, _has_begun);

    // Start byte
    can_frame_buffer[0] = 0xAA;
    
    // Timestamp
    for (int i = 0; i < 4; i++) {
        can_frame_buffer[i+1] = timestamp >> (i * 8);
    }
    
    // DLC
    can_frame_buffer[5] = outgoing_frame.dlc;
    
    // Arbitration ID
    for (int i = 0; i < 4; i++) {
        can_frame_buffer[i+6] = outgoing_frame.arbitration_id >> (i * 8);
    }
    
    // Calculate CRC if use_crc is true
    if (outgoing_frame.use_crc == true) {
        // Set counter in next last byte in payload
        outgoing_frame.payload[outgoing_frame.dlc-2] = outgoing_frame.counter;
        
        // Calculate CRC and put in last byte in payload
        outgoing_frame.payload[outgoing_frame.dlc-1] = getCRC8(
            outgoing_frame.payload, outgoing_frame.dlc-1
        );
    }
    
    // Payload
    for (int i = 0; i < outgoing_frame.dlc; i++) {
        can_frame_buffer[10 + i] = outgoing_frame.payload[i];
    }
    
    // End byte
    can_frame_buffer[10+outgoing_frame.dlc] = 0xBB;
    
    // Wait for any remaining packets to be received
    //_streamRef.flush();
    
    // Send Frame
    for (int i = 0; i <= 10 + outgoing_frame.dlc; i++) {
        _streamRef.write(can_frame_buffer[i]);
    }
    
    outgoing_frame.counter++;
}


bool SerialCAN::receive(Frame &incoming_frame, unsigned long timeout_ms) {
    uint8_t data_byte;
    unsigned long time_since_byte;
    unsigned long time_delta;
    
    if (_streamRef.available()) {
        data_byte = _streamRef.read();
        
        // Check for frame start byte
        if (data_byte == 0xAA) {
            can_frame_buffer[0] = data_byte;
            // Incoming CAN Frame
            bool got_delimeter_byte = false;
            for (int i = 0; i < 18; i++) {
                time_since_byte = millis();
                while(!_streamRef.available()) {
                    time_delta = millis() - time_since_byte;
                    if (time_delta > timeout_ms) {
                        _fault_reason = timeout;
                        return false;
                    }
                }
                
                // Read data
                data_byte = _streamRef.read();
                can_frame_buffer[i+1] = data_byte;
                
                // Parse Header
                if (i < 4) {
                    incoming_frame.timestamp = incoming_frame.timestamp >> 8;
                    incoming_frame.timestamp = incoming_frame.timestamp | (data_byte << 24);
                }
                
                if (i == 4) {
                    incoming_frame.dlc = constrain(data_byte, 0, 8);
                }
                
                if (i > 4 && i < 9) {
                    incoming_frame.arbitration_id = incoming_frame.arbitration_id >> 8;
                    incoming_frame.arbitration_id = incoming_frame.arbitration_id | (data_byte << 24);
                }
                
                // Parse payload
                if (i >= 9 && i < 9 + incoming_frame.dlc) {
                    incoming_frame.payload[i-9] = data_byte;
                }
                
                if (i >= 9 + incoming_frame.dlc) {
                    if (data_byte == 0xBB) {
                        got_delimeter_byte = true;
                        break;
                    }
                }
            }
            
            if (got_delimeter_byte == true) {
                // Check crc match if use_crc is true
                if (incoming_frame.use_crc == true) {
                    // Store counter
                    incoming_frame.counter = incoming_frame.payload[incoming_frame.dlc-2];
                    
                    // Calculate CRC, excluding CRC byte in payload
                    uint8_t crc_value = getCRC8(incoming_frame.payload, incoming_frame.dlc-1);
                    
                    // Check if CRC is a match between calculated and payload CRC
                    if (crc_value != incoming_frame.payload[incoming_frame.dlc-1]) {
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

    for (int byte = 0; byte < nBytes; byte++)
    {
        remainder = crcTable[remainder^message[byte]];
    }

    return (remainder);
}
