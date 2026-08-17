//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_HADES_GOLD_MINUS_H
#define PACKET_HADES_GOLD_MINUS_H

#include "PacketHeader.h"

class Packet_Hades_Gold_Minus : public PacketHeader {
public:
    unsigned int m_accId;   // +0x0a（GoldMinus @84b944a：CUser::get_acc_id）
    unsigned int m_gold;    // +0x0e（param_1：金币减量）
    unsigned short m_reason;// +0x12（param_3：ENUM_GOLD_CONTROL_REASON，0=普通/1=导入/2=拍卖）
    Packet_Hades_Gold_Minus() : PacketHeader(0xfdb, 0x14),
        m_accId(0), m_gold(0), m_reason(0){};
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Hades_Gold_Minus, 0x14);

#endif  // PACKET_HADES_GOLD_MINUS_H
