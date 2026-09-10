// ============================================================================
// Wave6 Hack 惩罚域实现（WongWork 黑名单 / IP 计数器 / 反作弊规则）。
// 控制流/常量取自 df_game_r 反汇编与
// docs/class_func_reports/WongWork__CAutoPunishRuleHackTypeMgr/ 报告。
// ============================================================================
#include "W6_HackPunishDeps.h"

#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

// ----------------------------------------------------------------------------
// 【临时】CUser.h 当前引用但尚未有任何权威头提供声明的类型（Wave6 进行中）；
// 仅本 TU 编译期可见：引用参数所需的最小前向 + 枚举占位（不带语义假设）。
// 权威头落地后删除本块并改 include。
// ----------------------------------------------------------------------------
#ifndef DNF_W6_HP_USER_FWD_TYPES
#define DNF_W6_HP_USER_FWD_TYPES
class CodeHackChecksum;
class CodeHackChecksumEx;
class CodeHackCheckRangeEx;
// [dedup] 两枚举权威定义见 CUser.h 守卫段（include 后即可见）。
#endif

#include "CUser.h"   // CUser::m_historyLog（+0x79700 cUserHistoryLog）

// Divide_C_ClassIP_EndIP 权威声明见 Divide_C_ClassIP_EndIP.h（W7 批次新建 TU）；
// ORIG 返回 void（_Z22Divide_C_ClassIP_EndIPPcS_S_ @0x086b1dd5），原 char* 声明修正。
#include "Divide_C_ClassIP_EndIP.h"

namespace WongWork
{

// ----------------------------------------------------------------------------
// HackIPCounter_t —— ORIG W C1/C2(EjjPcj) 0x080f9892 (0x63)：
// +0/+2 截 u16、+0x14=cnt，ip 区 memset(0xd)+strncpy(0xd)。C2 与 C1 同地址。
// ----------------------------------------------------------------------------
HackIPCounter_t::HackIPCounter_t(unsigned int a, unsigned int b, char* ip,
                                 unsigned int cnt)
{
    m_a = (unsigned short)a;   // mov %dx,(%eax)
    m_b = (unsigned short)b;   // mov %dx,0x2(%eax)
    m_cnt = cnt;               // mov %edx,0x14(%eax)
    memset(m_ip, 0, 0xd);      // memset(this+4, 0, 0xd)
    strncpy(m_ip, ip, 0xd);    // strncpy(this+4, param3, 0xd)
}

// ----------------------------------------------------------------------------
// HackFullIPCounter_t —— ORIG W C1/C2(EjjPcj) 0x080f98f6 (0x63)：
// 同构，ip 区长 0x10。
// ----------------------------------------------------------------------------
HackFullIPCounter_t::HackFullIPCounter_t(unsigned int a, unsigned int b, char* ip,
                                         unsigned int cnt)
{
    m_a = (unsigned short)a;
    m_b = (unsigned short)b;
    m_cnt = cnt;
    memset(m_ip, 0, 0x10);
    strncpy(m_ip, ip, 0x10);
}

// ----------------------------------------------------------------------------
// CAutoPunishRuleBlackIPMgr::stIPInfo_t —— ORIG W C1/C2(Ehh) 0x084eb598 (0x27)。
// ----------------------------------------------------------------------------
CAutoPunishRuleBlackIPMgr::stIPInfo_t::stIPInfo_t(unsigned char minIp,
                                                  unsigned char maxIp)
{
    m_minIp = minIp;   // mov %dl,(%eax)
    m_maxIp = maxIp;   // mov %dl,0x1(%eax)
}

// ----------------------------------------------------------------------------
// addBlackIP —— ORIG W 0x084eb5c0 (0x7e)：stIPInfo_t(a,b) 后 insert(make_pair(addr))，
// 固定 return true。
// ----------------------------------------------------------------------------
bool CAutoPunishRuleBlackIPMgr::addBlackIP(unsigned long addr, unsigned char minIp,
                                           unsigned char maxIp)
{
    // b28d-solo-addBlackIP: ORIG make_pair 实例第二参为 stIPInfo_t&&（mangled OS9_）
    // ⇒ 原码是匿名右值临时而非具名左值 info。ORIG 0x084eb5c0。
    m_map.insert(std::make_pair(addr, stIPInfo_t(minIp, maxIp)));
    return true;   // mov $0x1,%eax
}

// ----------------------------------------------------------------------------
// isBlackIP —— ORIG W 0x084eb63e (0x18c)：把点分 IP 从末段向前找 '.'，
// 取末段数值 lastSeg，末段替换为 "0"，inet_addr 得网络地址 key；
// lower_bound..upper_bound(netAddr) 内区间 [minIp,maxIp] 覆盖 lastSeg 即命中。
// ----------------------------------------------------------------------------
bool CAutoPunishRuleBlackIPMgr::isBlackIP(char const* ip)
{
    // b28d-solo-isBlackIP: **AE 111/111**（check_tu_routed fresh）。历次修复：
    // ipBuf 0x12（4×movl+movw 内联清零，原 0x10 且 i+2 可越界到 0x11）、lastSeg
    // 无符号化（复现 mem 直比 + ja/jb）、命中判定改 `A && B` 语句表达式
    // （P226 零槽短路物化）。本轮 N98 槽序翻正：把 seg 从循环体内提为**函数头
    // union 视图**（`union { char m_c[4]; unsigned int m_w; }`）并在循环内经
    // `seg.m_w = 0` 写整字 —— 同时满足 ORIG 的三条直证：seg 槽 -0x34（浅于
    // cit1 -0x38/cit2 -0x3c）、`movl $0,-0x34(%ebp)` 单条、且该清零仍在循环体内
    // （命中才清）。对照：块内 `char seg[4]={0}` 被 4.4 排到最深槽 -0x3c；函数头
    // `char seg[4]={0}` 槽号虽对但清零被提到序言（DIFF）。ORIG 0x084eb63e。
    char ipBuf[0x12] = {0};                    // 4×movl + movw 内联清零
    strncpy(ipBuf, ip, 0x10);                  // strncpy(ipBuf, param, 0x10)

    unsigned long netAddr = 0;                 // -0x30
    unsigned char lastSeg = 0;                 // -0xd（ORIG cmp 为 8 位无符号比较）
    // P212/P309 + P246：ORIG 槽序 seg(-0x34) 浅于 it(-0x38)/endIt(-0x3c)，且
    // `movl $0,-0x34` 在循环体内（命中才清零）⇒ 函数头声明 union 视图占槽、
    // 循环内经 `m_w` 写整字（4.4 对块内 `char seg[4]={0}` 给最深槽 -0x3c 且
    // 函数头初始化会把清零提到序言）。ORIG 0x084eb63e。
    union { char m_c[4]; unsigned int m_w; } seg;
    for (int i = 0xf; i >= 0; --i)             // i=-0xc 起始 0xf，not+shr 判 ≥0
    {
        if (ipBuf[i] == '.')                   // cmp $0x2e,%al
        {
            seg.m_w = 0;                       // movl $0,-0x34(%ebp) 单条
            strcpy(seg.m_c, &ipBuf[i + 1]);
            lastSeg = (unsigned char)atoi(seg.m_c); // mov %al,-0xd(%ebp)
            ipBuf[i + 1] = '0';                // movb $0x30
            ipBuf[i + 2] = '\0';               // movb $0x0
            netAddr = inet_addr(ipBuf);        // inet_addr(ipBuf) -> netlong
            break;
        }
    }

    std::multimap<unsigned long, stIPInfo_t>::const_iterator it =
        m_map.lower_bound(netAddr);
    std::multimap<unsigned long, stIPInfo_t>::const_iterator endIt =
        m_map.upper_bound(netAddr);
    for (; it != endIt; ++it)
    {
        // ORIG: ja L_false / jb L_false / mov $1 / L_false: mov $0 / test;je 迭代。
        if (({
                it->second.m_minIp <= lastSeg &&
                it->second.m_maxIp >= lastSeg;
            }))
        {
            return true;                       // 区间覆盖 -> $1
        }
    }
    return false;                              // range 尽 -> $0
}

// ----------------------------------------------------------------------------
// getBlackIPCount —— ORIG W 0x084eb7ca (0x13)：tail-call multimap::size()。
// ----------------------------------------------------------------------------
int CAutoPunishRuleBlackIPMgr::getBlackIPCount()
{
    return (int)m_map.size();
}

// ----------------------------------------------------------------------------
// setHackCnt —— ORIG W 0x084eb854 (0x17f)：HackTypeInfo_t(type1,u16(value1),
// type2,u16(value2),u16(v5),u16(v6),v7) 以 key=type1 insert；
// 若 type2 != 0 再以 key=type2 insert 第二条。
// ----------------------------------------------------------------------------
void CAutoPunishRuleHackTypeMgr::setHackCnt(ENUM_HACKTYPE type1, unsigned int value1,
                                            ENUM_HACKTYPE type2, unsigned int value2,
                                            unsigned int v5, unsigned int v6,
                                            unsigned int v7)
{
    // b28d-solo-setHackCnt: ORIG make_pair 实例第二参为 HackTypeInfo_t&&（mangled OS9_）
    // ⇒ 原码为匿名右值临时；具名 info 使 4.4 把对象排到最深处。ORIG 0x084eb854。
    m_hackTypeMap.insert(std::make_pair(
        type1, HackTypeInfo_t(type1, (unsigned short)value1, type2,
                              (unsigned short)value2, (unsigned short)v5,
                              (unsigned short)v6, v7)));

    if (type2 != (ENUM_HACKTYPE)0)             // test 0x14(ebp)
    {
        m_hackTypeMap.insert(std::make_pair(
            type2, HackTypeInfo_t(type1, (unsigned short)value1, type2,
                                  (unsigned short)value2, (unsigned short)v5,
                                  (unsigned short)v6, v7)));
    }
}

// ----------------------------------------------------------------------------
// setTotalTrade —— ORIG W 0x084eb9d4 (0x2c)：+0x18=type，+0x1c..=total(u64)。
// ----------------------------------------------------------------------------
void CAutoPunishRuleHackTypeMgr::setTotalTrade(unsigned int type,
                                               unsigned long long total)
{
    m_totalTradeType = type;
    m_totalTradeValue = total;
}

// ----------------------------------------------------------------------------
// GetExceptionCnt —— ORIG T 0x080f972a (0x8a)，sret 返回 POD(4B)：
// find(key)；命中取 pair.second，未命中 stHacktypeExceptionInfo_t(0,0)。
// ----------------------------------------------------------------------------
stHacktypeExceptionInfo_t CAutoPunishRuleHackTypeMgr::GetExceptionCnt(int key)
{
    std::map<int, stHacktypeExceptionInfo_t>::iterator it = m_exceptionMap.find(key);
    if (it != m_exceptionMap.end())
    {
        return it->second;
    }
    return stHacktypeExceptionInfo_t(0, 0);
}

// ----------------------------------------------------------------------------
// setIPCounterData —— ORIG T 0x080f9218 (0x109)：HackIPCounter_t(a,b,ip,cnt)
// 以 key=(u16)a insert 入 +0x24 表；b!=0 时再以 key=(u16)b 插一条新构造副本。
// ----------------------------------------------------------------------------
void CAutoPunishRuleHackTypeMgr::setIPCounterData(unsigned int a, unsigned int b,
                                                  char* ip, unsigned int cnt)
{
    // b28d-solo-setIPCounterData: ORIG make_pair 实例为
    // make_pair<unsigned int&, HackIPCounter_t>（第一参取形参 a/b 地址，
    // 第二参匿名右值临时）——旧写法 (unsigned short)a + 具名 entry 两个旋钮全错。
    // ORIG 0x080f9218。
    m_ipCounterMap.insert(std::make_pair(a, HackIPCounter_t(a, b, ip, cnt)));

    if (b != 0)
    {
        m_ipCounterMap.insert(std::make_pair(b, HackIPCounter_t(a, b, ip, cnt)));
    }
}

// ----------------------------------------------------------------------------
// _findIPCount —— ORIG T 0x080f9322 (0x148)：equal_range((u16)a) 遍历，
// 元素 m_b==b 时 Divide_C_ClassIP_EndIP 取 class-C 前缀，
// 前缀非空且 strncmp(prefix, entry.m_ip, 0xc)==0 → 返回 m_cnt；否则 0。
// ----------------------------------------------------------------------------
int CAutoPunishRuleHackTypeMgr::_findIPCount(unsigned int a, unsigned int b,
                                             char const* ipStr)
{
    // b28d-solo-_findIPCount: ORIG 循环体对 it->second 做 0x18 字节按值拷贝到
    // -0x38（6 条 mov 对），prefix 用 4 条 movl 内联清零，且 m_b 与 strncmp 为
    // 两个独立守卫落同一 continue 出口。残差 1 条（gap=+1，dual_rank 唯一候选）：
    // ORIG `lea -0x48(%ebp),%eax; movzbl (%eax),%eax`（4.1 数组元素读恒发地址），
    // 4.4 恒折 `movzbl -0x48(%ebp),%eax`。本轮再探针三形（/tmp/probe_lea.cpp：
    // `prefix[0]`、`*prefix`、`*(char*)((unsigned int)prefix)`）**全部折 disp**；
    // 具名指针/引用形能出 lea 但多占 4B 槽破坏帧 0x58 ⇒ N117/N102 封顶。
    // ORIG 0x080f9322。
    typedef std::multimap<unsigned short, HackIPCounter_t> IpCounterMap;
    IpCounterMap::const_iterator it = m_ipCounterMap.lower_bound((unsigned short)a);
    IpCounterMap::const_iterator endIt = m_ipCounterMap.upper_bound((unsigned short)a);

    for (; it != endIt; ++it)
    {
        HackIPCounter_t entry = it->second;
        if ((unsigned int)entry.m_b == b)
        {
            char prefix[0x10] = {0};
            Divide_C_ClassIP_EndIP(prefix, (char*)0, (char*)ipStr);
            if (prefix[0] != '\0')
            {
                if (strncmp(prefix, entry.m_ip, 0xc) == 0)
                {
                    return (int)entry.m_cnt;
                }
            }
        }
    }
    return 0;
}

// ----------------------------------------------------------------------------
// WriteIPCounterLog —— ORIG T 0x080f967a (0x57)：getUserP 非空时转发
// cUserHistoryLog(+0x79700)::IPCounterLog(...)。
// ----------------------------------------------------------------------------
void CAutoPunishRuleHackTypeMgr::WriteIPCounterLog(CHackAnalyzer* analyzer, int a,
                                                   int b, int c, int d, int e,
                                                   char* ip)
{
    CUser* user = analyzer->getUserP();
    if (user != 0)
    {
        user->m_historyLog.IPCounterLog(a, b, c, d, e, ip);
    }
}

// ----------------------------------------------------------------------------
// ResetIpCounterData —— ORIG W 0x084eba00 (0x16)：clear +0x24 multimap。
// ----------------------------------------------------------------------------
void CAutoPunishRuleHackTypeMgr::ResetIpCounterData()
{
    m_ipCounterMap.clear();
}

// ----------------------------------------------------------------------------
// setFullIPCounterData —— ORIG T 0x080f946a (0x109)：对 +0x40 全 IP 表，
// 结构同 setIPCounterData。
// ----------------------------------------------------------------------------
void CAutoPunishRuleHackTypeMgr::setFullIPCounterData(unsigned int a, unsigned int b,
                                                      char* ip, unsigned int cnt)
{
    // b28d-solo-setFullIPCounterData: 同 setIPCounterData——ORIG make_pair 实例为
    // make_pair<unsigned int&, HackFullIPCounter_t>（形参 a/b 取址 + 匿名右值临时）。
    // ORIG 0x080f946a。
    m_fullIPCounterMap.insert(std::make_pair(a, HackFullIPCounter_t(a, b, ip, cnt)));

    if (b != 0)
    {
        m_fullIPCounterMap.insert(std::make_pair(b, HackFullIPCounter_t(a, b, ip, cnt)));
    }
}

// ----------------------------------------------------------------------------
// _findFullIPCount —— ORIG T 0x080f9574 (0x105)：equal_range((u16)a) 遍历，
// 元素 m_b==b 且 strncmp(param3, entry.m_ip, 0x10)==0 → 返回 m_cnt；否则 0。
// ----------------------------------------------------------------------------
int CAutoPunishRuleHackTypeMgr::_findFullIPCount(unsigned int a, unsigned int b,
                                                 char const* ipStr)
{
    // b28d-solo-_findFullIPCount: ORIG 循环体对 it->second 做 0x18 字节按值拷贝到
    // -0x38（6 条 mov 对），m_b 与 strncmp 为两个独立守卫落同一 continue 出口。
    // ORIG 0x080f9574。
    typedef std::multimap<unsigned short, HackFullIPCounter_t> FullCounterMap;
    FullCounterMap::const_iterator it = m_fullIPCounterMap.lower_bound((unsigned short)a);
    FullCounterMap::const_iterator endIt = m_fullIPCounterMap.upper_bound((unsigned short)a);

    for (; it != endIt; ++it)
    {
        HackFullIPCounter_t entry = it->second;
        if ((unsigned int)entry.m_b == b)
        {
            if (strncmp(ipStr, entry.m_ip, 0x10) == 0)
            {
                return (int)entry.m_cnt;
            }
        }
    }
    return 0;
}

// ----------------------------------------------------------------------------
// WriteFullIPCounterLog —— ORIG T 0x080f96d2 (0x57)：转发
// cUserHistoryLog(+0x79700)::FullIPCounterLog(...)。
// ----------------------------------------------------------------------------
void CAutoPunishRuleHackTypeMgr::WriteFullIPCounterLog(CHackAnalyzer* analyzer,
                                                       int a, int b, int c, int d,
                                                       int e, char* ip)
{
    CUser* user = analyzer->getUserP();
    if (user != 0)
    {
        user->m_historyLog.FullIPCounterLog(a, b, c, d, e, ip);
    }
}

// ----------------------------------------------------------------------------
// ResetFullIpCounterData —— ORIG W 0x084eba16 (0x16)：clear +0x40 multimap。
// ----------------------------------------------------------------------------
void CAutoPunishRuleHackTypeMgr::ResetFullIpCounterData()
{
    m_fullIPCounterMap.clear();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// _checkApplyFlag —— ORIG W 0x0869550c。
// 依据 objdump 逐分支恢复：
//   m_value3 == 1/5  → 拷贝出参直接放行；
//   m_value3 == 2/10 → 在 lookOutsA/lookOutsB 观察名单中命中 {m_type1,m_type2}
//                      即拒绝；m_value3 == 3 → 在 ipCounters 名单中命中即拒绝；
//   m_value3 == 4/9  → _findIPCount 超阈值记日志并拒绝，否则记日志继续；
//   m_value3 == 0xb/0xc → _findFullIPCount 同型处理；
//   其余 → 拷贝出参放行。
// ----------------------------------------------------------------------------
bool CAutoPunishRuleHackTypeMgr::_checkApplyFlag(
    const HackTypeInfo_t& info, CHackAnalyzer* analyzer, HackTypeInfo_t& outInfo,
    std::vector<stLookOuts>& lookOutsA, std::vector<tyIPCounters>& ipCounters,
    std::vector<stLookOuts>& lookOutsB, char* ip)
{
    // b28d-solo-_checkApplyFlag: N 封顶（寄存器分配/帧差）。已修：①cnt 两句式
    // 预清（P198①）；②补回 ORIG 0x8695825 的冗余 `szC = ipCounters.size()` 求值
    // ⇒ 324=324、p167 CLEAR 18/18。残差：ORIG 用 %esi 缓存 ip、%ebx 缓存 cnt，
    // 4.4 两值共用 %ebx（帧 0x44 vs 0x40、缺 push %esi）+ `m_value4 > (unsigned
    // int)cnt` 4.4 恒载 cnt 入寄存器而 ORIG mem 直比（P209）。**jcc 逐位交叉
    // 验证：37/37 MATCH**（控制流与 ORIG 等价，残差纯为寄存器分配/装载序，
    // 属 P75/N7 跨版本结构性差异）。ORIG 0x0869550c。
    // [F2a-C05② 复测·不适用] 件级 F2 旋钮（具名局部 unsigned→int 求比较内存直比）
    //   逐条核对报告 5 处 ORIG `cmp -0xNN(%ebp),%eax`：其中 3 处（`setl` 族，
    //   ORIG 0x869550c+0x?? 的 cnt 循环尾测）我方已为同形 mem 直比（仅槽号差）；
    //   其余 2 处 = `m_value4 > (unsigned int)cnt`（ORIG `cmp -0x10/-0xc(%ebp),%eax;
    //   jbe`）——mem 操作数 cnt 是**跨调用返回值**（_findIPCount 结果局部），
    //   且 jcc 为**无符号序 jbe**（P196 红线：局部改 int 即翻有符号序，属真语义
    //   bug 家族）⇒ 禁改，维持 P209/N18b 封顶。无改动、无回滚。
    if (info.m_value3 == 1 || info.m_value3 == 5)
    {
        outInfo.CopyStruct(info);
        return true;
    }

    int szA = (int)lookOutsA.size();
    if (info.m_value3 == 2 && szA > 0)
    {
        for (int i = 0; i < szA; ++i)
        {
            if (info.m_type1 == lookOutsA[i].m_a &&
                info.m_type2 == lookOutsA[i].m_b)
            {
                return false;
            }
        }
    }

    int szB = (int)lookOutsB.size();
    if (info.m_value3 == 10 && szB > 0)
    {
        for (int i = 0; i < szB; ++i)
        {
            if (info.m_type1 == lookOutsB[i].m_a &&
                info.m_type2 == lookOutsB[i].m_b)
            {
                return false;
            }
        }
    }

    int szC = (int)ipCounters.size();
    if (info.m_value3 == 3 && szC > 0)
    {
        for (int i = 0; i < szC; ++i)
        {
            if (info.m_type1 == ipCounters[i].m_a &&
                info.m_type2 == ipCounters[i].m_b)
            {
                return false;
            }
        }
    }

    if (info.m_value3 == 4 || info.m_value3 == 9)
    {
        // P198①：ORIG 在 call 前有 `movl $0x0,-0x10(%ebp)` 预清。
        int cnt = 0;
        cnt = _findIPCount(info.m_type1, info.m_type2, ip);
        if (info.m_value4 > (unsigned int)cnt)
        {
            WriteIPCounterLog(analyzer, 0, info.m_type1, info.m_type2,
                              info.m_value4, cnt, ip);
            return false;
        }
        WriteIPCounterLog(analyzer, 1, info.m_type1, info.m_type2,
                          info.m_value4, cnt, ip);
    }

    // ORIG 0x8695825 在 4/9 段与 11/12 段之间重新求值 ipCounters.size() 并复用
    // szC 槽（-0x20）；P167 差集 MISSING 1×tyIPCounters::size()。
    szC = (int)ipCounters.size();

    if (info.m_value3 == 11 || info.m_value3 == 12)
    {
        // P198①：ORIG 在 call 前有 `movl $0x0,-0xc(%ebp)` 预清。
        int cnt = 0;
        cnt = _findFullIPCount(info.m_type1, info.m_type2, ip);
        if (info.m_value4 > (unsigned int)cnt)
        {
            WriteFullIPCounterLog(analyzer, 0, info.m_type1, info.m_type2,
                                  info.m_value4, cnt, ip);
            return false;
        }
        WriteFullIPCounterLog(analyzer, 1, info.m_type1, info.m_type2,
                              info.m_value4, cnt, ip);
    }

    outInfo.CopyStruct(info);
    return true;
}

// ----------------------------------------------------------------------------
// checkMaxHackCntOver —— ORIG W 0x08695360。
// 遍历 m_hackTypeMap 中 key == type 的 [lower_bound, upper_bound) 区间；
// 规则条目按两个对称条件判断"累计计数超限"（条件成立即置 flag）：
//   A: m_type1==type && m_value1<=param && getServerHackAccumulatedCnt(m_type2)>=m_value2
//   B: m_type2==type && m_value2<=param && getServerHackAccumulatedCnt(m_type1)>=m_value1
// flag 置位后交由 _checkApplyFlag 处置；返回 true 表示命中处置（调用方终止）。
// ----------------------------------------------------------------------------
bool CAutoPunishRuleHackTypeMgr::checkMaxHackCntOver(
    ENUM_HACKTYPE type, unsigned int param, CHackAnalyzer* analyzer,
    HackTypeInfo_t& outInfo, std::vector<stLookOuts>& lookOutsA,
    std::vector<tyIPCounters>& ipCounters, std::vector<stLookOuts>& lookOutsB,
    char* ip)
{
    // b28d-solo-checkMaxHackCntOver: **AE 131/131**（check_tu_routed fresh）。
    // 历次修复：去掉 (unsigned short)type cast（复现 32 位 cmp）、m_value2 左置
    // 复现 call 前 %ebx 缓存、A/B 条件改 else-if 链。本轮两处翻正：
    //  ① P225 推广：`if (flag && _checkApplyFlag(...))` 改**嵌套 if** —— 合并形
    //     在 4.4 物化 bool（mov $0/mov $1+test），多 1 个 je+1 个 jmp，jcc 序列
    //     与 ORIG 失配（jcc 逐位核对：修前 MISMATCH、修后 MATCH）。
    //  ② P326 变体：去掉具名 `iterator lower/upper` 中间量、直接以 const_iterator
    //     接收 lower_bound/upper_bound 的返回值 —— 复现 ORIG 的 sret 临时槽阶梯
    //     lower(-0x14)/cit(-0x18)、upper(-0x10)/citEnd(-0x1c)（旧形 lower 组反而
    //     落在浅槽，帧 0x54 两侧一致但槽号互换）。
    // ORIG 0x08695360。
    std::multimap<ENUM_HACKTYPE, HackTypeInfo_t>::const_iterator cit =
        m_hackTypeMap.lower_bound(type);
    std::multimap<ENUM_HACKTYPE, HackTypeInfo_t>::const_iterator citEnd =
        m_hackTypeMap.upper_bound(type);

    for (; cit != citEnd; ++cit)
    {
        HackTypeInfo_t info = cit->second;
        bool flag = false;
        if (info.m_type1 == type &&
            info.m_value1 <= param &&
            info.m_value2 <= analyzer->getServerHackAccumulatedCnt(
                (ENUM_HACKTYPE)info.m_type2))
        {
            flag = true;
        }
        // ORIG: A 成立即置 flag 并跳过 B（else-if 链）。
        else if (info.m_type2 == type &&
                 info.m_value2 <= param &&
                 info.m_value1 <= analyzer->getServerHackAccumulatedCnt(
                     (ENUM_HACKTYPE)info.m_type1))
        {
            flag = true;
        }
        // P225 推广：ORIG 为 `cmpb flag; je 尾; call; test %al; je 尾; mov $1`
        // 的**嵌套 if 直跳**形；合并 `flag && f()` 在 4.4 会物化 bool
        // （mov $0/mov $1 + test）多出 1 个 je + 1 个 jmp（jcc 序列失配）。
        if (flag)
        {
            if (_checkApplyFlag(info, analyzer, outInfo, lookOutsA,
                                ipCounters, lookOutsB, ip))
            {
                return true;
            }
        }
    }
    return false;
}

// checkTotalTradeRuleOver —— ORIG W 0x08531fd2 (0x63) const：
//  type==0 且 totalValue==0          -> false
//  totalTradeType > tradeType        -> false
//  value 低 32 位 > total 低 32 位    -> false
//  低相等时高位 > total 高位          -> false
//  其余                               -> true
// （即记录的总量阈值未被当前 total 超越即视为 over。）
// ----------------------------------------------------------------------------
bool CAutoPunishRuleHackTypeMgr::checkTotalTradeRuleOver(unsigned int tradeType,
                                                         unsigned long long total) const
{
    // b28d-solo-checkTotalTradeRuleOver v3: ORIG 0x08531fd2 = **64 位成员整体比较**
    // `m_totalTradeValue <= total` 的正向包裹形（探针 fF 逐条命中）：`ja ret0`（type>）、
    // `cmp hi; ja ret0; cmp hi; jb ret1; cmp lo; ja ret0` 是 4.1 对 `<=` 的展开；
    // 旧 lo/hi 具名手写比较（成员 hi 重读 3 次）与 ORIG 的 hi 单读复用不符。
    if (m_totalTradeType == 0 && m_totalTradeValue == 0)
    {
        return false;
    }
    if (m_totalTradeType <= tradeType)
    {
        if (m_totalTradeValue <= total)
        {
            return true;
        }
    }
    return false;
}

}  // namespace WongWork
