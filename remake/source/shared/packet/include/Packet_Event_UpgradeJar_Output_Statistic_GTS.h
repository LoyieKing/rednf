//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_EVENT_UPGRADEJAR_OUTPUT_STATISTIC_GTS_H
#define PACKET_EVENT_UPGRADEJAR_OUTPUT_STATISTIC_GTS_H

#include "PacketHeader.h"

class Packet_Event_UpgradeJar_Output_Statistic_GTS : public PacketHeader {
public:
    int m_itemCode;  // +a（SendEventJarOutputItemStatistics：非罐子 Inven_Item+2 的产出道具 id）
    int m_itemCount; // +e（Inven_Item+7 罐子数量）

    Packet_Event_UpgradeJar_Output_Statistic_GTS() : PacketHeader(0x106e, 0x12),
        m_itemCode(0), m_itemCount(0)
    {

    }
} __attribute__((packed));
TEST_CLASS_SIZE(Packet_Event_UpgradeJar_Output_Statistic_GTS, 0x12);


#endif //PACKET_EVENT_UPGRADEJAR_OUTPUT_STATISTIC_GTS_H
