// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + CValueStatistic.cpp
// Packet_Value_Statistic：ORIG 弱内联 ctor W @0x086100ca。
// has_def=1：完整复制定义自 CValueStatistic.cpp:15（InsertValueStatistic 反汇编布局证据）。
#ifndef DNF_GAME_PACKET_VALUE_STATISTIC_H_
#define DNF_GAME_PACKET_VALUE_STATISTIC_H_

struct Packet_Value_Statistic
{
    char m_header[0x06];  // +0x00 包头(6 字节)
    int m_level;          // +0x06
    int m_values[30];     // +0x0a（0x78 字节）
};

#endif  // DNF_GAME_PACKET_VALUE_STATISTIC_H_
