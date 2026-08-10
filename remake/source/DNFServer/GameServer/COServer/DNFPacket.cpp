// df_coserver_r — coserver 专用包结构构造（仿 ORIG CoserverPacket.cpp 布局，
//                4 个构造同文件连续排布以复现 ORIG 尾对齐 nop）
#include "DNFApplication.h"
#include "DNFUdpHandler.h"

Packet_CutOff_UDP_Call_UserInfo::Packet_CutOff_UDP_Call_UserInfo()
    : PacketHeader(0x3f9, 10)
{
}

Packet_Monitor_UDP_User_Getout::Packet_Monitor_UDP_User_Getout()
    : PacketHeader(0x3ee, 0xe)
{
    m_userID = 0;
}

Packet_Monitor_Event_Start::Packet_Monitor_Event_Start()
    : PacketHeader(0x44f, 0x12)
{
}

Packet_Monitor_Event_End::Packet_Monitor_Event_End()
    : PacketHeader(0x450, 0xe)
{
}
