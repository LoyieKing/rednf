//
// 从 DBMW/DNFPacket.h 原样抽出（唯一声明点），供 statics 等 TU 直接使用真实类型
// ——statics 侧原先只能做强转视图，导致装载序 base-first（ORIG 为 index-first）。
//
#ifndef PACKET_FRAME_LAG_STATISTIC_RESULT_LOAD_SPEC_H
#define PACKET_FRAME_LAG_STATISTIC_RESULT_LOAD_SPEC_H

#include "PacketHeader.h"

class Packet_Frame_Lag_Statistic_Result_Load_Spec : public PacketHeader
{
public:
    Packet_Frame_Lag_Statistic_Result_Load_Spec();
    unsigned char m_loadSeq;             // +0xa（回显请求序号）
    int m_batchIndex;                    // +0xb（每 6 条发送后自增）
    int m_count;                         // +0xf
    int m_uniqueId[6];                   // +0x13
    union {
        unsigned int m_modifyTime[6];    // +0x2b（DBMW get_uint 视角）
        int m_modifyTimeInt[6];          // +0x2b（Statics int 视角：ORIG 单段直载）
    };
    int m_specId[6];                     // +0x43
    unsigned char m_cpuVendor[6];        // +0x5b
    unsigned char m_cpuProcessorNum[6];  // +0x61
    int m_aboveCpuClock[6];              // +0x67
    int m_belowCpuClock[6];              // +0x7f
    unsigned short m_ram[6];             // +0x97
    int m_videocardVendor[6];            // +0xa3
    int m_videocardDevice[6];            // +0xbb
    unsigned short m_videocardTextureMem[6]; // +0xd3
    unsigned char m_osVersion[6];        // +0xdf（总大小 0xe5）
} __attribute__((packed));

#endif  // PACKET_FRAME_LAG_STATISTIC_RESULT_LOAD_SPEC_H
