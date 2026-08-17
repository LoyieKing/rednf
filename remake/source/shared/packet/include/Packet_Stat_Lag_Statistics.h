//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_STAT_LAG_STATISTICS_H
#define PACKET_STAT_LAG_STATISTICS_H

#include "PacketHeader.h"

class Packet_Stat_Lag_Statistics : public PacketHeader {
public:
    char m_recordCount;  // +a（dispatch_sig 首字节记录数，<8，外层循环上界）
    struct __attribute__((packed)) LagModuleRecord {
        union {
            int m_int;      // +0（dispatch_sig get_int 写入；AddLagStatistics 先判 m_int!=0）
            float m_float;  // +0（AddLagStatistics 经 LagModuleWire.m_u.m_float 读平均）
        } m_avg;            // +0..+3（DB lag_stat_module.average；wire 流式模块号字节
                            //         dispatch_sig 仅作槽位索引 local_15、不落盘，
                            //         故 ORIG 槽内无 m_moduleNo 成员：10B 槽=avg+dev+count）
        float m_deviation;  // +4..+7（dispatch_sig get_int 写入、AddLagStatistics 按 float 读；DB deviation）
        unsigned short m_count;  // +8..+9（dispatch_sig get_short；DB count）
    } m_records[8];         // +b..+5a（8×10B 模块记录槽，槽位下标即模块号 0..7）
    char m_detailCount;  // +5b（index==1 记录附带的明细条数，≤10，内层循环上界）
    struct __attribute__((packed)) LagDetailRecord {
        unsigned short m_key;    // +0（dispatch_sig get_short；AddLagStatistics map key）
        char m_pad1;             // +2（dispatch_sig get_byte 占位）
        float m_a;               // +3（dispatch_sig get_int；→ STDungeonLagStatistics.m_data[0]）
        float m_b;               // +7（dispatch_sig get_int；→ m_data[1]）
        unsigned short m_cntA;   // +0xb（dispatch_sig get_short；→ m_data[2]）
        char m_pad2;             // +0xd（dispatch_sig get_byte 占位）
        float m_c;               // +0xe（dispatch_sig get_int；→ m_data[4]）
        float m_d;               // +0x12（dispatch_sig get_int；→ m_data[5]）
        unsigned short m_cntB;   // +0x16（dispatch_sig get_short；→ m_data[6]）
    } m_details[10];        // +5c..+14b（10×24B dungeon 明细槽，与 Statics LagDungeonWire::Dun 逐槽位一致）

    Packet_Stat_Lag_Statistics() : PacketHeader(0x17b9, 0x14c),
        m_recordCount(0), m_detailCount(0)
    {

    }
} __attribute__((packed));
TEST_CLASS_SIZE(Packet_Stat_Lag_Statistics, 0x14c);

#endif  // PACKET_STAT_LAG_STATISTICS_H
