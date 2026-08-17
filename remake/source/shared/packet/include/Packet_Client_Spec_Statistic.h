//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_CLIENT_SPEC_STATISTIC_H
#define PACKET_CLIENT_SPEC_STATISTIC_H

#include "PacketHeader.h"

class Packet_Client_Spec_Statistic : public PacketHeader {
public:
    unsigned char m_type;        // +0xa（df_statics_r Packet_ClientSpecLocal.m_type；DisConnSig=1 / Dispatcher send=2|0）
    unsigned char m_count;       // +0xb（ClientSpecLocal.m_count；getClientHWSpec[1] 记录数）
    int m_errorCode;             // +0xc（DisConnSig: local_cb6=param_4；ClientSpecLocal.m_errorCode）
    unsigned short m_errorLine;  // +0x10（DisConnSig: GetLastLoginChannelNo；ClientSpecLocal.m_errorLine）
    char m_spec[0xbf4];          // +0x12..+0xc05（0xc/条 HWSpec 记录，共 m_count 个；子字段
                                 //      +0=m_category1(uchar)、+4=m_category2(uint)、
                                 //      +8=m_category3(uint)，见 df_statics_r HWSpecResearcher.h
                                 //      HWSpec / DB log_hardware_ting(category1,category2,category3)；
                                 //      DisConnSig / Dispatcher_Client_Spec_Statistic::send memcpy 目标）

    Packet_Client_Spec_Statistic() : PacketHeader(0x9ce, 0xc06),
        m_count(0)
    {

    }
} __attribute__((packed));
TEST_CLASS_SIZE(Packet_Client_Spec_Statistic, 0xc06);

#endif  // PACKET_CLIENT_SPEC_STATISTIC_H
