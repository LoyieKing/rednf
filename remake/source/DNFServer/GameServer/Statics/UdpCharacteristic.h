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
    unsigned char m_fieldA;  // +0xa
    int m_fieldB;            // +0xb
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
    char m_serverGroup;  // +0xa（p2pnetwork_statistic.server_group）
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
    int m_intervalTick;         // +0（保存间隔计数）
    int m_successPartyTry;      // +4（p2pnetwork_statistic.success_party_try）
    int m_totalPartyTry;        // +8（p2pnetwork_statistic.total_party_try）
    int m_dungeonBadPing;       // +0xc（p2pnetwork_statistic.dungeon_bad_ping）
    int m_dungeonTotal;         // +0x10（p2pnetwork_statistic.dungeon_total）
    int m_pvpBadPing;           // +0x14（p2pnetwork_statistic.pvp_bad_ping）
    int m_pvpTotal;             // +0x18（p2pnetwork_statistic.pvp_total）
    int m_fairPvpBadPing;       // +0x1c（p2pnetwork_statistic.fair_pvp_bad_ping）
    int m_fairPvpTotal;         // +0x20（p2pnetwork_statistic.fair_pvp_total）
    int m_successDungeonClear;  // +0x24（p2pnetwork_statistic.success_dungeon_clear）
    int m_totalDungeonClear;    // +0x28（p2pnetwork_statistic.total_dungeon_clear）
};

#endif // UDP_CHARACTERISTIC_H_
