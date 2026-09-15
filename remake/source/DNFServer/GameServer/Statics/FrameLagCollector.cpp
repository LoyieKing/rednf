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
#include "Packet_Frame_Lag_Statistic_Result_Load_Spec.h"
#include "Packet_Frame_Lag_Statistic_Result_Reload_Spec.h"

// [2026-09-15 W5 机理 T-march / per-TU 目标特性] 本 TU 的 ORIG 是**按无 CMOV 目标**编译的：
//   全 ELF 指令统计 ⇒ ORIG `fucomip=0, fucompp=1` 而我方 `fucomip=1, fucompp=0`（同一处，
//   即 is_valid_statistic_packet 的 `m_f == 0.0` 比较）。探针（同 c6446r 工具链、19 种源码
//   写法 × 4 后端全部生成 fucomip）确认差异**不出自源码**，而出自 `-march`：
//     -march=i386/i486/i586 ⇒ `fucompp; fnstsw %ax; sahf`（= ORIG ✓）
//     -march=i686（默认）    ⇒ `fucomip %st(1),%st; fstp %st(0)`（= 我方）
//   因 `fucomip/fucomi` 需要 CMOV；本 pragma 等价 per-file `-march=i386`，只影响本 TU。
#pragma GCC target("arch=i386")

struct PairView { int m_key; int m_f0; char m_rest[0x1c]; };
struct PktView { char m_pad[0xa]; int m_a; } __attribute__((packed));

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

// Load/Reload Spec 的 m_modifyTime 在 ORIG 侧按 int 读取：ORIG 为
// `mov 0xb(%eax,%edx,4),%edx` 单段直落；共享头写 unsigned int 会多一条
// int←unsigned 转换中转（4.4 实测量：mov mem,%eax; mov %eax,%edx）。
struct FrameLagModifyTimeInt
{
    char m_pad[0x2b];
    int m_modifyTime[6];    // +0x2b
} __attribute__((packed));





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
// [DNF-NONIDENTICAL] DNF-STA-DIFF-0021 | statics | 与ORIG差异=DIFF | FrameLagCollector::PushOneFrameLagData | 详见 function_reports/statics/_ZN17FrameLagCollector19PushOneFrameLagDataEP30Packet_Frame_Lag_Statistic_Add.md
int FrameLagCollector::PushOneFrameLagData(Packet_Frame_Lag_Statistic_Add* pkt)
{
    // [2026-09-15 P372 实验] ORIG 为 C 风格函数体顶部前置声明：char 局部
    //   match 落 -0xd、循环变量 i 落 -0xc（我方原为 -0x9 / -0x10）。
    unsigned char match;
    int i;
    if (m_field4 != 2)
    {
        return 2;
    }
    if (!is_valid_statistic_packet(pkt))
    {
        return 4;
    }
    m_field6c++;
    m_directx.add_cnt(pkt->m_layout.u_main.m_directx);
    for (i = 0; i < 6; i++)
    {
        m_memory[i].SetUsedMemory((char)pkt->m_used[i],
                                  pkt->m_val[i]);
    }
    std::map<int, MonitoringSpecCase>::iterator it;
    std::map<int, FrameLagDataStruct>::iterator fd;
    it = m_monitor.begin();
    for (; it != m_monitor.end(); ++it)
    {
        match = 1;
        // [2026-09-15 载入形态] ORIG 为 `movzbl -0xd(%ebp),%eax; cmp $0x1,%eax`（388），
        //   `if (match == 1)` 被 GCC 折叠成 `cmpb`（我方 387 = 长度差 1 的唯一来源）。
        //   微实验（4.4.6 -m32 -O0）穷举 30 形（unsigned/signed char · bool · int · short ·
        //   volatile · 各类显式 cast 与提升写法）：唯 `switch (match) { case 1: … }` 复现
        //   movzbl+cmpl ⇒ 用 switch（单 case 不生成跳转表，长度差消除 387→388、ndiff 59→4）。
        switch (match)
        {
        case 1:
            if (!(it->second.m_cpuVendor == -1 ||
                  it->second.m_cpuVendor == pkt->m_layout.u_main.m_field_f))
            {
                match = 0;
                goto MATCH_DONE;
            }
            if (!(it->second.m_cpuProcessorNum == -1 ||
                  it->second.m_cpuProcessorNum == pkt->m_layout.u_main.m_field_e))
            {
                match = 0;
                goto MATCH_DONE;
            }
            if (!(it->second.m_aboveCpuClock == -1 ||
                  it->second.m_aboveCpuClock <= pkt->m_layout.u_main.m_min))
            {
                match = 0;
                goto MATCH_DONE;
            }
            if (!(it->second.m_belowCpuClock == -1 ||
                  it->second.m_belowCpuClock > pkt->m_layout.u_main.m_min))
            {
                match = 0;
                goto MATCH_DONE;
            }
            if (!(it->second.m_ram == (unsigned short)-1 ||
                  it->second.m_ram == pkt->m_layout.u_main.m_field_18))
            {
                match = 0;
                goto MATCH_DONE;
            }
            if (!(it->second.m_videocardVendor == -1 ||
                  it->second.m_videocardVendor ==
                      pkt->m_layout.u_main.m_field_12))
            {
                match = 0;
                goto MATCH_DONE;
            }
            if (!(it->second.m_videocardDevice == -1 ||
                  it->second.m_videocardDevice ==
                      pkt->m_layout.u_main.m_field_14))
            {
                match = 0;
                goto MATCH_DONE;
            }
            if (!(it->second.m_videocardTextureMem == (unsigned short)-1 ||
                  it->second.m_videocardTextureMem == pkt->m_layout.u_main.m_field_16))
            {
                match = 0;
                goto MATCH_DONE;
            }
            if (!(it->second.m_osVersion == -1 ||
                  it->second.m_osVersion == pkt->m_layout.u_main.m_field_1a))
            {
                match = 0;
                goto MATCH_DONE;
            }
            break;
        }
    MATCH_DONE:
        if (match)
        {
            fd = m_data.find(it->second.m_specId);
            if (fd != m_data.end())
            {
                fd->second.m0 += 1;
                if (-1 < (char)pkt->m_layout.u_main.m_module &&
                    (char)pkt->m_layout.u_main.m_module < 8)
                {
                    fd->second.m_b[((char)pkt->m_layout.u_main.m_module)] += 1;
                }
                if (-1 < pkt->m_layout.u_main.m_items[0].m_s0)
                {
                    fd->second.m_c[1] += (int)pkt->m_layout.u_main.m_items[0].m_s0;
                    fd->second.m_c[2] += (int)pkt->m_layout.u_main.m_items[0].m_s1;
                    fd->second.m_c[0] += 1;
                }
                accFrameLagStruct(fd->second, (FrameLagStruct*)((char*)pkt + 0x24));
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
// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-STA-DIFF-0022 已消除）
int FrameLagCollector::PushMonitoringSpecData(Packet_Frame_Lag_Statistic_Result_Load_Spec* pkt)
{
    if (m_field4 != 1)
    {
        return 3;
    }
    if (m_field18 == pkt->m_loadSeq)
    {
        if (m_map1c.find(*(int*)((char*)pkt + 0xb)) == m_map1c.end())
        {
            m_map1c[*(int*)((char*)pkt + 0xb)] = 1;
            for (int i = 0; i <= 5; i++)
            {
                if (pkt->m_uniqueId[i] == -1)
                {
                    break;
                }
                if (m_field4c < pkt->m_modifyTimeInt[i])
                {
                    m_field4c = pkt->m_modifyTimeInt[i];
                }
                MonitoringSpecCase mc;
                mc.m_specId = pkt->m_specId[i];
                mc.m_cpuVendor = pkt->m_cpuVendor[i];
                mc.m_cpuProcessorNum = pkt->m_cpuProcessorNum[i];
                mc.m_aboveCpuClock = pkt->m_aboveCpuClock[i];
                mc.m_belowCpuClock = pkt->m_belowCpuClock[i];
                mc.m_ram = pkt->m_ram[i];
                mc.m_videocardVendor = pkt->m_videocardVendor[i];
                mc.m_videocardDevice = pkt->m_videocardDevice[i];
                mc.m_videocardTextureMem = pkt->m_videocardTextureMem[i];
                mc.m_osVersion = pkt->m_osVersion[i];
                m_monitor[pkt->m_uniqueId[i]] = mc;
                FrameLagDataStruct fd;
                m_data[mc.m_specId] = fd;
            }
            if (m_map1c.size() == (size_t)pkt->m_count)
            {
                m_field4 = 2;
                puts("============FirstSpecLoad Complete!!!==========");
            }
        }
    }
    return 0;
}
// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-STA-DIFF-0023 已消除）
int FrameLagCollector::PushMonitoringSpecData(Packet_Frame_Lag_Statistic_Result_Reload_Spec* pkt)
{
    if (m_field4 != 2)
    {
        return 2;
    }
    if (m_field19 == pkt->m_loadSeq)
    {
        std::map<int, char>::iterator e = m_map34.end();
        if (m_map34.find(*(int*)((char*)pkt + 0xb)) == e)
        {
            m_map34[*(int*)((char*)pkt + 0xb)] = 1;
            for (int i = 0; i <= 5; i++)
            {
                if (pkt->m_uniqueId[i] == -1)
                {
                    break;
                }
                if (m_field50 < pkt->m_modifyTimeInt[i])
                {
                    m_field50 = pkt->m_modifyTimeInt[i];
                }
                std::map<int, MonitoringSpecCase>::iterator it;
                it = m_monitor.find(pkt->m_uniqueId[i]);
                if (it == m_monitor.end())
                {
                    MonitoringSpecCase mc;
                    mc.m_specId = pkt->m_specId[i];
                    mc.m_cpuVendor = pkt->m_cpuVendor[i];
                    mc.m_cpuProcessorNum = pkt->m_cpuProcessorNum[i];
                    mc.m_aboveCpuClock = pkt->m_aboveCpuClock[i];
                    mc.m_belowCpuClock = pkt->m_belowCpuClock[i];
                    mc.m_ram = pkt->m_ram[i];
                    mc.m_videocardVendor = pkt->m_videocardVendor[i];
                    mc.m_videocardDevice = pkt->m_videocardDevice[i];
                    mc.m_videocardTextureMem = pkt->m_videocardTextureMem[i];
                    mc.m_osVersion = pkt->m_osVersion[i];
                    m_monitor[pkt->m_uniqueId[i]] = mc;
                }
                else
                {
                    it->second.m_specId = pkt->m_specId[i];
                    it->second.m_cpuVendor = pkt->m_cpuVendor[i];
                    it->second.m_cpuProcessorNum = pkt->m_cpuProcessorNum[i];
                    it->second.m_aboveCpuClock = pkt->m_aboveCpuClock[i];
                    it->second.m_belowCpuClock = pkt->m_belowCpuClock[i];
                    it->second.m_ram = pkt->m_ram[i];
                    it->second.m_videocardVendor = pkt->m_videocardVendor[i];
                    it->second.m_videocardDevice = pkt->m_videocardDevice[i];
                    it->second.m_videocardTextureMem = pkt->m_videocardTextureMem[i];
                    it->second.m_osVersion = pkt->m_osVersion[i];
                }
                FrameLagDataStruct fd;
                m_data[pkt->m_specId[i]] = fd;
            }
            if (m_map34.size() == (size_t)pkt->m_count)
            {
                if (m_field50 > m_field4c)
                {
                    m_field4c = m_field50;
                }
                puts("============ReloadSpec Complete!!!==========");
            }
        }
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
// [DNF-NONIDENTICAL] DNF-STA-DIFF-0018 | statics | 与ORIG差异=DIFF | FrameLagCollector::SaveFrameLagData | 详见 function_reports/statics/_ZN17FrameLagCollector16SaveFrameLagDataEP14CServerHandler.md
int FrameLagCollector::SaveFrameLagData(CServerHandler* handler)
{
    if (m_field4 != 2)
    {
        return 2;
    }
    m_field10++;
    if (m_field10 >= m_collectInterval)
    {
        m_field10 = 0;
        if (m_field6c == 0)
        {
            return 0;
        }
        Packet_Frame_Lag_Statistic_Write_Lag_Index pkt;
        pkt.m_serverGroup = (char)handler->GetServerGroupNo();
        // [2026-09-15 W5 机理 W3/M 修正] ORIG 在 `time(&t)` 后紧跟
        //   `mov -0x30(%ebp),%eax; mov %eax,-0x38(%ebp)` —— 该 store 的**目标是
        //   pkt 对象内的成员**而非新局部：pkt 基址 = -0x38 - 0x177 = -0x1af（与
        //   ORIG `mov %al,-0x1a5(%ebp)` 即 pkt+0xa 的 m_serverGroup 完全吻合），
        //   而 Packet_Frame_Lag_Statistic_Write_Lag_Index::m_tail 正好在 +0x177
        //   ⇒ 这是 `pkt.m_tail` 的 4 字节写入（原先误判为「额外的 time_t 局部」，
        //   故 tCopy 四形态全部劣化）。
        time_t t;
        time(&t);
        pkt.m_tail = (int)t;
        std::map<int, FrameLagDataStruct>::iterator it;
        for (it = m_data.begin(); it != m_data.end(); ++it)
        {
            pkt.m_key = it->first;
            pkt.m_value = it->second.m0;
            int i;
            for (i = 0; i < 8; i++)
            {
                pkt.m_part[i] = it->second.m_b[i + 8];
            }
            if (it->second.m_c[0] == 0)
            {
                pkt.m_ratio1 = -1;
                pkt.m_ratio2 = -1;
            }
            else
            {
                pkt.m_ratio1 = (short)((double)it->second.m_c[1] /
                        (double)(unsigned int)it->second.m_c[0]);
                pkt.m_ratio2 = (short)((double)it->second.m_c[2] /
                        (double)(unsigned int)it->second.m_c[0]);
            }
            int k;
            for (k = 0; k < 6; k++)
            {
                pkt.m_items[k].m_s[0] = 0;
                pkt.m_items[k].m_s[1] = 0;
                pkt.m_items[k].m_s[2] = 0;
                pkt.m_items[k].m_s[3] = 0;
                if (0 < it->second.m_h[k][0])
                {
                    pkt.m_items[k].m_s[0] =
                        (short)(it->second.m_d[k] / it->second.m_h[k][0]);
                }
                if (0 < it->second.m_h[k][1])
                {
                    pkt.m_items[k].m_s[1] =
                        (short)(it->second.m_e[k] / it->second.m_h[k][1]);
                }
                if (0 < it->second.m_h[k][2])
                {
                    pkt.m_items[k].m_s[2] =
                        (short)(it->second.m_f[k] / it->second.m_h[k][2]);
                }
                if (0 < it->second.m_h[k][3])
                {
                    pkt.m_items[k].m_s[3] =
                        (short)(it->second.m_g[k] / it->second.m_h[k][3]);
                }
                int j;
                for (j = 0; j < 6; j++)
                {
                    pkt.m_items[k].m_pair[j][0] = it->second.m_i[k][j].m_words[0];
                    pkt.m_items[k].m_pair[j][1] = it->second.m_i[k][j].m_words[1];
                }
            }
            handler->SendToDB((PacketHeader*)&pkt);
            it->second.init();
            DNFFLib::Sleep_Ext(0, 1);
        }
        m_field6c = 0;
    }
    time_t t2;
    time(&t2);
    if (t2 < m_field8c || m_field90 < t2)
    {
        m_collectInterval = 0x1e;
    }
    return 0;
}
// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-STA-DIFF-0025 已消除）
int FrameLagCollector::SaveCollectedDirectxVersion(CServerHandler* handler)
{
    if (m_field4 != 2)
    {
        return 2;
    }
    if (m_field9c != m_today)
    {
        m_field9c = m_today;
    Packet_Frame_Lag_Statistic_Write_Query pkt;
    time_t now;
    time(&now);
    snprintf((char*)&pkt + 10, 0x400,
             "inSert into directx_version(occ_time,server_group,ver_etc,ver_8_x,ver_9_0,ver_9_0_a,ver_9_0_b,ver_9_0_c,ver_10_x,ver_11_x) values(from_unixtime(%d),%hhd,%u,%u,%u,%u,%u,%u,%u,%u)",
             (int)now, (signed char)handler->GetServerGroupNo(),
             m_directx.m_data[0], m_directx.m_data[1],
             m_directx.m_data[2], m_directx.m_data[3],
             m_directx.m_data[4], m_directx.m_data[5],
             m_directx.m_data[6], m_directx.m_data[7]);
        handler->SendToDB((PacketHeader*)&pkt);
        m_directx.init();
    }
    return 0;
}
// [DNF-NONIDENTICAL] DNF-STA-DIFF-0024 | statics | 与ORIG差异=DIFF | FrameLagCollector::is_valid_statistic_packet | 详见 function_reports/statics/_ZN17FrameLagCollector25is_valid_statistic_packetEP30Packet_Frame_Lag_Statistic_Add.md
bool FrameLagCollector::is_valid_statistic_packet(Packet_Frame_Lag_Statistic_Add* pkt)
{
    // [2026-09-15 P372] ORIG 顶部前置声明：i 落 -0xc、j 落 -0x8（我方使用点
    //   声明时被排成 -0x8/-0x4）。
    int i;
    int j;
    // [2026-09-15 StaticsDiff 机理 E] 零 cast 直访（原先走视图 cast ⇒ base-先载入，
    //   与 ORIG 的 index-先相反）。字段布局见 shared/packet/include/Packet_Frame_Lag_Statistic_Add.h。
    if ((char)pkt->m_layout.u_main.m_module < 0 ||
        8 < (char)pkt->m_layout.u_main.m_module)
    {
        return false;
    }
    for (i = 0; i < 6; i++)
    {
        if (pkt->m_layout.u_main.m_items[i].m_v0 < 0) return 0;
        if (pkt->m_layout.u_main.m_items[i].m_v1 < 0) return 0;
        if (pkt->m_layout.u_main.m_items[i].m_v2 < 0) return 0;
        if (pkt->m_layout.u_main.m_items[i].m_v3 < 0) return 0;
        for (j = 0; j < 6; j++)
        {
            if (99999 < pkt->m_layout.u_sub.m_sub[i][j][4].m_i) return 0;
            if (pkt->m_layout.u_sub.m_sub[i][j][4].m_f == 0.0 &&
                0 < pkt->m_layout.u_sub.m_sub[i][j][4].m_i) return 0;
        }
    }
    return 1;
}
// [DNF-NONIDENTICAL] DNF-STA-DIFF-0019 | statics | 与ORIG差异=DIFF | FrameLagCollector::accFrameLagStruct | 详见 function_reports/statics/_ZN17FrameLagCollector17accFrameLagStructERNS_18FrameLagDataStructEP14FrameLagStruct.md
void FrameLagCollector::accFrameLagStruct(FrameLagDataStruct& data, FrameLagStruct* pkt)
{
    // [2026-09-15 机理 E] FrameLagStruct 补真实字段后零 cast 直访 ⇒ 长度差消除
    //   （367→356）、ndiff 186→108。
    //   [实测] i 提到函数作用域（i 先声明）+ j 移入内层 for ⇒ 112（劣于 108）⇒ 维持
    //   j 在函数作用域、i 在 for 内（108）。
    // [2026-09-15 机理 N] ORIG 槽深：i=-0x18（42 次引用）、j=-0x14（17 次）⇒ i 先声明占更深槽。
    //   [实测] 只换乘法顺序不重排声明 ⇒ ndiff 76；N + 换序 ⇒ 17（最优）。
    int i;
    int j;
    for (i = 0; i < 6; i++)
    {
        if (0 < pkt[i].m_v0)
        {
            data.m_d[i] +=
                (int)pkt[i].m_v0;
            ++data.m_h[i][0];
        }
        if (0 < pkt[i].m_v1)
        {
            data.m_e[i] +=
                (int)pkt[i].m_v1;
            ++data.m_h[i][1];
        }
        if (0 < pkt[i].m_v2)
        {
            data.m_f[i] +=
                (int)pkt[i].m_v2;
            ++data.m_h[i][2];
        }
        if (0 < pkt[i].m_v3)
        {
            data.m_g[i] +=
                (int)pkt[i].m_v3;
            ++data.m_h[i][3];
        }
        for (j = 0; j < 6; j++)
        {
            data.m_i[i][j].s.m_i =
                data.m_i[i][j].s.m_i *
                data.m_a[i];
            data.m_i[i][j].s.m_f =
                data.m_i[i][j].s.m_f *
                (float)(unsigned int)data.m_a[i];
            data.m_i[i][j].s.m_i +=
                pkt[i].m_sub[j].m_i;
            data.m_i[i][j].s.m_f +=
                pkt[i].m_sub[j].m_f;
            data.m_i[i][j].s.m_i /=
                data.m_a[i] + 1U;
            data.m_i[i][j].s.m_f /=
                (float)(data.m_a[i] + 1U);
        }
        ++data.m_a[i];
    }
}
// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-STA-DIFF-0017 已消除）
int FrameLagCollector::SaveUsedMemory(CServerHandler* handler)
{
    m_field1e4++;
    if (m_field1e4 >= m_collectInterval)
    {
        m_field1e4 = 0;
        Packet_Frame_Lag_Used_Memory_Write_Query pkt;
        for (int i = 0; i < 6; i++)
        {
            for (int j = 0; j < 6; j++)
            {
                if (m_memory[i].m_sums[j] != 0)
                {
                    unsigned int avg = (unsigned int)m_memory[i].m_sums[j] /
                                       (unsigned int)m_memory[i].m_counts[j];
                    snprintf((char*)&pkt + 10, 0x400,
                             "inSert into used_memory (occ_time, minute_type, module, memory) values (now(),%d,%d,%d)",
                             i, j, avg);
                    handler->SendToDB((PacketHeader*)&pkt);
                }
            }
            m_memory[i].init();
        }
    }
    return 0;
}
Packet_Frame_Lag_Statistic_Load_Spec::Packet_Frame_Lag_Statistic_Load_Spec()
    : PacketHeader(0xc27, 0xb)
{
}
FrameLagCollector::FrameLagDataStruct::FrameLagDataStruct()
{
    init();
}
// [DNF-NONIDENTICAL] DNF-STA-DIFF-0020 | statics | 与ORIG差异=DIFF | FrameLagCollector::FrameLagDataStruct::init | 详见 function_reports/statics/_ZN17FrameLagCollector18FrameLagDataStruct4initEv.md
void FrameLagCollector::FrameLagDataStruct::init()
{
    // [2026-09-15 P372] ORIG 顶部前置声明（槽深→浅 i/-0x10、k/-0xc、j/-0x8）；
    //   使用点声明会被 4.4 排成 -0xc/-0x8/-0x4（整体浅 4）。
    // [2026-09-15 实验] 内层 `m_i[k][j]` 写成二维后 ORIG 的 %ebx 承载 j 的形态复现
    //   （ndiff 44→2，仅剩 m_i 基址 disp 的 0x8/0x10 拆分差）。
    int i;
    int k;
    int j;
    m0 = 0;
    for (i = 0; i < 8; i++)
    {
        m_b[i] = 0;
    }
    m_c[0] = 0;
    m_c[1] = 0;
    m_c[2] = 0;
    for (k = 0; k < 6; k++)
    {
        m_a[k] = 0;
        m_d[k] = 0;
        m_e[k] = 0;
        m_f[k] = 0;
        m_g[k] = 0;
        m_h[k][0] = 0;
        m_h[k][1] = 0;
        m_h[k][2] = 0;
        m_h[k][3] = 0;
        for (j = 0; j < 6; j++)
        {
            m_i[k][j].s.m_i = 0;
            m_i[k][j].s.m_f = 0.0f;
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
