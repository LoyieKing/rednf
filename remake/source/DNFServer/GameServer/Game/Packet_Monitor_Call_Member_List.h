// LinkHelper 核销批次：原 HdrCreate3 空骨架（sizeof=1、ctor 无定义）与
// shared/packet/include 权威定义（PacketHeader(0x4be,0x12) + m_idByChannel@0xa
// + m_charNo@0xe，packed，ORIG ctor W @0x0822ba76 仅基类调用）冲突。
// 本文件改为转发权威头：类由权威头 inline 定义产生（ctor W 经 COMDAT 外联）。
#include "../../shared/packet/include/Packet_Monitor_Call_Member_List.h"

#ifndef PACKET_MONITOR_CALL_MEMBER_LIST_H
#error "shared/packet/include/Packet_Monitor_Call_Member_List.h 未进入 include 路径"
#endif
