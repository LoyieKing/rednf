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
    // ORIG C1 0x84b9a1e W（0x23）：仅写 PacketHeader 头，不清零载荷——acc_id/status
    // 由调用方 CHades::Send_UserStatus 自行覆盖。
    Packet_Hades_UserStatus() : PacketHeader(0xfdd, 0x12) {};
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Hades_UserStatus, 0x12);

#endif  // PACKET_HADES_USERSTATUS_H
