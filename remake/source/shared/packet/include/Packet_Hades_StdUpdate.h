//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_HADES_STDUPDATE_H
#define PACKET_HADES_STDUPDATE_H

#include "PacketHeader.h"

class Packet_Hades_StdUpdate : public PacketHeader {
public:
    int m_accId;             // +a（StdCateUpdate: CUser::get_acc_id）
    unsigned short m_stdCate; // +e（StdCateUpdate param_1，方法名即 "Standard Category Update"；
                             //     全量 4 个调用点传 1/2/3：CHades::UseFatigue→1（疲劳消耗）、
                             //     CParty::dungeon_start→2（组队副本开始，逐成员）、
                             //     CHades::needLogin 与 Inter_LoginInfo::dispatch_sig→3（登录/补发登录）；
                             //     接收端为 Hades 反外挂服务器（member_punish_info 落库 "HADES" 处置），
                             //     外部协议无公开文档，已穷举内部 + 外部）
    unsigned short m_fieldC; // +10（StdCateUpdate param_2：全量 4 个调用点均恒传 0；
                             //     疑为子类别/数值，但无非零调用点证据，保留占位名）

    Packet_Hades_StdUpdate() : PacketHeader(0xfd9, 0x12),
        m_accId(0), m_stdCate(0), m_fieldC(0)
    {

    }
} __attribute__((packed));
TEST_CLASS_SIZE(Packet_Hades_StdUpdate, 0x12);

#endif  // PACKET_HADES_STDUPDATE_H
