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
    m_field4 = 0;
    m_field8 = 0;
    m_fieldc = 0;
    m_field10 = 0;
    m_field14 = 0;
    m_field18 = 0;
    m_field1c = 0;
    m_field20 = 0;
    m_field24 = 0;
    m_field28 = 0;
}
UdpCharacteristic::~UdpCharacteristic()
{
}
void UdpCharacteristic::InitUdpCharacteristicData()
{
    m_field4 = 0;
    m_field8 = 0;
    m_fieldc = 0;
    m_field10 = 0;
    m_field14 = 0;
    m_field18 = 0;
    m_field1c = 0;
    m_field20 = 0;
    m_field24 = 0;
    m_field28 = 0;
}
void UdpCharacteristic::PushPartyResultData(Packet_Party_Result_Statistic* pkt)
{
    m_field4 += ((UdpPingPacketView*)pkt)->m_fieldA;
    m_field8 += ((UdpPingPacketView*)pkt)->m_fieldB;
}
void UdpCharacteristic::PushPartyPingData(Packet_Party_Ping_Statistic* pkt)
{
    m_fieldc += ((UdpPingPacketView*)pkt)->m_fieldA;
    m_field10 += ((UdpPingPacketView*)pkt)->m_fieldB;
}
void UdpCharacteristic::PushPvpPingData(Packet_Pvp_Ping_Statistic* pkt)
{
    m_field14 += ((UdpPingPacketView*)pkt)->m_fieldA;
    m_field18 += ((UdpPingPacketView*)pkt)->m_fieldB;
}
void UdpCharacteristic::PushFairPvpPingData(Packet_Fair_Pvp_Ping_Statistic* pkt)
{
    m_field1c += ((UdpPingPacketView*)pkt)->m_fieldA;
    m_field20 += ((UdpPingPacketView*)pkt)->m_fieldB;
}
void UdpCharacteristic::PushAbnormalExitData(Packet_Abnormal_Exit_Statistic* pkt)
{
    m_field24 += ((UdpPingPacketView*)pkt)->m_fieldA;
    m_field28 += ((UdpPingPacketView*)pkt)->m_fieldB;
}
void UdpCharacteristic::SaveUdpCharacteristicData(CServerHandler* handler, int interval)
{
    m_field0 += 1;
    if (interval <= m_field0)
    {
        m_field0 = 0;
        Packet_Udp_Characteristic pkt;
        pkt.m_fieldA = (char)handler->GetServerGroupNo();
        pkt.m_values[0] = m_field4;
        pkt.m_values[1] = m_field8;
        pkt.m_values[6] = m_field1c;
        pkt.m_values[7] = m_field20;
        pkt.m_values[2] = m_fieldc;
        pkt.m_values[3] = m_field10;
        pkt.m_values[4] = m_field14;
        pkt.m_values[5] = m_field18;
        pkt.m_values[8] = m_field24;
        pkt.m_values[9] = m_field28;
        handler->SendToDB((PacketHeader*)&pkt);
        InitUdpCharacteristicData();
    }
}
Packet_Udp_Characteristic::Packet_Udp_Characteristic()
    : PacketHeader(0xfaa, 0x33)
{
    m_fieldA = 0;
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
