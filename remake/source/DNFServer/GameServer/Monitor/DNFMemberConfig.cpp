// df_monitor_r — DNFMemberConfig（从 MonitorTypes/App/Table 拆分）
#include <stdio.h>
#include "RawAccess.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <signal.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <algorithm>

#include "DNFMemberConfig.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "Packet_Monitor_Member_Secede.h"
#include "Packet_GM_Request_Mid.h"
#include "Packet_PvPChannelInfo.h"
#include "Packet_PvPChannelUserCount.h"
#include "Packet_Item_Limit_Edition_Sell_Start.h"
#include "Packet_DBMW_Change_Char_Name.h"
#include "Packet_Monitor_Reply_Charac_Info.h"
#include "DNFMember.h"

CMemberConfig::CMemberConfig() throw()
{
}

CMemberConfig::~CMemberConfig() {}

void CMemberConfig::Load_Table(const std::string& path)
{
    int rc = Load_Txt_Table_Data(path.c_str(), 10);
    if (0 < rc && rc < 0xb)
    {
        return;
    }
    DNF_LOG_SCOPE_LINE(0x36, "./log/TableError", "Member Config Table - ReturnCode = %d\n", rc);
    throw CDNFException("CMemberConfig::Load_Table() Exception Break!");
}

bool CMemberConfig::Parse_Table(char* line, int idx)
{
    char* tokens[4];
    if (line[0] == '#')
    {
        return 0;
    }
    if (DNFFLib::ExplodeString(line, " \t\r\n\"", tokens, 3) == 3)
    {
        if (idx < 10)
        {
            ST_MemberConfig* info = &m_table[idx];
            info->m_a = atoi(tokens[0]);
            info->m_b = atoi(tokens[1]);
            info->m_c = atoi(tokens[2]);
            return 1;
        }
    }
    return 0;
}

ST_MemberConfig* CMemberConfig::GetMemberInfo()
{
    return m_table;
}

CMemberExpTbl::CMemberExpTbl() throw()
{
    m_count = 0;
    memset(m_table, 0, 0x2c);
}

CMemberExpTbl::~CMemberExpTbl() {}

// [2026-09-14 修复] ORIG df_monitor_r `CMemberExpTbl::Load_Table`（0x175 字节，
//   func_size_audit.py 扫出本地只有 0x5 的空壳）。ORIG 逐段实证：
//     rc = Load_Txt_Table_Data(path.c_str(), 0xb)     ← [esp+8]=0xb
//     m_count = (unsigned char)rc                      ← `mov BYTE PTR [eax+4],dl`（+4 就是 m_count）
//     `test al,al; je fail` 然后 `cmp al,0xb; jbe 成功` ⇒ 等价于 `0 < m_count && m_count <= 0xb`
//     fail: DNF_LOG_SCOPE_LINE(0x69, "./log/TableError",
//                               "Member Exp Table - ReturnCode = %d\n"(0x811fbc0), <m_count>)
//           throw CDNFException("CMemberConfig::Load_Table() Exception Break!"(0x811fb90))
//   注意异常串里写的是 **CMemberConfig**（ORIG 原版如此，逐字保留）。
void CMemberExpTbl::Load_Table(const std::string& path)
{
    m_count = (unsigned char)Load_Txt_Table_Data(path.c_str(), 0xb);
    if (m_count != 0 && m_count <= 0xb)
    {
        return;
    }
    DNF_LOG_SCOPE_LINE(0x69, "./log/TableError",
                       "Member Exp Table - ReturnCode = %d\n", (unsigned int)m_count);
    throw CDNFException("CMemberConfig::Load_Table() Exception Break!");
}

// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-MON-DIFF-0016 已消除）| CMemberExpTbl::Parse_Table
bool CMemberExpTbl::Parse_Table(char* line, int idx)
{
    if (line[0] == '#')
    {
        return 0;
    }
    char* tokens[2];
    if (DNFFLib::ExplodeString(line, "\t\"", tokens, 1) == 1 && tokens[1] == 0)
    {
        if (idx < 0xb)
        {
            m_table[idx] = atoi(tokens[0]);
            return 1;
        }
    }
    return 0;
}

// [DNF-NONIDENTICAL] DNF-MON-DIFF-0017 | monitor | 与ORIG差异=DIFF | CMemberExpTbl::GetMemberExpLevel | 详见 function_reports/monitor/_ZN13CMemberExpTbl17GetMemberExpLevelEj.md
int CMemberExpTbl::GetMemberExpLevel(unsigned int exp)
{
    // [P-DIFF] 唯一残差：我方 `m_count - 1`（unsigned char 算术）多一条冗余 `movzbl %al,%eax`
    // （ORIG 为 movzbl+sub 单扩展）。已验证该差异对 4.1.2-52 / 4.4.4-13 / 4.4.6-3 /
    // 4.4.7-3 / 4.4.7-23 / 重建 4.4.7 六个后端与 6 种源码形态（含 (int)/(unsigned int)/-1u/位域）
    // 全部不变 ⇒ 属未知前端差异，暂无法消除。
    char* p = (char*)m_table;
    int local_c = m_count - 1;
    int local_8 = 1;
    if (exp >= (unsigned int)m_table[local_c])
    {
        return m_count - 1;
    }
    if (exp == 0)
    {
        return local_8;
    }
    while (local_c-- != 0)
    {
        if (exp <= *(unsigned int*)p || *(unsigned int*)(p + 4) < exp)
        {
            local_8 = local_8 + 1;
            p = p + 4;
        }
        else
        {
            break;
        }
    }
    return local_8;
}

unsigned int CMemberExpTbl::GetMaxMemberExp()
{
    return m_table[m_count - 1];
}

unsigned char CMemberExpTbl::IsMemberExpLevelUp(unsigned int exp)
{
    char* p = (char*)m_table;
    unsigned int count = (unsigned int)m_count;
    if (exp == 1)
    {
        return 0;
    }
    if (exp >= (unsigned int)m_table[(unsigned int)m_count - 1])
    {
        return 1;
    }
    count = count - 1;
    p = p + 4;
    while (count-- != 0)
    {
        if (exp - 1 == *(unsigned int*)p)
        {
            return 1;
        }
        if (exp < *(unsigned int*)p)
        {
            break;
        }
        p = p + 4;
    }
    return 0;
}

void CMemberExpTbl::GetMemberExpLevel(unsigned int exp, unsigned int& lo, unsigned int& hi,
                                      unsigned char& lv)
{
    char* p = (char*)m_table;
    int local_c = (int)m_count - 1;
    int l = 1;
    if (exp == 0)
    {
        lo = *(unsigned int*)p;
        hi = *(unsigned int*)(p + 4);
        lv = (unsigned char)l;
    }
    else
    {
        while (local_c-- != 0)
        {
            if (*(unsigned int*)p < exp && exp <= *(unsigned int*)(p + 4))
            {
                lo = *(unsigned int*)p;
                hi = *(unsigned int*)(p + 4);
                lv = (unsigned char)l;
                return;
            }
            l++;
            p += 4;
        }
    }
}

ST_MemberConfig::ST_MemberConfig()
{
    m_a = 0;
    m_b = 0;
    m_c = 0;
}
