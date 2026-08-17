//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_STAT_COMPATIBILITY_INDEX_H
#define PACKET_STAT_COMPATIBILITY_INDEX_H

#include "PacketHeader.h"

class Packet_Stat_Compatibility_Index : public PacketHeader {
public:
    unsigned int m_accId;        // +0xa（Dispatcher_CompatibilityIndex::process: CUser::get_acc_id；DB ting_user_spec.m_id）
    unsigned char m_cpuVendor;   // +0xe（DB ting_user_spec.cpu_vendor）
    unsigned char m_cpuNum;      // +0xf（DB ting_user_spec.cpu_num）
    int m_cpuClock;              // +0x10（DB ting_user_spec.cpu_clock）
    unsigned short m_ram;        // +0x14（DB ting_user_spec.ram）
    unsigned short m_videoVendor;// +0x16（DB ting_user_spec.video_vendor）
    unsigned short m_videoDevice;// +0x18（DB ting_user_spec.video_device）
    unsigned short m_videoRam;   // +0x1a（DB ting_user_spec.video_ram）
    unsigned char m_os;          // +0x1c（DB ting_user_spec.os）
    unsigned char m_osBit;       // +0x1d（DB ting_user_spec.os_bit）

    Packet_Stat_Compatibility_Index() : PacketHeader(0x27e0, 0x1e),
        m_accId(0), m_cpuVendor(0), m_cpuNum(0), m_cpuClock(0), m_ram(0), m_videoVendor(0), m_videoDevice(0), m_videoRam(0), m_os(0), m_osBit(0)
    {

    }
} __attribute__((packed));
TEST_CLASS_SIZE(Packet_Stat_Compatibility_Index, 0x1e);


#endif //PACKET_STAT_COMPATIBILITY_INDEX_H
