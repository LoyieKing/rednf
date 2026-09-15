#ifndef FRAME_LAG_COLLECTOR_H_
#define FRAME_LAG_COLLECTOR_H_

#include <map>

#include "PacketHeader.h"

class CServerHandler;
// [2026-09-15 机理 E] 原为空类（仅前向声明）⇒ .cpp 只能用 FrameLagAccItem 视图 cast，
//   而 cast 会强制 base 先载入（ORIG 为 index 先、base 末尾 add 融入）。
//   逐项镜像视图：+0..+6 四个 short，+0x8 起 6 个 {int,float}（步长 8）⇒ sizeof = 0x38
//   （与 ORIG `pkt + i*0x38` 的步长一致）。
class FrameLagStruct
{
public:
    struct Sub
    {
        int m_i;     // +0x00
        float m_f;   // +0x04
    } __attribute__((packed));

    short m_v0;      // +0x00
    short m_v1;      // +0x02
    short m_v2;      // +0x04
    short m_v3;      // +0x06
    Sub m_sub[6];    // +0x08，步长 8
} __attribute__((packed));
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
    unsigned char m_fieldA;  // +0xa
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
        unsigned int m_counts[6];   // +0（ORIG 除法为 divl：字段为 unsigned）
        unsigned int m_sums[6];     // +0x18
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
        int m0;              // +0x00
        int m_a[6];          // +0x04..0x1b
        short m_b[8];        // +0x1c..0x2b
        int m_c[3];          // +0x2c
        int m_d[6];          // +0x38
        int m_e[6];          // +0x50
        int m_f[6];          // +0x68
        int m_g[6];          // +0x80
        int m_h[6][4];       // +0x98
        union Pair
        {
            struct
            {
                int m_i;         // +0
                float m_f;       // +4
            } s;
            int m_words[2];      // 用于按 ORIG 的 int 位模式读写
        };
        // ORIG 的 m_i 基址 = 0x100、容量 41 个 Pair（0x100 + 41*8 = 0x248 = ORIG 的
        //   rep movsl 146-ints 锚定值）。两侧不可兼得：要复现 ORIG 的**二维展开**
        //   （内层 j 走 %ebx）就必须让类型真为二维，而 41 是质数无法分解 ⇒ 实测取舍：
        //   二维 [6][7] 从 0xf8（本声明，init 2 / Save 63 / acc 186）
        //   vs 一维 [41] 从 0x100（init 49 / Save 188 / acc 285）
        //   vs 二维视图 cast（init 25 / Save 74 / acc 318）⇒ 均劣于基线。
        //   注：+0x100 的同址 disp 编码（ORIG `add $0x1e`+disp 0x10）在可得后端下
        //   一律折成 `add/lea $0x20`+disp 0x0（4.4.6-3 / 4.4.4-13 / 4.4.7-23）
        //   或 disp 0x100 不拆（4.1.2-52）⇒ 属编译器常量拆分轴。
        //   [2026-09-15 StaticsDiff 复测] 在 0xf8 补 8 字节成员使 m_i 落到 0x100：
        //   init 2→4、acc 186→198、服务 identical 1142→1140（11 非 identical）⇒ **劣化，已回退**。
        Pair m_i[6][7];      // +0xf8（类大小保持 0x248）
    };
    class MonitoringSpecCase
    {
    public:
        int   m_specId;          // +0x00（monitoring_spec.spec_id）
        char  m_cpuVendor;       // +0x04（monitoring_spec.cpu_vendor）
        char  m_cpuProcessorNum; // +0x05（monitoring_spec.cpu_processor_num）
        int   m_aboveCpuClock;   // +0x08（monitoring_spec.above_cpu_clock）
        int   m_belowCpuClock;   // +0x0c（monitoring_spec.below_cpu_clock）
        unsigned short m_ram;    // +0x10（monitoring_spec.ram）
        int   m_videocardVendor; // +0x14（monitoring_spec.videocard_vendor）
        int   m_videocardDevice; // +0x18（monitoring_spec.videocard_device）
        unsigned short m_videocardTextureMem; // +0x1c（monitoring_spec.videocard_texture_mem）
        char  m_osVersion;       // +0x1e（monitoring_spec.os_version）
    };

    FrameLagCollector();
    ~FrameLagCollector();
    int ReLoadSpec(CServerHandler* handler);
    void RenewToday();
    int SaveUsedMemory(CServerHandler* handler);
    int SaveDailyBadSpec(CServerHandler* handler);
    int SaveFrameLagData(CServerHandler* handler);
    int GetCollectInterval();
    int PushOneFrameLagData(Packet_Frame_Lag_Statistic_Add* pkt);
    int CollectIntervalCheck(Packet_Frame_Lag_Collect_Interval_Check* pkt);
    int PopMonitoringSpecData(Packet_Frame_Lag_Spec_Delete_Notify* pkt);
    int PushMonitoringSpecData(Packet_Frame_Lag_Statistic_Result_Load_Spec* pkt);
    int PushMonitoringSpecData(Packet_Frame_Lag_Statistic_Result_Reload_Spec* pkt);
    bool is_valid_statistic_packet(Packet_Frame_Lag_Statistic_Add* pkt);
    int SaveCollectedDirectxVersion(CServerHandler* handler);
    bool Init();
    int LoadSpec(CServerHandler* handler);
    void accFrameLagStruct(FrameLagDataStruct& data, FrameLagStruct* pkt);

    unsigned char m_field0;             // +0
    int m_field4;                       // +4
    int m_field8;                       // +8
    int m_fieldc;                       // +0xc
    int m_field10;                      // +0x10
    int m_field14;                      // +0x14
    unsigned char m_field18;            // +0x18（ORIG 以 movzbl 载入后按无符号 byte 比较）
    unsigned char m_field19;            // +0x19
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
