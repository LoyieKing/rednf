//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_FIND_FACTORY_HUB_USER_H
#define PACKET_MONITOR_FIND_FACTORY_HUB_USER_H

#include "PacketHeader.h"
#include <string.h>

class Packet_Monitor_Find_Factory_Hub_User : public PacketHeader {
public:
    unsigned int m_charNo;      // +0xa 请求者
    unsigned char m_nameLen;    // +0xe
    char m_name[0x1e];          // +0xf
    unsigned short m_field2e;   // +0x2d（第三轮穷举 2026-08-16：Inter_FindFactoryHub::dispatch_sig
                                //        @84dc97a 恒写 0x2c1（邻近 SetETC/addServerHackCnt 键
                                //        0x2c0/0x2bd，疑为 hack 分析键但无消费端反汇编证实）；
                                //        monitor OnMonitorFindFactoryHubUser 原样转发至
                                //        Packet_Notice_Find_Factory_Hub_User::m_field2e；
                                //        0x100f 游戏端接收不在语料，无语义名证据，保留）
    unsigned int m_accId;       // +0x2f（Inter_FindFactoryHub::dispatch_sig: CUser::get_acc_id）
    Packet_Monitor_Find_Factory_Hub_User() : PacketHeader(0x100e, 0x33),
        m_charNo(0), m_nameLen(0), m_field2e(0), m_accId(0)
    {
    memset(m_name, 0, sizeof(m_name));
    };
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Monitor_Find_Factory_Hub_User, 0x33);


#endif //PACKET_MONITOR_FIND_FACTORY_HUB_USER_H
