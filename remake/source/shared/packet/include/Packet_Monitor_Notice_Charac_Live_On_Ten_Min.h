//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_MONITOR_NOTICE_CHARAC_LIVE_ON_TEN_MIN_H
#define PACKET_MONITOR_NOTICE_CHARAC_LIVE_ON_TEN_MIN_H

#include "PacketHeader.h"

class Packet_Monitor_Notice_Charac_Live_On_Ten_Min : public PacketHeader {
public:
    Packet_Monitor_Notice_Charac_Live_On_Ten_Min(): PacketHeader(0x640,0xe) {};
};


#endif //PACKET_MONITOR_NOTICE_CHARAC_LIVE_ON_TEN_MIN_H
