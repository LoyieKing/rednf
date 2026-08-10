#ifndef UDP_CHARACTERISTIC_H_
#define UDP_CHARACTERISTIC_H_

#include "PacketHeader.h"

class CServerHandler;
class Packet_Pvp_Ping_Statistic;
class Packet_Party_Ping_Statistic;
class Packet_Fair_Pvp_Ping_Statistic;
class Packet_Party_Result_Statistic;
class Packet_Abnormal_Exit_Statistic;

// ---- FrameLag 专用包（0xc29-0xc30 族，UdpCharacteristic 使用）----
#pragma pack(push, 1)
class Packet_Frame_Lag_Statistic_Reload_Spec : public PacketHeader
{
public:
    Packet_Frame_Lag_Statistic_Reload_Spec();
};

class Packet_Frame_Lag_Statistic_Write_Lag_Index : public PacketHeader
{
public:
    Packet_Frame_Lag_Statistic_Write_Lag_Index();
    char m_data[0x171];  // +0xa（packetSize 0x17b）
};

class Packet_Frame_Lag_Statistic_Write_Query : public PacketHeader
{
public:
    Packet_Frame_Lag_Statistic_Write_Query();
    char m_data[0x400];  // +0xa（packetSize 0x40a）
};

class Packet_Frame_Lag_Used_Memory_Write_Query : public PacketHeader
{
public:
    Packet_Frame_Lag_Used_Memory_Write_Query();
    char m_data[0x400];  // +0xa（packetSize 0x40a）
};

class Packet_Udp_Characteristic : public PacketHeader
{
public:
    Packet_Udp_Characteristic();
    char m_fieldA;    // +0xa
    int m_values[10]; // +0xb
} __attribute__((packed));
#pragma pack(pop)

// ---- UdpCharacteristic：0x2C ----
class UdpCharacteristic
{
public:
    UdpCharacteristic();
    ~UdpCharacteristic();
    void PushPvpPingData(Packet_Pvp_Ping_Statistic* pkt);
    void PushPartyPingData(Packet_Party_Ping_Statistic* pkt);
    void PushFairPvpPingData(Packet_Fair_Pvp_Ping_Statistic* pkt);
    void PushPartyResultData(Packet_Party_Result_Statistic* pkt);
    void PushAbnormalExitData(Packet_Abnormal_Exit_Statistic* pkt);
    void InitUdpCharacteristicData();
    void SaveUdpCharacteristicData(CServerHandler* handler, int interval);
    int m_field0;   // +0
    int m_field4;   // +4
    int m_field8;   // +8
    int m_fieldc;   // +0xc
    int m_field10;  // +0x10
    int m_field14;  // +0x14
    int m_field18;  // +0x18
    int m_field1c;  // +0x1c
    int m_field20;  // +0x20
    int m_field24;  // +0x24
    int m_field28;  // +0x28
};

#endif // UDP_CHARACTERISTIC_H_
