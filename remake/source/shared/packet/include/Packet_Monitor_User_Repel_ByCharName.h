//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_USER_REPEL_BYCHARNAME_H
#define PACKET_MONITOR_USER_REPEL_BYCHARNAME_H

#include "PacketHeader.h"
#include <string.h>

class Packet_Monitor_User_Repel_ByCharName : public PacketHeader {
public:
    unsigned int m_fieldA;      // +0xa（第三轮穷举 2026-08-16：CMonitorServerProxy::SendUserRepel
                                //      @847101a 写 param_2→+0xa、param_1→+0xe、param_3→+0x12；
                                //      语料无调用点；monitor ORIG OnUserRepelByCharName 反汇编
                                //      仅读 +0xe/+0x12（mov 0xe / lea 0x12），未读 +0xa；
                                //      语义无证据，保留占位名）
    unsigned int m_dbid;        // +0xe FindUser
    char m_name[0x1e];          // +0x12
    Packet_Monitor_User_Repel_ByCharName() : PacketHeader(0x4d0, 0x30)
    {
    memset(m_name, 0, sizeof(m_name));
    };
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Monitor_User_Repel_ByCharName, 0x30);


#endif //PACKET_MONITOR_USER_REPEL_BYCHARNAME_H
