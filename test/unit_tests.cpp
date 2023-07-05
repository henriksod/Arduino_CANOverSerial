//
//    FILE: unit_tests.cpp
//  AUTHOR: Henrik Söderlund
//    DATE: 2023-07-02
// PURPOSE: unit tests for the solve() methods of SerialCAN
//          https://github.com/henriksod/Arduino_CANOverSerial
//          https://github.com/Arduino-CI/arduino_ci/blob/master/REFERENCE.md
//

// supported assertions
// ----------------------------
// assertEqual(expected, actual);               // a == b
// assertNotEqual(unwanted, actual);            // a != b
// assertComparativeEquivalent(expected, actual);    // abs(a - b) == 0 or (!(a > b) && !(a < b))
// assertComparativeNotEquivalent(unwanted, actual); // abs(a - b) > 0  or ((a > b) || (a < b))
// assertLess(upperBound, actual);              // a < b
// assertMore(lowerBound, actual);              // a > b
// assertLessOrEqual(upperBound, actual);       // a <= b
// assertMoreOrEqual(lowerBound, actual);       // a >= b
// assertTrue(actual);
// assertFalse(actual);
// assertNull(actual);

// // special cases for floats
// assertEqualFloat(expected, actual, epsilon);    // fabs(a - b) <= epsilon
// assertNotEqualFloat(unwanted, actual, epsilon); // fabs(a - b) >= epsilon
// assertInfinity(actual);                         // isinf(a)
// assertNotInfinity(actual);                      // !isinf(a)
// assertNAN(arg);                                 // isnan(a)
// assertNotNAN(arg);                              // !isnan(a)

#include "Arduino.h"
#include "SerialCAN.h"

using serial_can::SerialCAN;
using serial_can::Frame; 
using serial_can::utils::DummySerial;

unittest_setup()
{
}


unittest_teardown()
{
}


unittest(test_frame_string)
{
  // An example CAN frame {arbitration_id, dlc, use_crc}
  Frame example_frame{0xFF, 6, true};

  // Encode the variables into the frame payload
  example_frame.encode("test");

  assertEqual('t', example_frame.payload[0]);
  assertEqual('e', example_frame.payload[1]);
  assertEqual('s', example_frame.payload[2]);
  assertEqual('t', example_frame.payload[3]);
}


unittest(test_frame_value)
{
  // An example CAN frame {arbitration_id, dlc, use_crc}
  Frame example_frame{0xFF, 6, true};

  // Encode the variables into the frame payload
  example_frame.encode<uint16_t>({0x1515, 0xE2E2});

  assertEqual(0x15, example_frame.payload[0]);
  assertEqual(0x15, example_frame.payload[1]);
  assertEqual(0xE2, example_frame.payload[2]);
  assertEqual(0xE2, example_frame.payload[3]);
}


unittest(test_serial_can_send)
{
  DummySerial dummySerial;

  // Serial CAN communication
  SerialCAN serialCAN{&dummySerial};  
  // An example CAN frame {arbitration_id, dlc, use_crc}
  Frame example_frame{0xFF, 6, false};

  // Encode the text into the frame payload
  example_frame.encode("test");

  // Dispatch the message with a given timestamp
  serialCAN.begin();
  serialCAN.send(&example_frame, 1);

  assertEqual(0xAA, dummySerial.dummy_buffer[0]);
  assertEqual(0x00, dummySerial.dummy_buffer[1]);
  assertEqual(0x00, dummySerial.dummy_buffer[2]);
  assertEqual(0x00, dummySerial.dummy_buffer[3]);
  assertEqual(0x01, dummySerial.dummy_buffer[4]);
  assertEqual(0x06, dummySerial.dummy_buffer[5]);
  assertEqual(0x00, dummySerial.dummy_buffer[6]);
  assertEqual(0x00, dummySerial.dummy_buffer[7]);
  assertEqual(0x00, dummySerial.dummy_buffer[8]);
  assertEqual(0xFF, dummySerial.dummy_buffer[9]);
  assertEqual('t', dummySerial.dummy_buffer[10]);
  assertEqual('e', dummySerial.dummy_buffer[11]);
  assertEqual('s', dummySerial.dummy_buffer[12]);
  assertEqual('t', dummySerial.dummy_buffer[13]);
  assertEqual(0x00, dummySerial.dummy_buffer[14]);
  assertEqual(0x00, dummySerial.dummy_buffer[15]);
  assertEqual(0x00, dummySerial.dummy_buffer[16]);
  assertEqual(0x00, dummySerial.dummy_buffer[17]);
  assertEqual(0xBB, dummySerial.dummy_buffer[18]);
}


unittest(test_serial_can_receive)
{
  DummySerial dummySerial;

  // Serial CAN communication
  SerialCAN serialCAN{&dummySerial};  
  // An example CAN frame {arbitration_id, dlc, use_crc}
  Frame example_frame{0x00, 6, false};

  serialCAN.begin();
  assertEqual(0xAA, serialCAN.receive(&example_frame, 1));
  assertEqual(0x00, serialCAN.receive(&example_frame, 1));
  assertEqual(0x00, serialCAN.receive(&example_frame, 1));
  assertEqual(0x00, serialCAN.receive(&example_frame, 1));
  assertEqual(0x01, serialCAN.receive(&example_frame, 1));
  assertEqual(0x08, serialCAN.receive(&example_frame, 1));
  assertEqual(0xFF, serialCAN.receive(&example_frame, 1));
  assertEqual(0xFF, serialCAN.receive(&example_frame, 1));
  assertEqual(0xFF, serialCAN.receive(&example_frame, 1));
  assertEqual(0xFF, serialCAN.receive(&example_frame, 1));
  assertEqual(0xF0, serialCAN.receive(&example_frame, 1));
  assertEqual(0x00, serialCAN.receive(&example_frame, 1));
  assertEqual(0x00, serialCAN.receive(&example_frame, 1));
  assertEqual(0x00, serialCAN.receive(&example_frame, 1));
  assertEqual(0x00, serialCAN.receive(&example_frame, 1));
  assertEqual(0x00, serialCAN.receive(&example_frame, 1));
  assertEqual(0x00, serialCAN.receive(&example_frame, 1));
  assertEqual(0x00, serialCAN.receive(&example_frame, 1));
  assertEqual(0xBB, serialCAN.receive(&example_frame, 1));
}

unittest_main()


// -- END OF FILE --