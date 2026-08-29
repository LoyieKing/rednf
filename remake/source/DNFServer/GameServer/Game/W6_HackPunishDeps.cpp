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
    stIPInfo_t info(minIp, maxIp);
    m_map.insert(std::make_pair(addr, info));
    return true;   // mov $0x1,%eax
}

// ----------------------------------------------------------------------------
// isBlackIP —— ORIG W 0x084eb63e (0x18c)：把点分 IP 从末段向前找 '.'，
// 取末段数值 lastSeg，末段替换为 "0"，inet_addr 得网络地址 key；
// lower_bound..upper_bound(netAddr) 内区间 [minIp,maxIp] 覆盖 lastSeg 即命中。
// ----------------------------------------------------------------------------
bool CAutoPunishRuleBlackIPMgr::isBlackIP(char const* ip)
{
    char ipBuf[0x10];
    memset(ipBuf, 0, sizeof(ipBuf));           // 逐 dword 清零 -0x2a 区
    strncpy(ipBuf, ip, 0x10);                  // strncpy(ipBuf, param, 0x10)

    unsigned long netAddr = 0;                 // -0x30
    char lastSeg = 0;                          // -0xd
    for (int i = 0xf; i >= 0; --i)             // i=-0xc 起始 0xf，not+shr 判 ≥0
    {
        if (ipBuf[i] == '.')                   // cmp $0x2e,%al
        {
            char seg[0x20];                    // strcpy 目标 -0x34
            strcpy(seg, &ipBuf[i + 1]);
            lastSeg = (char)atoi(seg);         // mov %al,-0xd(%ebp)
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
        if ((unsigned char)it->second.m_minIp > (unsigned char)lastSeg)
        {
            continue;                          // ja -> 本元素不命中，继续迭代
        }
        if ((unsigned char)it->second.m_maxIp < (unsigned char)lastSeg)
        {
            continue;                          // jb -> 继续迭代
        }
        return true;                           // 区间覆盖 -> $1
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
    HackTypeInfo_t info(type1, (unsigned short)value1,
                        type2, (unsigned short)value2,
                        (unsigned short)v5, (unsigned short)v6, v7);
    m_hackTypeMap.insert(std::make_pair(type1, info));

    if (type2 != (ENUM_HACKTYPE)0)             // test 0x14(ebp)
    {
        HackTypeInfo_t info2(type1, (unsigned short)value1,
                             type2, (unsigned short)value2,
                             (unsigned short)v5, (unsigned short)v6, v7);
        m_hackTypeMap.insert(std::make_pair(type2, info2));
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
    HackIPCounter_t entry(a, b, ip, cnt);
    m_ipCounterMap.insert(std::make_pair((unsigned short)a, entry));

    if (b != 0)
    {
        HackIPCounter_t entry2(a, b, ip, cnt);
        m_ipCounterMap.insert(std::make_pair((unsigned short)b, entry2));
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
    typedef std::multimap<unsigned short, HackIPCounter_t> IpCounterMap;
    IpCounterMap::const_iterator it = m_ipCounterMap.lower_bound((unsigned short)a);
    IpCounterMap::const_iterator endIt = m_ipCounterMap.upper_bound((unsigned short)a);

    for (; it != endIt; ++it)
    {
        const HackIPCounter_t& entry = it->second;
        if ((unsigned int)entry.m_b == b)
        {
            char prefix[0x10];
            memset(prefix, 0, sizeof(prefix));
            Divide_C_ClassIP_EndIP(prefix, (char*)0, (char*)ipStr);
            if (prefix[0] != '\0' && strncmp(prefix, entry.m_ip, 0xc) == 0)
            {
                return (int)entry.m_cnt;
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
    HackFullIPCounter_t entry(a, b, ip, cnt);
    m_fullIPCounterMap.insert(std::make_pair((unsigned short)a, entry));

    if (b != 0)
    {
        HackFullIPCounter_t entry2(a, b, ip, cnt);
        m_fullIPCounterMap.insert(std::make_pair((unsigned short)b, entry2));
    }
}

// ----------------------------------------------------------------------------
// _findFullIPCount —— ORIG T 0x080f9574 (0x105)：equal_range((u16)a) 遍历，
// 元素 m_b==b 且 strncmp(param3, entry.m_ip, 0x10)==0 → 返回 m_cnt；否则 0。
// ----------------------------------------------------------------------------
int CAutoPunishRuleHackTypeMgr::_findFullIPCount(unsigned int a, unsigned int b,
                                                 char const* ipStr)
{
    typedef std::multimap<unsigned short, HackFullIPCounter_t> FullCounterMap;
    FullCounterMap::const_iterator it = m_fullIPCounterMap.lower_bound((unsigned short)a);
    FullCounterMap::const_iterator endIt = m_fullIPCounterMap.upper_bound((unsigned short)a);

    for (; it != endIt; ++it)
    {
        const HackFullIPCounter_t& entry = it->second;
        if ((unsigned int)entry.m_b == b &&
            strncmp(ipStr, entry.m_ip, 0x10) == 0)
        {
            return (int)entry.m_cnt;
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
        int cnt = _findIPCount(info.m_type1, info.m_type2, ip);
        if (info.m_value4 > (unsigned int)cnt)
        {
            WriteIPCounterLog(analyzer, 0, info.m_type1, info.m_type2,
                              info.m_value4, cnt, ip);
            return false;
        }
        WriteIPCounterLog(analyzer, 1, info.m_type1, info.m_type2,
                          info.m_value4, cnt, ip);
    }

    if (info.m_value3 == 11 || info.m_value3 == 12)
    {
        int cnt = _findFullIPCount(info.m_type1, info.m_type2, ip);
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
    std::multimap<ENUM_HACKTYPE, HackTypeInfo_t>::iterator lower =
        m_hackTypeMap.lower_bound(type);
    std::multimap<ENUM_HACKTYPE, HackTypeInfo_t>::const_iterator cit = lower;
    std::multimap<ENUM_HACKTYPE, HackTypeInfo_t>::iterator upper =
        m_hackTypeMap.upper_bound(type);
    std::multimap<ENUM_HACKTYPE, HackTypeInfo_t>::const_iterator citEnd = upper;

    for (; cit != citEnd; ++cit)
    {
        HackTypeInfo_t info = cit->second;
        bool flag = false;
        if (info.m_type1 == (unsigned short)type &&
            info.m_value1 <= param &&
            analyzer->getServerHackAccumulatedCnt(
                (ENUM_HACKTYPE)info.m_type2) >= info.m_value2)
        {
            flag = true;
        }
        if (info.m_type2 == (unsigned short)type &&
            info.m_value2 <= param &&
            analyzer->getServerHackAccumulatedCnt(
                (ENUM_HACKTYPE)info.m_type1) >= info.m_value1)
        {
            flag = true;
        }
        if (flag && _checkApplyFlag(info, analyzer, outInfo, lookOutsA,
                                    ipCounters, lookOutsB, ip))
        {
            return true;
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
    unsigned int totalLo = (unsigned int)(total & 0xffffffffULL);
    unsigned int totalHi = (unsigned int)(total >> 32);

    if (m_totalTradeType == 0 && m_totalTradeValue == 0)
    {
        return false;
    }
    if (m_totalTradeType > tradeType)
    {
        return false;
    }
    {
        unsigned int ruleLo = (unsigned int)m_totalTradeValue;
        if (ruleLo > totalLo)
        {
            return false;
        }
        if (ruleLo < totalLo)
        {
            return true;
        }
        unsigned int ruleHi = (unsigned int)(m_totalTradeValue >> 32);
        if (ruleHi > totalHi)
        {
            return false;
        }
    }
    return true;
}

}  // namespace WongWork
