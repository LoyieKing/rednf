//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_REASON_CRASH_DOWN_INFO_H
#define PACKET_REASON_CRASH_DOWN_INFO_H

#include "PacketHeader.h"

class Packet_Reason_Crash_Down_Info : public PacketHeader {
public:
    int m_occTime;  // +a（CarryClientCrashDown：CSystemTime::getCurSec；统计端 SQL log_client_ting_stat.occ_time）
    int m_channel;  // +e（CarryClientCrashDown：CEnvironment::get_channel_no；SQL channel_no）
    int m_reason;  // +12（CarryClientCrashDown：统计 map key（崩溃/断线原因码）；SQL reason）
    int m_cnt;  // +16（CarryClientCrashDown：统计 map value（次数）；SQL cnt）

    Packet_Reason_Crash_Down_Info() : PacketHeader(0xc48, 0x1a),
        m_occTime(0), m_channel(0), m_reason(0), m_cnt(0)
    {

    }
} __attribute__((packed));
TEST_CLASS_SIZE(Packet_Reason_Crash_Down_Info, 0x1a);

#endif  // PACKET_REASON_CRASH_DOWN_INFO_H
