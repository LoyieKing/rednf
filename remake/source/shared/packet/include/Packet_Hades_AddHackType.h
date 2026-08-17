//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_HADES_ADDHACKTYPE_H
#define PACKET_HADES_ADDHACKTYPE_H

#include "PacketHeader.h"

class Packet_Hades_AddHackType : public PacketHeader {
public:
    unsigned int m_accId;      // +0x0a（AddHackTypeCnt @84b9328：CUser::get_acc_id）
    unsigned short m_hackType; // +0x0e（param_1：ENUM_HACKTYPE，(u16) 截断）
    unsigned short m_count;    // +0x10（param_2：hack 次数）
    unsigned int m_field12;    // +0x12（param_3 原样落包；语义无内部证据，待定）
    unsigned int m_field16;    // +0x16（param_4 原样落包；语义无内部证据，待定）
    Packet_Hades_AddHackType() : PacketHeader(0xfd7, 0x1a),
        m_accId(0), m_hackType(0), m_count(0), m_field12(0), m_field16(0){};
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Hades_AddHackType, 0x1a);

#endif  // PACKET_HADES_ADDHACKTYPE_H
