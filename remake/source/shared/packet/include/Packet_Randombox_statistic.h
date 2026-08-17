//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_RANDOMBOX_STATISTIC_H
#define PACKET_RANDOMBOX_STATISTIC_H

#include "PacketHeader.h"

class Packet_Randombox_statistic : public PacketHeader {
public:
    char m_boxKind;  // +a（SendRandomBoxStatistic 参数1；df_statics_r AddRandomboxStatistic 作
                     //      5 槽盒类下标 0..4（<0 或 >4 丢弃）；DBMW 按下标映射 box_kind：
                     //      0=randombox、2=emeraldbox，DB log_randombox.box_kind）
    char m_actionKind;  // +b（SendRandomBoxStatistic 参数2 类型 ENUM_RANDOMBOX_ACTION_KIND；
                        //      df_statics_r 0=create/1=open → m_randomboxA/B，DB log_randombox
                        //      create_count/open_count）

    Packet_Randombox_statistic() : PacketHeader(0x17b6, 0xc),
        m_boxKind(0), m_actionKind(0)
    {

    }
} __attribute__((packed));
TEST_CLASS_SIZE(Packet_Randombox_statistic, 0xc);

#endif  // PACKET_RANDOMBOX_STATISTIC_H
