//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_WEB_PROHIBIT_USER_CONNECT_H
#define PACKET_WEB_PROHIBIT_USER_CONNECT_H

#include "PacketHeader.h"

class Packet_Web_Prohibit_User_Connect : public PacketHeader {
public:
    Packet_Web_Prohibit_User_Connect() : PacketHeader(0x4c8, 0x13)
    {
        m_id = 0;
        m_time = 0;
        m_bIsConnect = 0;
        m_serverGroup = 0;
    }
    int m_id;                 // +0xa（日志 "m_id : %d"；FindProhibitUser/DeleteProhibitUser/GetCacheCharacter 键）
    char m_flag;              // +0xe（日志 "flag( %d )"；0=添加禁止，非0=删除；manager 回包置 0x7f）
    short m_time;             // +0xf（日志 "time( %d )"；SetUserConnectableTime(dbid, m_time, -1, true)；ORIG movswl 有符号扩展）
    char m_bIsConnect;        // +0x11（日志 "m_bIsConnect(%d)"；monitor 按用户是否在线置 0/1，manager 置 2）
    char m_serverGroup;       // +0x12（日志 "server group(%d)"；monitor 置 Get_ServerGroup()）
} __attribute__((packed));

#endif  // PACKET_WEB_PROHIBIT_USER_CONNECT_H
