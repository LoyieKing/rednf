//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_PCROOM_RESPONSE_H
#define PACKET_PCROOM_RESPONSE_H

#include "PacketHeader.h"

class Packet_Pcroom_Response : public PacketHeader {
public:
    int m_uid;              // +0xa  GarenaAuthPCRoom 与 GM "pcroom" 分支均写 CUser::GetUID
    int m_accId;            // +0xe  两处均写 CUser::get_acc_id
    int m_field12;          // +0x12 两条填充路径（GarenaAuthPCRoom/GM pcroom）均恒写 1；
                            //      GarenaAuthPCRoom 仅当 GarenaCyberCafe(gcaType & 1)（Garena 網咖/
                            //      PC Room 账号；台服公开活动 "GCA 特約網咖"）时由 Inter_LoadEtc::
                            //      dispatch_sig 调用；接收端为外部 PCRoom 服务器（经 MsgQueue 0x17e
                            //      转发），协议未公开，无消费端证据，保留占位名
    int m_expireTime;       // +0x16 两处均写 time()+0x93a80（7 天后，推断为有效期）
    char m_field1a;         // +0x1a 两处均恒写 1（疑为 PC room 标志；外部协议未公开，无接收端证据）
    char m_field1b;         // +0x1b 两处均恒写 0xb（疑为 PC room 编号/类型；外部协议未公开，无证据）
    char m_pad1c[2];        // +0x1c..+0x1d ORIG ctor 与两条填充路径均未写入（外部协议未公开，布局待定）
    char m_fieldC;          // +0x1e ORIG ctor 清零、两条填充路径均未写入（语义无证据，已穷举内部+外部）

    Packet_Pcroom_Response() : PacketHeader(0xfaf, 0x1f),
        m_uid(0), m_accId(0), m_field12(0), m_expireTime(0), m_field1a(0), m_field1b(0), m_fieldC(0)
    {

    }
} __attribute__((packed));
TEST_CLASS_SIZE(Packet_Pcroom_Response, 0x1f);

#endif  // PACKET_PCROOM_RESPONSE_H
