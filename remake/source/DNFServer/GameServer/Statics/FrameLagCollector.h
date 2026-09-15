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
        unsigned short m_b[8]; // +0x1c..0x2b  // [2026-09-15 机理 B/类型] ORIG `m_b[idx] += 1`
        //   生成 `lea 0x1(%edx),%ecx`（unsigned short ⇒ +1 结果必须新建寄存器再截断），
        //   signed short 则折叠成 `add $0x1,%edx`（30 形微实验 v3：唯 unsigned 复现 lea）。
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
        // ORIG 的 m_i 有效区 = 0x100..0x248（41 格；ORIG 类大小 0x248 由
        //   `rep movsl 146`（=0x248）与 `_Rb_tree_node` 的 new 尺寸 0x25c
        //   （16 hdr + 4 key + 0x248）双重锚定），而 [6][7] 声明只能从 0xf8 起
        //   （42 格，类大小同为 0x248）⇒ 我方 m_i 访问的 disp 恒比 ORIG 少 8。
        //   [2026-09-15 W10-Static 穷举（含隔离探针，全部实测）] 该轴不可调：
        //     · 起点改 0x100（pad8[2] + [41] 一维）⇒ 一维索引失去 %ebx 承载 j，
        //       init 2→49、acc 17→239 ✗（探针 B1/G2 复现）
        //     · 起点改 0x100（pad8[2] + [6][7] 二维）⇒ GCC 折成 `add $32`+disp 0，
        //       与 ORIG 的 `add $30`+disp 0x10 仍不同（探针 C）
        //     · 嵌套子对象（f8 + 8）可复现 ORIG 的 30格+16 拆分且保留 %ebx，
        //       但 [6][7] 使类大小变 0x250 ⇒ map 节点 0x264、rep 148 及连带函数
        //       劣化（历史服务 6→11 件）✗
        //     · 索引 +1 格（m_i[k][j+1] / 一维 [42] k*7+j+1）⇒ 多一条 `lea 0x1` ✗
        //   结论：二维声明（42 格）与起点 0x100（41 格）在 sizeof 固定下互斥。
        Pair m_i[6][7];      // +0xf8（ORIG 有效区从 0x100 起 ⇒ disp 差 8）
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
