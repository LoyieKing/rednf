// df_statics_r — UdpCharacteristic（ORIG UdpCharacteristic.o 拆分）
#include <stdio.h>

#include "UdpCharacteristic.h"
#include "DNFServerHandler.h"
#include "DNFFileLog.h"

struct UdpPingPacketView
{
    unsigned char m_hdr[0xa];
    unsigned short m_fieldA;
    unsigned short m_fieldB;
};
Packet_Frame_Lag_Statistic_Reload_Spec::Packet_Frame_Lag_Statistic_Reload_Spec()
    : PacketHeader(0xc29, 0xf)
{
}
Packet_Frame_Lag_Statistic_Write_Lag_Index::Packet_Frame_Lag_Statistic_Write_Lag_Index()
    : PacketHeader(0xc2b, 0x17b)
{
}
Packet_Frame_Lag_Statistic_Write_Query::Packet_Frame_Lag_Statistic_Write_Query()
    : PacketHeader(0xc2d, 0x40a)
{
}
Packet_Frame_Lag_Used_Memory_Write_Query::Packet_Frame_Lag_Used_Memory_Write_Query()
    : PacketHeader(0xc30, 0x40a)
{
}
UdpCharacteristic::UdpCharacteristic()
{
    m_successPartyTry = 0;
    m_totalPartyTry = 0;
    m_dungeonBadPing = 0;
    m_dungeonTotal = 0;
    m_pvpBadPing = 0;
    m_pvpTotal = 0;
    m_fairPvpBadPing = 0;
    m_fairPvpTotal = 0;
    m_successDungeonClear = 0;
    m_totalDungeonClear = 0;
}
UdpCharacteristic::~UdpCharacteristic()
{
}
void UdpCharacteristic::InitUdpCharacteristicData()
{
    m_successPartyTry = 0;
    m_totalPartyTry = 0;
    m_dungeonBadPing = 0;
    m_dungeonTotal = 0;
    m_pvpBadPing = 0;
    m_pvpTotal = 0;
    m_fairPvpBadPing = 0;
    m_fairPvpTotal = 0;
    m_successDungeonClear = 0;
    m_totalDungeonClear = 0;
}
void UdpCharacteristic::PushPartyResultData(Packet_Party_Result_Statistic* pkt)
{
    m_successPartyTry += ((UdpPingPacketView*)pkt)->m_fieldA;
    m_totalPartyTry += ((UdpPingPacketView*)pkt)->m_fieldB;
}
void UdpCharacteristic::PushPartyPingData(Packet_Party_Ping_Statistic* pkt)
{
    m_dungeonBadPing += ((UdpPingPacketView*)pkt)->m_fieldA;
    m_dungeonTotal += ((UdpPingPacketView*)pkt)->m_fieldB;
}
void UdpCharacteristic::PushPvpPingData(Packet_Pvp_Ping_Statistic* pkt)
{
    m_pvpBadPing += ((UdpPingPacketView*)pkt)->m_fieldA;
    m_pvpTotal += ((UdpPingPacketView*)pkt)->m_fieldB;
}
void UdpCharacteristic::PushFairPvpPingData(Packet_Fair_Pvp_Ping_Statistic* pkt)
{
    m_fairPvpBadPing += ((UdpPingPacketView*)pkt)->m_fieldA;
    m_fairPvpTotal += ((UdpPingPacketView*)pkt)->m_fieldB;
}
void UdpCharacteristic::PushAbnormalExitData(Packet_Abnormal_Exit_Statistic* pkt)
{
    m_successDungeonClear += ((UdpPingPacketView*)pkt)->m_fieldA;
    m_totalDungeonClear += ((UdpPingPacketView*)pkt)->m_fieldB;
}
void UdpCharacteristic::SaveUdpCharacteristicData(CServerHandler* handler, int interval)
{
    m_intervalTick += 1;
    if (interval <= m_intervalTick)
    {
        m_intervalTick = 0;
        Packet_Udp_Characteristic pkt;
        pkt.m_serverGroup = (char)handler->GetServerGroupNo();
        pkt.m_values[0] = m_successPartyTry;
        pkt.m_values[1] = m_totalPartyTry;
        pkt.m_values[6] = m_fairPvpBadPing;
        pkt.m_values[7] = m_fairPvpTotal;
        pkt.m_values[2] = m_dungeonBadPing;
        pkt.m_values[3] = m_dungeonTotal;
        pkt.m_values[4] = m_pvpBadPing;
        pkt.m_values[5] = m_pvpTotal;
        pkt.m_values[8] = m_successDungeonClear;
        pkt.m_values[9] = m_totalDungeonClear;
        handler->SendToDB((PacketHeader*)&pkt);
        InitUdpCharacteristicData();
    }
}
Packet_Udp_Characteristic::Packet_Udp_Characteristic()
    : PacketHeader(0xfaa, 0x33)
{
    m_serverGroup = 0;
    m_values[0] = 0;
    m_values[1] = 0;
    m_values[2] = 0;
    m_values[3] = 0;
    m_values[4] = 0;
    m_values[5] = 0;
    m_values[6] = 0;
    m_values[7] = 0;
    m_values[8] = 0;
    m_values[9] = 0;
}
