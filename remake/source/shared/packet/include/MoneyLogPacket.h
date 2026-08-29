//
// MoneyLogPacket —— 金钱日志统计包（ORIG df_game_r 0x08694602 W C1/C2）。
// 布局：PacketHeader(10B) + 5×int（+0x0a..+0x1d），sizeof 0x1e。
// ctor：PacketHeader(0x2723, 0x1e) + 5 int 清零（定义见 OnlyOrigBatch4_Force.cpp）。
// 使用点：CUser::SendMoenyLog（ORIG 0x0868f472 对象 @-0x2a，字段 +0xa/+0xe/+0x12/
//   +0x16/+0x1a 实证）。
//

#ifndef MONEYLOGPACKET_H
#define MONEYLOGPACKET_H

#include "PacketHeader.h"

class MoneyLogPacket : public PacketHeader {
public:
    int m_characNo;        // +0x0a
    int m_money0;          // +0x0e（m_moneyLogs[i].m_money[0]）
    int m_money1;          // +0x12（m_moneyLogs[i].m_money[1]）
    unsigned int m_accId;  // +0x16
    int m_money2;          // +0x1a（m_moneyLogs[i].m_money[2]）

    // ORIG W 0x08694602（C2 同址）：PacketHeader(0x2723, 0x1e) + 5 int 清零。
    MoneyLogPacket();
} __attribute__((packed));

TEST_CLASS_SIZE(MoneyLogPacket, 0x1e);

#endif  // MONEYLOGPACKET_H
