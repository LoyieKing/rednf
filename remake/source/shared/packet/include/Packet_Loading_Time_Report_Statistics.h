//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_LOADING_TIME_REPORT_STATISTICS_H
#define PACKET_LOADING_TIME_REPORT_STATISTICS_H

#include "PacketHeader.h"

class Packet_Loading_Time_Report_Statistics : public PacketHeader {
public:
    Packet_Loading_Time_Report_Statistics(): PacketHeader(0xfb0,0x806) {};
};


#endif //PACKET_LOADING_TIME_REPORT_STATISTICS_H
