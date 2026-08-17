//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_LOADING_TIME_REPORT_STATISTICS_H
#define PACKET_LOADING_TIME_REPORT_STATISTICS_H

#include "PacketHeader.h"

class Packet_Loading_Time_Report_Statistics : public PacketHeader {
public:
    int m_vals[9];                   // +0xa..+0x2d（Dispatcher_LoadingTimeReport 读客户端 9 项；接收 LoadingValsWire.m_vals → LoadingTimeReport.m_data）
    unsigned int m_accId;            // +0x2e（sender CUser::get_acc_id；receiver m_key → key.m_mId；DB powerwar_loading/lag.m_id）
    unsigned short m_pwCount;        // +0x32（sender local_1c，<=99）
    struct __attribute__((packed)) PwItem {
        unsigned short m_round;      // +0（receiver m_key → key.m_round；DB round）
        unsigned char m_player;      // +2（receiver m_f0 → m_player；DB player）
        unsigned short m_myLoading;  // +3（receiver m_f2 → m_myLoading；DB my_loading）
        unsigned short m_otherLoading; // +5（receiver m_f4 → m_otherLoading；DB other_loading）
        unsigned short m_vsLoading;  // +7（receiver m_f6 → m_vsLoading；DB vs_loading）
    } m_pw[0x64];                    // +0x34，步长 9，至 +0x3b8
    unsigned short m_lagCount;       // +0x3b8（sender local_1e，<=99）
    struct __attribute__((packed)) LagItem {
        unsigned short m_round;      // +0（receiver m_key → key.m_round；DB round）
        unsigned char m_player;      // +2（receiver m_f0 → m_player；DB player）
        unsigned int m_lagAvg;       // +3（receiver m_f4 → m_lagAvg；DB lag_avg）
        unsigned int m_lagCnt;       // +7（receiver m_f8 → m_lagCnt；DB lag_cnt）
    } m_lag[0x64];                   // +0x3ba，步长 0xb，至 +0x806

    Packet_Loading_Time_Report_Statistics(): PacketHeader(0xfb0,0x806) {};
} __attribute__((packed));
TEST_CLASS_SIZE(Packet_Loading_Time_Report_Statistics, 0x806);


#endif //PACKET_LOADING_TIME_REPORT_STATISTICS_H
