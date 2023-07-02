#include "Frame.h"
#include <string.h>

using namespace serial_can;

template<typename T>
void Frame::encode(std::initializer_list<T> data_list) {
    const size_t T_size = sizeof(T);

    m_assert( "Maximum allowed number of bytes is 6 since use_crc is true.",
        (use_crc && data_list.size() * T_size <= 6) || !use_crc );
    m_assert( "Maximum allowed number of bytes is 8.",
        data_list.size() * T_size <= 8 );

    size_t current_start_byte = 0;
    for( auto elem : data_list )
    {
        packData_<T>(elem, current_start_byte);
        current_start_byte += T_size;
    }
}

void Frame::encode(const char* string) {
    size_t string_len = strlen(string);

    m_assert( "Maximum allowed number of bytes is 6 since use_crc is true.",
        (use_crc && string_len <= 6) || !use_crc );
    m_assert( "Maximum allowed number of bytes is 8.", string_len <= 8 );

    size_t current_start_byte = 0;
    for( int i = 0; i < string_len; i++ )
        packData_<char>(string[i], current_start_byte++);
}

template<typename T>
void Frame::packData_(T data, int start_byte) {
    for (int i = 0; i < sizeof(data); i++) {
        payload[i+start_byte] = data >> (i * 8);
    }
}

template void Frame::encode<char>(std::initializer_list<char>);
template void Frame::encode<int>(std::initializer_list<int>);
template void Frame::encode<uint8_t>(std::initializer_list<uint8_t>);
template void Frame::encode<uint16_t>(std::initializer_list<uint16_t>);
template void Frame::encode<uint32_t>(std::initializer_list<uint32_t>);
template void Frame::encode<int8_t>(std::initializer_list<int8_t>);
template void Frame::encode<int16_t>(std::initializer_list<int16_t>);
template void Frame::encode<int32_t>(std::initializer_list<int32_t>);

template void Frame::packData_<char>(char, int);
template void Frame::packData_<int>(int, int);
template void Frame::packData_<uint8_t>(uint8_t, int);
template void Frame::packData_<uint16_t>(uint16_t, int);
template void Frame::packData_<uint32_t>(uint32_t, int);
template void Frame::packData_<int8_t>(int8_t, int);
template void Frame::packData_<int16_t>(int16_t, int);
template void Frame::packData_<int32_t>(int32_t, int);