
#include "SerialCAN.h"

using namespace serial_can; 

// Serial CAN communication
SerialCAN serialCAN{&Serial};

// An example CAN frame {arbitration_id, dlc, use_crc}
Frame example_frame{0xFF, 6, Frame::crc8};

uint32_t timestamp = 0;

void setup() {
    // If using E2E protections by setting use_crc=true,
    // we can use very high baud rates over serial
    serialCAN.begin(460800);
}

void loop() {
    timestamp += 1;

    // Encode a string into the frame payload
    example_frame.encode("test");

    // Dispatch the message with a given timestamp
    serialCAN.send(&example_frame, timestamp);

    // Encode data into the frame payload
    example_frame.encode<uint8_t>({
        static_cast<uint8_t>(0x01),
        static_cast<uint8_t>(0x02)});

    // Dispatch the message with a given timestamp
    serialCAN.send(&example_frame, timestamp);

    delay(100);
}
