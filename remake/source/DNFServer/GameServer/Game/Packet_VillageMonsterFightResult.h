// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + VillageMonster_helpers.cpp
// Packet_VillageMonsterFightResult：ORIG 弱内联 ctor W @0x086b4b84（0x2b 字节）。
// has_def=1：完整复制定义自 VillageMonster_helpers.cpp:140（无基类的手工布局 struct，
// PacketHeader(0x1772,0x2b) 字节语义由发送侧填写；ctor 为隐式/空，ORIG W 为编译器内联）。
#ifndef DNF_GAME_PACKET_VILLAGEMONSTERFIGHTRESULT_H_
#define DNF_GAME_PACKET_VILLAGEMONSTERFIGHTRESULT_H_

struct Packet_VillageMonsterFightResult
{
    unsigned short m_header0;   // +0x00
    unsigned short m_header2;   // +0x02
    char m_pad4[6];             // +0x04
    unsigned int m_uids[4];     // +0x0a
    unsigned int m_characNos[4];// +0x1a
    unsigned char m_flag;       // +0x2a
};  // sizeof == 0x2b（与 ORIG 报文一致，手工布局已对齐）

#endif  // DNF_GAME_PACKET_VILLAGEMONSTERFIGHTRESULT_H_
