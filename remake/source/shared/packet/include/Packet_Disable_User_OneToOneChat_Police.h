//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_DISABLE_USER_ONETOONECHAT_POLICE_H
#define PACKET_DISABLE_USER_ONETOONECHAT_POLICE_H

#include "PacketHeader.h"

class Packet_Disable_User_OneToOneChat_Police : public PacketHeader {
public:
    Packet_Disable_User_OneToOneChat_Police(): PacketHeader(0x1f43,0x30) {};
};


#endif //PACKET_DISABLE_USER_ONETOONECHAT_POLICE_H
