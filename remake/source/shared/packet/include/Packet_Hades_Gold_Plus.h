//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_HADES_GOLD_PLUS_H
#define PACKET_HADES_GOLD_PLUS_H

#include "PacketHeader.h"

class Packet_Hades_Gold_Plus : public PacketHeader {
public:
    unsigned int m_accId;   // +0x0a（GoldPlus @84b93fa：CUser::get_acc_id）
    unsigned int m_gold;    // +0x0e（param_1：金币增量）
    unsigned short m_reason;// +0x12（param_3：ENUM_GOLD_CONTROL_REASON，0=普通/1=导入/2=拍卖）
    Packet_Hades_Gold_Plus() : PacketHeader(0xfda, 0x14),
        m_accId(0), m_gold(0), m_reason(0){};
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Hades_Gold_Plus, 0x14);

#endif  // PACKET_HADES_GOLD_PLUS_H
