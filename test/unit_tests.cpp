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


#include <ArduinoUnitTests.h>

#include "Arduino.h"
#include "SerialCAN.h"

using namespace serial_can; 

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
  example_frame.encode<uint16_t>({0x15, 0xE2});

  assertEqual(0x1, example_frame.payload[0]);
  assertEqual(0x5, example_frame.payload[1]);
  assertEqual(0xE, example_frame.payload[2]);
  assertEqual(0x2, example_frame.payload[3]);
}

/// TODO: Test SerialCAN functionality

unittest_main()


// -- END OF FILE --