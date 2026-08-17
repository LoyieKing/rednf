//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_CALL_GUILD_LEVEL_UP_H
#define PACKET_MONITOR_CALL_GUILD_LEVEL_UP_H

#include "PacketHeader.h"

class Packet_Monitor_Call_Guild_Level_Up : public PacketHeader {
public:
    unsigned int m_uid;         // +0xa  Dispatcher_CallGuildLevelUp: CUser::GetUID（默认 0xffffffff=无用户）
    unsigned int m_charNo;      // +0xe
    unsigned int m_guildKey;    // +0x12
    unsigned char m_pad16;      // +0x16（第三轮穷举确认：SendGuildLevelup@846dd8a 及调用点
                                //      Dispatcher_CallGuildLevelUp@82001f0（GetUID→+a、
                                //      getCurCharacNo→+e、get_charac_guildkey→+12、
                                //      CheckGuildLevelUp 所需资金→+17）均不写本字节；
                                //      包 ctor（0x846f420）显式清零本字节（c6 40 16 00），
                                //      CMonitorServerProxy::SendGuildLevelup@8470ec4 同样不写；
                                //      guild OnCallGuildLevelUp 只读 +e/+12/+17——
                                //      纯对齐填充恒 0，m_fund 对齐到 +0x17，保留）
    unsigned int m_fund;        // +0x17（SendGuildLevelup param_4 = CheckGuildLevelUp 返回所需资金；
                                //      guild OnCallGuildLevelUp 校验并 SubGuildFund）
    Packet_Monitor_Call_Guild_Level_Up() : PacketHeader(0x408, 0x1b),
        m_uid(0xffffffffu), m_charNo(0), m_guildKey(0), m_pad16(0), m_fund(0)
    {

    };
} __attribute__((packed));


#endif //PACKET_MONITOR_CALL_GUILD_LEVEL_UP_H
