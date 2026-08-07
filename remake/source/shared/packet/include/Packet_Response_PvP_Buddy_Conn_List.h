//
// Created by loyieking on 24-3-30.
//

#ifndef REDNF_PACKET_RESPONSE_PVP_BUDDY_CONN_LIST_H
#define REDNF_PACKET_RESPONSE_PVP_BUDDY_CONN_LIST_H

#include "PacketHeader.h"
#include "PvPBuddyInfo.h"
#include <cstring>

#pragma pack(push, 1)
class Packet_Response_PvP_Buddy_Conn_List : public PacketHeader {
public:
    int charac_no;               // offset 0x0a
    int sTGameUserInfo_what3_0x05;               // offset 0x0e
    int buddyCount;               // offset 0x12
    PvPBuddyInfo buddies[32];  // offset 0x16
    // 原始：字段清零放成员初始化列表（在 buddies 数组隐式构造之前），
    // 之后再 memset 数组区（0x520）。顺序：base → 3×int → 32×PvPBuddyInfo() → memset。
    Packet_Response_PvP_Buddy_Conn_List() : PacketHeader(0x1b5b, 0x536), charac_no(0), sTGameUserInfo_what3_0x05(0), buddyCount(0) {
        memset(buddies, 0, sizeof(buddies));
    }
};
#pragma pack(pop)

TEST_CLASS_SIZE(Packet_Response_PvP_Buddy_Conn_List, 0x536);

#endif  // REDNF_PACKET_RESPONSE_PVP_BUDDY_CONN_LIST_H
