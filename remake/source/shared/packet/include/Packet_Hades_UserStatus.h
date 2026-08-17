//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_HADES_USERSTATUS_H
#define PACKET_HADES_USERSTATUS_H

#include "PacketHeader.h"

class Packet_Hades_UserStatus : public PacketHeader {
public:
    unsigned int m_accId;   // +0x0a（Send_UserStatus @84b97fe：CUser::get_acc_id）
    unsigned int m_status;  // +0x0e（位域：bit0=secuType&(1|2|0x20)、bit1=secuType&8、
                            //      bit2=secuType&0x10；secuType 存 CUser+0x8e97d）
    Packet_Hades_UserStatus() : PacketHeader(0xfdd, 0x12),
        m_accId(0), m_status(0){};
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Hades_UserStatus, 0x12);

#endif  // PACKET_HADES_USERSTATUS_H
