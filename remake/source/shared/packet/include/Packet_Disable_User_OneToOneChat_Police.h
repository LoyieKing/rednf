//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_DISABLE_USER_ONETOONECHAT_POLICE_H
#define PACKET_DISABLE_USER_ONETOONECHAT_POLICE_H

#include "PacketHeader.h"
#include <string.h>

class Packet_Disable_User_OneToOneChat_Police : public PacketHeader {
public:
    unsigned int m_gmId;        // +0xa（dispatch_sig：CUserCharacInfo::getCurCharacNo → GM 的 characNo；Monitor 端按 GM id 使用；保留旧名因 .cpp 引用未同步）
    unsigned int m_nameLength;  // +0xe（dispatch_sig：PacketBuf::get_int → 名字长度，作为 get_str 上限 0x1e 的实参）
    char m_name[0x1e];          // +0x12（dispatch_sig：PacketBuf::get_str 写入目标用户名）
    Packet_Disable_User_OneToOneChat_Police() : PacketHeader(0x1f43, 0x30),
        m_gmId(0)
    {
    memset(m_name, 0, sizeof(m_name));
    };
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Disable_User_OneToOneChat_Police, 0x30);


#endif //PACKET_DISABLE_USER_ONETOONECHAT_POLICE_H
