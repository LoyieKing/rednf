#ifndef FRAME_LAG_COLLECTOR_H_
#define FRAME_LAG_COLLECTOR_H_

#include <map>

#include "PacketHeader.h"

class CServerHandler;
class FrameLagStruct;
class Packet_Frame_Lag_Statistic_Add;
class Packet_Frame_Lag_Collect_Interval_Check : public PacketHeader
{
public:
    unsigned short m_fieldA;  // +0xa
} __attribute__((packed));
class Packet_Frame_Lag_Spec_Delete_Notify;
class Packet_Frame_Lag_Statistic_Result_Load_Spec;
class Packet_Frame_Lag_Statistic_Result_Reload_Spec;

// ---- FrameLag 专用包（0xc27 族，Load_Spec）----
#pragma pack(push, 1)
class Packet_Frame_Lag_Statistic_Load_Spec : public PacketHeader
{
public:
    Packet_Frame_Lag_Statistic_Load_Spec();
};
#pragma pack(pop)

// ---- FrameLagCollector：0x1E8 ----
class FrameLagCollector
{
public:
    class UsedMemoryStruct
    {
    public:
        void init();
        void SetUsedMemory(char idx, short value);
        int m_counts[6];   // +0
        int m_sums[6];     // +0x18
    };
    class DirectxVersionStruct
    {
    public:
        void init();
        void add_cnt(unsigned int version);
        int m_data[8];     // +0（0x20）
    };
    class FrameLagDataStruct
    {
    public:
        FrameLagDataStruct();
        void init();
        char m_data[0x248];  // +0（实际大小 0x248，map 节点 0x25c）
    };
    class MonitoringSpecCase
    {
    public:
        char m_data[0x20];  // +0（实际大小 0x20，map 节点 0x34）
    };

    FrameLagCollector();
    ~FrameLagCollector();
    void ReLoadSpec(CServerHandler* handler);
    void RenewToday();
    void SaveUsedMemory(CServerHandler* handler);
    int SaveDailyBadSpec(CServerHandler* handler);
    int SaveFrameLagData(CServerHandler* handler);
    int GetCollectInterval();
    int PushOneFrameLagData(Packet_Frame_Lag_Statistic_Add* pkt);
    int CollectIntervalCheck(Packet_Frame_Lag_Collect_Interval_Check* pkt);
    int PopMonitoringSpecData(Packet_Frame_Lag_Spec_Delete_Notify* pkt);
    int PushMonitoringSpecData(Packet_Frame_Lag_Statistic_Result_Load_Spec* pkt);
    int PushMonitoringSpecData(Packet_Frame_Lag_Statistic_Result_Reload_Spec* pkt);
    int is_valid_statistic_packet(Packet_Frame_Lag_Statistic_Add* pkt);
    int SaveCollectedDirectxVersion(CServerHandler* handler);
    bool Init();
    void LoadSpec(CServerHandler* handler);
    void accFrameLagStruct(FrameLagDataStruct& data, FrameLagStruct* pkt);

    unsigned char m_field0;             // +0
    int m_field4;                       // +4
    int m_field8;                       // +8
    int m_fieldc;                       // +0xc
    int m_field10;                      // +0x10
    int m_field14;                      // +0x14
    char m_field18;                     // +0x18
    char m_field19;                     // +0x19
    std::map<int, char> m_map1c;        // +0x1c
    std::map<int, char> m_map34;        // +0x34
    int m_field4c;                      // +0x4c
    int m_field50;                      // +0x50
    std::map<int, MonitoringSpecCase> m_monitor;  // +0x54
    int m_field6c;                      // +0x6c
    std::map<int, FrameLagDataStruct> m_data;     // +0x70
    int m_collectInterval;              // +0x88
    int m_field8c;                      // +0x8c
    int m_field90;                      // +0x90
    int m_field94;                      // +0x94
    int m_today;                        // +0x98
    int m_field9c;                      // +0x9c
    int m_renewCnt;                     // +0xa0
    DirectxVersionStruct m_directx;     // +0xa4
    UsedMemoryStruct m_memory[6];       // +0xc4
    int m_field1e4;                     // +0x1e4
};

#endif // FRAME_LAG_COLLECTOR_H_
