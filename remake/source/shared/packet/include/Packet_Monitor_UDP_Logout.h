//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_UDP_LOGOUT_H
#define PACKET_MONITOR_UDP_LOGOUT_H

#include "PacketHeader.h"
#include <string.h>

class Packet_Monitor_UDP_Logout : public PacketHeader {
public:
    unsigned int m_dbid;            // +0xa  FindUser
    unsigned char m_channel;        // +0xe
    int m_cache0;                   // +0xf  CACHE_CHARACTER_TYPE.m_field0
    unsigned int m_cache4;          // +0x13（第三轮穷举 2026-08-16：CGuildServerProxy::SendLogout
                                    //      @846dbfe 写 param_5；monitor ORIG OnLogout 反汇编
                                    //      无 +0x13 读取（cacheType.m_field4 用 m_channel 重建），
                                    //      +0x13 未消费；OnLogin 的 m_cache4 属
                                    //      Packet_Monitor_UDP_Login(+0x28) 另一包，语义无消费端证据）
    unsigned char m_foc;            // +0x17 Force of Connection
    unsigned int m_memberKey;       // +0x18
    char m_name[0x1e];              // +0x1c SendLogout strcpy(param_3 名字)
    unsigned char m_cacheFlag;      // +0x3a（第三轮穷举确认：monitor ORIG OnLogout 反汇编
                                    //      movzbl 0x3a(%eax) 读取并作 CacheCharacter 开关
                                    //      （!=0 才缓存）；发送侧 SendLogout 未写，ctor 默认 0
                                    //      （不缓存）；名字 m_cacheFlag 有消费证据保留）
    // 2026-08-16 修正：ORIG ctor（0x846f2d4）不初始化 +0x3a（m_cacheFlag）
    Packet_Monitor_UDP_Logout() : PacketHeader(0x3e9, 0x3b),
        m_dbid(0), m_channel(255), m_cache0(0), m_cache4(0), m_foc(0), m_memberKey(0)
    {
    memset(m_name, 0, sizeof(m_name));
    };
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_Monitor_UDP_Logout, 0x3b);

#endif  // PACKET_MONITOR_UDP_LOGOUT_H
