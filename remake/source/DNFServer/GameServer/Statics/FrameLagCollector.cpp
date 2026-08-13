// df_statics_r — FrameLagCollector（ORIG FrameLagCollector.o 拆分）
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "FrameLagCollector.h"
#include "DNFServerHandler.h"
#include "DNFTableBase.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "UdpCharacteristic.h"
#include "Packet_Frame_Lag_Statistic_Add.h"

struct PairView { int m_key; int m_f0; char m_rest[0x1c]; };
struct PktView { char m_pad[0xa]; int m_a; } __attribute__((packed));
struct FrameLagPktHeader
{
    char m_pad0[0xa];           // +0x00 PacketHeader
    int m_min;                  // +0x0a
    char m_field_e;             // +0x0e
    char m_field_f;             // +0x0f
    char m_pad10[0x2];          // +0x10
    unsigned short m_field_12;  // +0x12
    unsigned short m_field_14;  // +0x14
    unsigned short m_field_16;  // +0x16
    unsigned short m_field_18;  // +0x18
    char m_field_1a;            // +0x1a
    unsigned int m_directx;     // +0x1b
    char m_module;              // +0x1f
    short m_sum1;               // +0x20
    short m_sum2;               // +0x22
} __attribute__((packed));

// FrameLag spec 包（Load/Reload）命名成员视图：每个视图只含一个数组，
// 数组基址对齐 ORIG 反汇编的 displacement（mov 0xNN(%reg,%idx,N)），
// 使 GCC -O0 生成与 ORIG 相同的折叠位移形态。
struct FrameLagSpecInts3  { char m_pad[3];  int m[0x100]; } __attribute__((packed)); // +0x03
struct FrameLagSpecInts7  { char m_pad[7];  int m[0x100]; } __attribute__((packed)); // +0x07
struct FrameLagSpecIntsB  { char m_pad[0xb]; int m[0x100]; } __attribute__((packed)); // +0x0b
struct FrameLagSpecIntsF  { char m_pad[0xf]; int m[0x100]; } __attribute__((packed)); // +0x0f
struct FrameLagSpecShorts3 { char m_pad[3]; short m[0x100]; } __attribute__((packed)); // +0x03
struct FrameLagSpecShorts7 { char m_pad[7]; short m[0x100]; } __attribute__((packed)); // +0x07
struct FrameLagSpecView
{
    char m_hdr[0xa];    // +0x00 PacketHeader
    char m_fieldA;      // +0x0a
    char m_padb[0x50];  // +0x0b..0x5a
    char m_field4[6];   // +0x5b
    char m_field5[6];   // +0x61
    char m_pad67[0x78]; // +0x67..0xde
    char m_field1e[6];  // +0xdf
} __attribute__((packed));

// FrameLag Statistic_Add 包：每 0x38 字节一个 item，v0..v3 位于 item 内 +0x24..0x2a；
// 用 (char*)pkt + i*0x38 + 0x20 的基址 + item 内 +4..0xa 成员，复现 ORIG 的
// add $0x20 + movzwl 0x4(%eax) 拆分形态。
struct FrameLagAddItem
{
    char m_pad[4];      // +0x00
    short m_v0;         // +0x04
    short m_v1;         // +0x06
    short m_v2;         // +0x08
    short m_v3;         // +0x0a
    char m_rest[0x2e];  // 补齐至 0x38
} __attribute__((packed));

// FrameLag Statistic_Add 包：+0xc 起 8 字节步长的子项（int + float），
// 复现 ORIG 的 mov 0xc(%ecx,%eax,8) / mov 0x10(%ecx,%eax,8) 形态。
struct FrameLagAddSub
{
    char m_pad[0xc];       // +0x00
    struct FrameLagAddSubItem { int m_i; float m_f; } m_sub[0x100];  // +0x0c，步长 8
} __attribute__((packed));

// FrameLag Statistic_Add 包尾：+0x174 起 char[6] + short[6]（used memory 上报），
// 复现 ORIG 的 movzbl 0x174(%edx,%eax,1) 与 add $0xb8 + movzwl 0xa(%eax,%edx,2)。
struct FrameLagMemView
{
    char m_pad[0x174];  // +0x00
    char m_used[6];     // +0x174
    short m_val[6];     // +0x17a
} __attribute__((packed));

// accFrameLagStruct 的 FrameLag item：+0..0x6 四个 short，+0x8 起 8 字节步长的
// int+float 子项（与 is_valid_statistic_packet 的 +0x20/+0xc 视图不同，按 ORIG 反汇编单独建模）。
struct FrameLagAccItem
{
    short m_v0;         // +0x00
    short m_v1;         // +0x02
    short m_v2;         // +0x04
    short m_v3;         // +0x06
    struct Sub { int m_i; float m_f; } m_sub[6];  // +0x08，步长 8
    char m_rest[0x28];  // 补齐至 0x38
} __attribute__((packed));

// FrameLagDataStruct 的命名成员视图（与 ORIG 反汇编逐槽位核对）
struct FrameLagDataLayout
{
    int m0;          // +0x00
    int m_a[2];      // +0x04 (view; accessed up to [5])
    short m_b[16];   // +0x0c
    int m_c[3];      // +0x2c
    int m_d[6];      // +0x38
    int m_e[6];      // +0x50
    int m_f[6];      // +0x68
    int m_g[6];      // +0x80
    int m_h[6][4];   // +0x98
    int m_pad[2];    // +0xf8
    int m_i[41][2];  // +0x100
};

int FrameLagCollector::GetCollectInterval()
{
    return m_collectInterval;
}
FrameLagCollector::FrameLagCollector()
    : m_field0(0), m_field4(0), m_field8(0), m_fieldc(0), m_field10(0), m_field14(0),
      m_field18(0), m_field19(0), m_map1c(), m_map34(), m_field4c(0), m_field50(0),
      m_monitor(), m_field6c(0), m_data(), m_collectInterval(0x1e)
{
    m_map1c.clear();
    m_map34.clear();
    m_monitor.clear();
    m_data.clear();
    m_field8c = 0;
    m_field90 = 0;
    m_field94 = 0;
    Init();
    m_directx.init();
    m_renewCnt = 0;
    m_field9c = m_today;
    for (int i = 0; i < 6; i++)
    {
        m_memory[i].init();
    }
    m_field1e4 = 0;
}
FrameLagCollector::~FrameLagCollector()
{
    m_data.clear();
    m_monitor.clear();
    m_map34.clear();
    m_map1c.clear();
}
bool FrameLagCollector::Init()
{
    if (m_field0 != 0)
    {
        return 1;
    }
    m_field0 = 1;
    time_t now = time(0);
    tm* pt = localtime(&now);
    m_today = pt->tm_mday;
    return 0;
}
void FrameLagCollector::RenewToday()
{
    for (;;)
    {
        m_renewCnt++;
        if (9 < m_renewCnt)
        {
            m_renewCnt = 0;
            time_t now = time(0);
            tm* pt = localtime(&now);
            m_today = pt->tm_mday;
        }
        break;
    }
}
int FrameLagCollector::LoadSpec(CServerHandler* handler)
{
    if (m_field4 != 2)
    {
        char needLoad = 0;
        if (m_field4 == 0)
        {
            needLoad = 1;
        }
        else if (m_field4 == 1)
        {
            m_field8++;
            if (m_field8 == 0x3c)
            {
                m_field8 = 0;
                needLoad = 1;
            }
        }
        if (needLoad != 0)
        {
            m_field4 = 1;
            m_field18++;
            if (m_field18 == 0x7f)
            {
                m_field18 = 1;
            }
            Packet_Frame_Lag_Statistic_Load_Spec pkt;
            pkt.m_fieldA = m_field18;
            m_map1c.clear();
            handler->SendToDB((PacketHeader*)&pkt);
        }
    }
    return 0;
}
int FrameLagCollector::ReLoadSpec(CServerHandler* handler)
{
    if (m_field4 != 2)
    {
        return 2;
    }
    m_field14++;
    if (m_field14 == 5)
    {
        m_field14 = 0;
        m_field19++;
        if (m_field19 == 0x7f)
        {
            m_field19 = 1;
        }
        m_map34.clear();
        Packet_Frame_Lag_Statistic_Reload_Spec pkt;
        pkt.m_fieldA = m_field19;
        pkt.m_fieldB = m_field4c;
        handler->SendToDB((PacketHeader*)&pkt);
    }
    return 0;
}
int FrameLagCollector::PushOneFrameLagData(Packet_Frame_Lag_Statistic_Add* pkt)
{
    if (m_field4 != 2)
    {
        return 2;
    }
    if (is_valid_statistic_packet(pkt) != 1)
    {
        return 4;
    }
    m_field6c++;
    m_directx.add_cnt(((FrameLagPktHeader*)pkt)->m_directx);
    for (int i = 0; i < 6; i++)
    {
        m_memory[i].SetUsedMemory((char)((FrameLagMemView*)pkt)->m_used[i],
                                  ((FrameLagMemView*)pkt)->m_val[i]);
    }
    for (std::map<int, MonitoringSpecCase>::iterator it = m_monitor.begin();
         it != m_monitor.end(); ++it)
    {
        char match = 1;
        if (!(it->second.m_cpuVendor == -1 || it->second.m_cpuVendor == ((FrameLagPktHeader*)pkt)->m_field_f))
            match = 0;
        if (match && !(it->second.m_cpuProcessorNum == -1 || it->second.m_cpuProcessorNum == ((FrameLagPktHeader*)pkt)->m_field_e))
            match = 0;
        if (match && !(it->second.m_aboveCpuClock == -1 || it->second.m_aboveCpuClock <= ((FrameLagPktHeader*)pkt)->m_min))
            match = 0;
        if (match && !(it->second.m_belowCpuClock == -1 || it->second.m_belowCpuClock > ((FrameLagPktHeader*)pkt)->m_min))
            match = 0;
        if (match && !(it->second.m_ram == -1 || it->second.m_ram == ((FrameLagPktHeader*)pkt)->m_field_18))
            match = 0;
        if (match && !(it->second.m_videocardVendor == -1 || (unsigned int)it->second.m_videocardVendor == (unsigned int)((FrameLagPktHeader*)pkt)->m_field_12))
            match = 0;
        if (match && !(it->second.m_videocardDevice == -1 || (unsigned int)it->second.m_videocardDevice == (unsigned int)((FrameLagPktHeader*)pkt)->m_field_14))
            match = 0;
        if (match && !(it->second.m_videocardTextureMem == -1 || it->second.m_videocardTextureMem == ((FrameLagPktHeader*)pkt)->m_field_16))
            match = 0;
        if (match && !(it->second.m_osVersion == -1 || it->second.m_osVersion == ((FrameLagPktHeader*)pkt)->m_field_1a))
            match = 0;
        if (match)
        {
            std::map<int, FrameLagDataStruct>::iterator fd = m_data.find(it->first);
            if (fd != m_data.end())
            {
                FrameLagDataStruct* v = &fd->second;
                v->m_data[0] = v->m_data[0] + 1;
                if (-1 < (char)((FrameLagPktHeader*)pkt)->m_module &&
                    (char)((FrameLagPktHeader*)pkt)->m_module < 8)
                {
                    ((FrameLagDataLayout*)v)->m_b[((char)((FrameLagPktHeader*)pkt)->m_module + 8)] += 1;
                }
                if (-1 < ((FrameLagPktHeader*)pkt)->m_sum1)
                {
                    ((FrameLagDataLayout*)v)->m_c[1] += (int)((FrameLagPktHeader*)pkt)->m_sum1;
                    ((FrameLagDataLayout*)v)->m_c[2] += (int)((FrameLagPktHeader*)pkt)->m_sum2;
                    ((FrameLagDataLayout*)v)->m_c[0] += 1;
                }
                accFrameLagStruct(*v, (FrameLagStruct*)((char*)pkt + 0x24));
            }
        }
    }
    return 0;
}
int FrameLagCollector::PopMonitoringSpecData(Packet_Frame_Lag_Spec_Delete_Notify* pkt)
{
    std::map<int, MonitoringSpecCase>::iterator it;
    bool again = true;
    bool erased = false;
    while (again)
    {
        again = false;
        it = m_monitor.begin();
        for (; it != m_monitor.end(); ++it)
        {
            if (((PairView*)it.operator->())->m_f0 == ((PktView*)pkt)->m_a)
            {
                m_monitor.erase(it);
                again = true;
                erased = true;
            }
        }
    }
    if (erased)
    {
        // ORIG 以 lea 0xa(%eax) 直取 key 地址；packed int 成员取址会被物化，保持裸形态
        m_data.erase(*(int*)((char*)pkt + 10));
    }
    return 0;
}
int FrameLagCollector::PushMonitoringSpecData(Packet_Frame_Lag_Statistic_Result_Load_Spec* pkt)
{
    if (m_field4 != 1)
    {
        return 3;
    }
    if (m_field18 != ((FrameLagSpecView*)pkt)->m_fieldA)
    {
        return 0;
    }
    int specId = ((FrameLagSpecIntsB*)pkt)->m[0];
    if (m_map1c.find(specId) == m_map1c.end())
    {
        return 0;
    }
    m_map1c[specId] = 1;
    for (int i = 0; i <= 5; i++)
    {
        int sid = ((FrameLagSpecInts3*)pkt)->m[i + 4];
        if (sid == -1)
        {
            break;
        }
        int ts = ((FrameLagSpecIntsB*)pkt)->m[i + 8];
        if (m_field4c < ts)
        {
            m_field4c = ts;
        }
        MonitoringSpecCase mc;
        mc.m_specId = ((FrameLagSpecInts3*)pkt)->m[i + 0x10];
        mc.m_cpuVendor = ((FrameLagSpecView*)pkt)->m_field4[i];
        mc.m_cpuProcessorNum = ((FrameLagSpecView*)pkt)->m_field5[i];
        mc.m_aboveCpuClock = ((FrameLagSpecInts7*)pkt)->m[i + 0x18];
        mc.m_belowCpuClock = ((FrameLagSpecIntsF*)pkt)->m[i + 0x1c];
        mc.m_ram = ((FrameLagSpecShorts7*)pkt)->m[i + 0x48];
        mc.m_videocardVendor = ((FrameLagSpecInts3*)pkt)->m[i + 0x28];
        mc.m_videocardDevice = ((FrameLagSpecIntsB*)pkt)->m[i + 0x2c];
        mc.m_videocardTextureMem = ((FrameLagSpecShorts3*)pkt)->m[i + 0x68];
        mc.m_osVersion = ((FrameLagSpecView*)pkt)->m_field1e[i];
        m_monitor[sid] = mc;
        FrameLagDataStruct fd;
        m_data[mc.m_specId] = fd;
    }
    if ((int)m_map1c.size() == ((FrameLagSpecIntsF*)pkt)->m[0])
    {
        m_field4 = 2;
        puts("============FirstSpecLoad Complete!!!==========");
    }
    return 0;
}
int FrameLagCollector::PushMonitoringSpecData(Packet_Frame_Lag_Statistic_Result_Reload_Spec* pkt)
{
    if (m_field4 != 2)
    {
        return 2;
    }
    if (m_field19 != ((FrameLagSpecView*)pkt)->m_fieldA)
    {
        return 0;
    }
    int specId = ((FrameLagSpecIntsB*)pkt)->m[0];
    if (m_map34.find(specId) == m_map34.end())
    {
        return 0;
    }
    m_map34[specId] = 1;
    for (int i = 0; i <= 5; i++)
    {
        int sid = ((FrameLagSpecInts3*)pkt)->m[i + 4];
        if (sid == -1)
        {
            break;
        }
        int ts = ((FrameLagSpecIntsB*)pkt)->m[i + 8];
        if (m_field50 < ts)
        {
            m_field50 = ts;
        }
        std::map<int, MonitoringSpecCase>::iterator it = m_monitor.find(sid);
        if (it == m_monitor.end())
        {
            MonitoringSpecCase mc;
            mc.m_specId = ((FrameLagSpecInts3*)pkt)->m[i + 0x10];
            mc.m_cpuVendor = ((FrameLagSpecView*)pkt)->m_field4[i];
            mc.m_cpuProcessorNum = ((FrameLagSpecView*)pkt)->m_field5[i];
            mc.m_aboveCpuClock = ((FrameLagSpecInts7*)pkt)->m[i + 0x18];
            mc.m_belowCpuClock = ((FrameLagSpecIntsF*)pkt)->m[i + 0x1c];
            mc.m_ram = ((FrameLagSpecShorts7*)pkt)->m[i + 0x48];
            mc.m_videocardVendor = ((FrameLagSpecInts3*)pkt)->m[i + 0x28];
            mc.m_videocardDevice = ((FrameLagSpecIntsB*)pkt)->m[i + 0x2c];
            mc.m_videocardTextureMem = ((FrameLagSpecShorts3*)pkt)->m[i + 0x68];
            mc.m_osVersion = ((FrameLagSpecView*)pkt)->m_field1e[i];
            m_monitor[sid] = mc;
        }
        else
        {
            it->second.m_specId = ((FrameLagSpecInts3*)pkt)->m[i + 0x10];
            it->second.m_cpuVendor = ((FrameLagSpecView*)pkt)->m_field4[i];
            it->second.m_cpuProcessorNum = ((FrameLagSpecView*)pkt)->m_field5[i];
            it->second.m_aboveCpuClock = ((FrameLagSpecInts7*)pkt)->m[i + 0x18];
            it->second.m_belowCpuClock = ((FrameLagSpecIntsF*)pkt)->m[i + 0x1c];
            it->second.m_ram = ((FrameLagSpecShorts7*)pkt)->m[i + 0x48];
            it->second.m_videocardVendor = ((FrameLagSpecInts3*)pkt)->m[i + 0x28];
            it->second.m_videocardDevice = ((FrameLagSpecIntsB*)pkt)->m[i + 0x2c];
            it->second.m_videocardTextureMem = ((FrameLagSpecShorts3*)pkt)->m[i + 0x68];
            it->second.m_osVersion = ((FrameLagSpecView*)pkt)->m_field1e[i];
        }
        FrameLagDataStruct fd;
        m_data[((FrameLagSpecInts3*)pkt)->m[i + 0x10]] = fd;
    }
    if ((int)m_map34.size() == ((FrameLagSpecIntsF*)pkt)->m[0])
    {
        if (m_field4c < m_field50)
        {
            m_field4c = m_field50;
        }
        puts("============ReloadSpec Complete!!!==========");
    }
    return 0;
}
int FrameLagCollector::CollectIntervalCheck(Packet_Frame_Lag_Collect_Interval_Check* pkt)
{
    if (pkt->m_fieldA != 0)
    {
        m_collectInterval = (short)pkt->m_fieldA;
    }
    else
    {
        m_collectInterval = 0x1e;
    }
    printf("FrameLagCollector::CollectIntervalCheck(), Interval check : %d", m_collectInterval);
    return 0;
}
int FrameLagCollector::SaveDailyBadSpec(CServerHandler* handler)
{
    return 0;
}
int FrameLagCollector::SaveFrameLagData(CServerHandler* handler)
{
    if (m_field4 != 2)
    {
        return 2;
    }
    m_field10++;
    if (!(m_collectInterval <= m_field10))
    {
        return 0;
    }
    m_field10 = 0;
    if (m_field6c == 0)
    {
        return 0;
    }
    Packet_Frame_Lag_Statistic_Write_Lag_Index pkt;
    char* pb = (char*)&pkt;
    pb[10] = (char)handler->GetServerGroupNo();
    // ORIG：time(&t) 后再拷贝到 now（now 实际未用于报文，仅为复现 ORIG 代码形态）
    time_t t;
    time(&t);
    time_t now = t;
    for (std::map<int, FrameLagDataStruct>::iterator it = m_data.begin();
         it != m_data.end(); ++it)
    {
        // ORIG 对 map value 的读写全部以 pair 基址（it.operator->()）为基准：
        // value 在 pair+4。偏移表达式按 ORIG 反汇编逐槽位核对过。
        char* v = (char*)it.operator->();
        *(int*)(pb + 0xb) = it->first;
        *(int*)(pb + 0xf) = *(int*)((char*)v + 4);
        for (int i = 0; i < 8; i++)
        {
            *(short*)(pb + 0x13 + i * 2) = *(short*)((char*)v + 0x10 + (i + 8) * 2);
        }
        if (*(int*)((char*)v + 0x30) == 0)
        {
            *(short*)(pb + 0x23) = -1;
            *(short*)(pb + 0x25) = -1;
        }
        else
        {
            *(short*)(pb + 0x23) = (short)((double)(*(int*)((char*)v + 0x34)) /
                                           (double)(*(unsigned int*)((char*)v + 0x30)) + 0.5);
            *(short*)(pb + 0x25) = (short)((double)(*(int*)((char*)v + 0x38)) /
                                           (double)(*(unsigned int*)((char*)v + 0x30)) + 0.5);
        }
        for (int i = 0; i < 6; i++)
        {
            *(short*)(pb + 0x27 + i * 0x38 + 0) = 0;
            *(short*)(pb + 0x27 + i * 0x38 + 2) = 0;
            *(short*)(pb + 0x27 + i * 0x38 + 4) = 0;
            *(short*)(pb + 0x27 + i * 0x38 + 6) = 0;
            if (0 < *(int*)((char*)v + (i + 9) * 0x10 + 0xc))
            {
                *(short*)(pb + 0x27 + i * 0x38 + 0) =
                    (short)(*(int*)((char*)v + 0xc + (i + 0xc) * 4) /
                            *(int*)((char*)v + (i + 9) * 0x10 + 0xc));
            }
            if (0 < *(int*)((char*)v + i * 0x10 + 0xa0))
            {
                *(short*)(pb + 0x27 + i * 0x38 + 2) =
                    (short)(*(int*)((char*)v + 4 + (i + 0x14) * 4) /
                            *(int*)((char*)v + i * 0x10 + 0xa0));
            }
            if (0 < *(int*)((char*)v + i * 0x10 + 0xa4))
            {
                *(short*)(pb + 0x27 + i * 0x38 + 4) =
                    (short)(*(int*)((char*)v + 0xc + (i + 0x18) * 4) /
                            *(int*)((char*)v + i * 0x10 + 0xa4));
            }
            if (0 < *(int*)((char*)v + i * 0x10 + 0xa8))
            {
                *(short*)(pb + 0x27 + i * 0x38 + 6) =
                    (short)(*(int*)((char*)v + 4 + (i + 0x20) * 4) /
                            *(int*)((char*)v + i * 0x10 + 0xa8));
            }
            for (int j = 0; j < 6; j++)
            {
                *(int*)(pb + 0xf + (i * 7 + j + 4) * 8) =
                    *(int*)((char*)v + 0x14 + (i * 7 + j + 0x1e) * 8);
                *(int*)(pb + 0x13 + (i * 7 + j + 4) * 8) =
                    *(int*)((char*)v + 0x18 + (i * 7 + j + 0x1e) * 8);
            }
        }
        handler->SendToDB((PacketHeader*)&pkt);
        ((FrameLagDataStruct*)((char*)v + 4))->init();
        DNFFLib::Sleep_Ext(0, 1);
    }
    m_field6c = 0;
    time_t t2 = time(0);
    if (t2 < m_field8c || m_field90 < t2)
    {
        m_collectInterval = 0x1e;
    }
}
int FrameLagCollector::SaveCollectedDirectxVersion(CServerHandler* handler)
{
    if (m_field4 != 2)
    {
        return 2;
    }
    if (m_field9c == m_today)
    {
        return 0;
    }
    m_field9c = m_today;
    Packet_Frame_Lag_Statistic_Write_Query pkt;
    time_t now;
    time(&now);
    snprintf((char*)&pkt + 10, 0x400,
             "inSert into directx_version(occ_time,server_group,ver_etc,ver_8_x,ver_9_0,ver_9_0_a,ver_9_0_b,ver_9_0_c,ver_10_x,ver_11_x) values(from_unixtime(%d),%hhd,%u,%u,%u,%u,%u,%u,%u,%u)",
             (int)now, (signed char)handler->GetServerGroupNo(),
             (unsigned int)m_directx.m_data[0], (unsigned int)m_directx.m_data[1],
             (unsigned int)m_directx.m_data[2], (unsigned int)m_directx.m_data[3],
             (unsigned int)m_directx.m_data[4], (unsigned int)m_directx.m_data[5],
             (unsigned int)m_directx.m_data[6], (unsigned int)m_directx.m_data[7]);
    handler->SendToDB((PacketHeader*)&pkt);
    m_directx.init();
    return 0;
}
int FrameLagCollector::is_valid_statistic_packet(Packet_Frame_Lag_Statistic_Add* pkt)
{
    if ((char)((FrameLagPktHeader*)pkt)->m_module < 0 ||
        8 < (char)((FrameLagPktHeader*)pkt)->m_module)
    {
        return 0;
    }
    for (int i = 0; i < 6; i++)
    {
        if (((FrameLagAddItem*)((char*)pkt + i * 0x38 + 0x20))->m_v0 < 0) return 0;
        if (((FrameLagAddItem*)((char*)pkt + i * 0x38 + 0x20))->m_v1 < 0) return 0;
        if (((FrameLagAddItem*)((char*)pkt + i * 0x38 + 0x20))->m_v2 < 0) return 0;
        if (((FrameLagAddItem*)((char*)pkt + i * 0x38 + 0x20))->m_v3 < 0) return 0;
        for (int j = 0; j < 6; j++)
        {
            if (99999 < ((FrameLagAddSub*)pkt)->m_sub[i * 7 + j + 4].m_i) return 0;
            if (((FrameLagAddSub*)pkt)->m_sub[i * 7 + j + 4].m_f == 0.0 &&
                0 < ((FrameLagAddSub*)pkt)->m_sub[i * 7 + j + 4].m_i) return 0;
        }
    }
    return 1;
}
void FrameLagCollector::accFrameLagStruct(FrameLagDataStruct& data, FrameLagStruct* pkt)
{
    for (int i = 0; i < 6; i++)
    {
        if (0 < ((FrameLagAccItem*)((char*)pkt + i * 0x38))->m_v0)
        {
            ((FrameLagDataLayout*)&data)->m_d[i] +=
                (int)((FrameLagAccItem*)((char*)pkt + i * 0x38))->m_v0;
            ((FrameLagDataLayout*)&data)->m_h[i][0] += 1;
        }
        if (0 < ((FrameLagAccItem*)((char*)pkt + i * 0x38))->m_v1)
        {
            ((FrameLagDataLayout*)&data)->m_e[i] +=
                (int)((FrameLagAccItem*)((char*)pkt + i * 0x38))->m_v1;
            ((FrameLagDataLayout*)&data)->m_h[i][1] += 1;
        }
        if (0 < ((FrameLagAccItem*)((char*)pkt + i * 0x38))->m_v2)
        {
            ((FrameLagDataLayout*)&data)->m_f[i] +=
                (int)((FrameLagAccItem*)((char*)pkt + i * 0x38))->m_v2;
            ((FrameLagDataLayout*)&data)->m_h[i][2] += 1;
        }
        if (0 < ((FrameLagAccItem*)((char*)pkt + i * 0x38))->m_v3)
        {
            ((FrameLagDataLayout*)&data)->m_g[i] +=
                (int)((FrameLagAccItem*)((char*)pkt + i * 0x38))->m_v3;
            ((FrameLagDataLayout*)&data)->m_h[i][3] += 1;
        }
        for (int j = 0; j < 6; j++)
        {
            ((FrameLagDataLayout*)&data)->m_i[i * 7 + j][0] =
                ((FrameLagDataLayout*)&data)->m_a[i] *
                ((FrameLagDataLayout*)&data)->m_i[i * 7 + j][0];
            ((FrameLagDataLayout*)&data)->m_i[i * 7 + j][1] =
                (float)(unsigned int)((FrameLagDataLayout*)&data)->m_a[i] *
                ((FrameLagDataLayout*)&data)->m_i[i * 7 + j][1];
            ((FrameLagDataLayout*)&data)->m_i[i * 7 + j][0] +=
                ((FrameLagAccItem*)((char*)pkt + i * 0x38))->m_sub[j].m_i;
            ((FrameLagDataLayout*)&data)->m_i[i * 7 + j][1] +=
                ((FrameLagAccItem*)((char*)pkt + i * 0x38))->m_sub[j].m_f;
            ((FrameLagDataLayout*)&data)->m_i[i * 7 + j][0] /=
                ((FrameLagDataLayout*)&data)->m_a[i] + 1U;
            ((FrameLagDataLayout*)&data)->m_i[i * 7 + j][1] /=
                (float)(((FrameLagDataLayout*)&data)->m_a[i] + 1);
        }
        ((FrameLagDataLayout*)&data)->m_a[i] += 1;
    }
}
void FrameLagCollector::SaveUsedMemory(CServerHandler* handler)
{
    m_field1e4++;
    if (m_collectInterval <= m_field1e4)
    {
        m_field1e4 = 0;
        Packet_Frame_Lag_Used_Memory_Write_Query pkt;
        for (int i = 0; i < 6; i++)
        {
            for (int j = 0; j < 6; j++)
            {
                int count = m_memory[i].m_counts[j];
                int sum = m_memory[i].m_sums[j];
                if (sum != 0)
                {
                    unsigned int avg = (unsigned int)sum / (unsigned int)count;
                    snprintf((char*)&pkt + 10, 0x400,
                             "inSert into used_memory (occ_time, minute_type, module, memory) values (now(),%d,%d,%d)",
                             i, j, avg);
                    handler->SendToDB((PacketHeader*)&pkt);
                }
            }
            m_memory[i].init();
        }
    }
}
Packet_Frame_Lag_Statistic_Load_Spec::Packet_Frame_Lag_Statistic_Load_Spec()
    : PacketHeader(0xc27, 0xb)
{
}
FrameLagCollector::FrameLagDataStruct::FrameLagDataStruct()
{
    init();
}
void FrameLagCollector::FrameLagDataStruct::init()
{
    ((FrameLagDataLayout*)this)->m0 = 0;
    for (int i = 0; i < 8; i++)
    {
        ((FrameLagDataLayout*)this)->m_b[i + 8] = 0;
    }
    ((FrameLagDataLayout*)this)->m_c[0] = 0;
    ((FrameLagDataLayout*)this)->m_c[1] = 0;
    ((FrameLagDataLayout*)this)->m_c[2] = 0;
    for (int i = 0; i < 6; i++)
    {
        ((FrameLagDataLayout*)this)->m_a[i] = 0;
        ((FrameLagDataLayout*)this)->m_d[i] = 0;
        ((FrameLagDataLayout*)this)->m_e[i] = 0;
        ((FrameLagDataLayout*)this)->m_f[i] = 0;
        ((FrameLagDataLayout*)this)->m_g[i] = 0;
        ((FrameLagDataLayout*)this)->m_h[i][0] = 0;
        ((FrameLagDataLayout*)this)->m_h[i][1] = 0;
        ((FrameLagDataLayout*)this)->m_h[i][2] = 0;
        ((FrameLagDataLayout*)this)->m_h[i][3] = 0;
        for (int j = 0; j < 6; j++)
        {
            ((FrameLagDataLayout*)this)->m_i[i * 7 + j][0] = 0;
            ((FrameLagDataLayout*)this)->m_i[i * 7 + j][1] = 0;
        }
    }
}
void FrameLagCollector::DirectxVersionStruct::init()
{
    memset(this, 0, 0x20);
}
void FrameLagCollector::DirectxVersionStruct::add_cnt(unsigned int version)
{
    if (version == 0xffffffff)
    {
        return;
    }
    if (version <= 0x7ffff)
    {
        m_data[0] += 1;
    }
    else if (version > 0x7ffff && version <= 0x8ffff)
    {
        m_data[1] += 1;
    }
    else if (version == 0x90000)
    {
        m_data[2] += 1;
    }
    else if (version == 0x90001)
    {
        m_data[3] += 1;
    }
    else if (version == 0x90002)
    {
        m_data[4] += 1;
    }
    else if (version == 0x90003)
    {
        m_data[5] += 1;
    }
    else if (version > 0x9ffff || version <= 0xaffff)
    {
        m_data[6] += 1;
    }
    else if (version > 0xaffff || version <= 0xbffff)
    {
        m_data[7] += 1;
    }
    else if (version > 0xbffff)
    {
        m_data[0] += 1;
    }
}
void FrameLagCollector::UsedMemoryStruct::init()
{
    memset(this, 0, 0x30);
}
void FrameLagCollector::UsedMemoryStruct::SetUsedMemory(char idx, short value)
{
    if (idx >= 6)
    {
        return;
    }
    m_sums[idx] += value;
    m_counts[idx]++;
}
