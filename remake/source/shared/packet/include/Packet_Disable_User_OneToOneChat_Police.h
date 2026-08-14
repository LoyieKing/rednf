//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_DISABLE_USER_ONETOONECHAT_POLICE_H
#define PACKET_DISABLE_USER_ONETOONECHAT_POLICE_H

#include "PacketHeader.h"

class Packet_Disable_User_OneToOneChat_Police : public PacketHeader {
public:
    unsigned int m_gmId;        // +0xa
    unsigned int m_fieldE;      // +0xe
    char m_name[0x1e];          // +0x12
    Packet_Disable_User_OneToOneChat_Police(): PacketHeader(0x1f43,0x30) {};
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Disable_User_OneToOneChat_Police, 0x30);


#endif //PACKET_DISABLE_USER_ONETOONECHAT_POLICE_H
