// HdrCreate3: ORIG exported symbols, first .h declaration.
// Evidence: nm ORIG（nm_44 仅 ctor）+ 定义提取自 Game/DisPatcherCore.cpp:12282-12309。
// 纯 wire POD（无 getter 证据）：各标量域按 asm 取数偏移具名，rows/entries/tail
// 区为交叠别名寻址，以平坦缓冲复现。ctor PacketHeader(0xc26, 0x186) 外联在 cpp，
// 接入时由宿主迁除 cpp 内重复段。
#ifndef DNF_GAME_PACKET_FRAME_LAG_STATISTIC_ADD_H_
#define DNF_GAME_PACKET_FRAME_LAG_STATISTIC_ADD_H_

#include "PacketHeader.h"

#pragma pack(push, 1)
class Packet_Frame_Lag_Statistic_Add : public PacketHeader
{
public:
    Packet_Frame_Lag_Statistic_Add();  // ORIG W 0x0822bcf4
    int m_fieldA;                // +0x0a（首个 get_int）
    char m_field0e;              // +0x0e
    char m_field0f;              // +0x0f
    unsigned char m_pad10[2];    // +0x10 驻留（wire 段间空档）
    unsigned short m_field12;    // +0x12
    unsigned short m_field14;    // +0x14
    short m_field16;             // +0x16
    short m_field18;             // +0x18
    char m_field1a;              // +0x1a
    int m_field1b;               // +0x1b（奇偏移，pack(1)）
    char m_field1f;              // +0x1f
    short m_field20;             // +0x20
    short m_field22;             // +0x22
    struct BodyTail {
        unsigned char m_bodyHead[0x14c];
        unsigned char m_tailFlag[6];
        short m_tailShort[6];
        unsigned char m_tailPad[4];
    };
    union {
        unsigned char m_body[0x162];
        BodyTail m_tail;
    };
};                               // sizeof = 0x186（ctor 参数 size）
#pragma pack(pop)

// C++98 形式断言（与 cpp 内断言同义）
typedef char assert_packet_frame_lag_statistic_add_size[
    (sizeof(Packet_Frame_Lag_Statistic_Add) == 0x186) ? 1 : -1];

#endif  // DNF_GAME_PACKET_FRAME_LAG_STATISTIC_ADD_H_
