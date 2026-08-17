//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_HADES_GOLDTRACKING_H
#define PACKET_HADES_GOLDTRACKING_H

#include "PacketHeader.h"
#include <string.h>

class Packet_Hades_GoldTracking : public PacketHeader {
public:
    unsigned int m_accId;      // +0x0a（GoldTracking @84b949a：CUser::get_acc_id）
    unsigned int m_characId;   // +0x0e（param_2；AddAuctionMoney 路径实传 acc_id，语义待定）
    char m_ip[16];             // +0x12（param_3：IP 字符串，memset 0x10 后 strncpy 0x10；
                               //      NULL 时全 0；AddAuctionMoney 传 GetIP、选角 Inter 传
                               //      硬编码 "127.0.0.1" @0x8c86f40）
    unsigned int m_gold;       // +0x22（param_4：金币量，语义待定）
    unsigned short m_usedFatigue; // +0x26（param_5：3 个调用点均传 getCurCharacUsedFatigue）
    unsigned char m_eventType; // +0x28（param_1：2=邮寄/选角，4=拍卖）
    Packet_Hades_GoldTracking() : PacketHeader(0xfdc, 0x29),
        m_accId(0), m_characId(0), m_gold(0), m_usedFatigue(0), m_eventType(0)
    {
        memset(m_ip, 0, sizeof(m_ip));
    };
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Hades_GoldTracking, 0x29);

#endif  // PACKET_HADES_GOLDTRACKING_H
