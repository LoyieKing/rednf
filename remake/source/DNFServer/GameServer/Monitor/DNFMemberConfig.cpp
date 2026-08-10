// df_monitor_r — DNFMemberConfig（从 MonitorTypes/App/Table 拆分）
#include <stdio.h>
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

CMemberConfig::CMemberConfig()
{
    for (int i = 0; i < 10; i++)
    {
        m_table[i].m_a = 0;
        m_table[i].m_b = 0;
        m_table[i].m_c = 0;
    }
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

int CMemberConfig::Parse_Table(char* line, int idx)
{
    if (line[0] == '#')
    {
        return 0;
    }
    char* tokens[3];
    if (DNFFLib::ExplodeString(line, " \t\r\n\"", tokens, 3) == 3 && idx < 10)
    {
        ST_MemberConfig* info = &m_table[idx];
        info->m_a = atoi(tokens[0]);
        info->m_b = atoi(tokens[1]);
        info->m_c = atoi(tokens[2]);
        return 1;
    }
    return 0;
}

ST_MemberConfig* CMemberConfig::GetMemberInfo()
{
    return m_table;
}

CMemberExpTbl::CMemberExpTbl()
{
    *(char*)((char*)this + 4) = 0;
    memset((char*)this + 8, 0, 0x2c);
}

CMemberExpTbl::~CMemberExpTbl() {}

void CMemberExpTbl::Load_Table(const std::string& path) {}

int CMemberExpTbl::Parse_Table(char* line, int idx)
{
    if (line[0] == '#')
    {
        return 0;
    }
    char* token = 0;
    bool ok = false;
    int tmp = 0;
    if (DNFFLib::ExplodeString(line, "\t\"", &token, 1) == 1 && tmp == 0)
    {
        ok = true;
    }
    if (ok && idx < 0xb)
    {
        *(int*)((char*)this + idx * 4 + 8) = atoi(token);
        return 1;
    }
    return 0;
}

int CMemberExpTbl::GetMemberExpLevel(unsigned int exp)
{
    int local_c = (int)(unsigned char)*(char*)((char*)this + 4) - 1;
    int local_8 = 1;
    if (exp < *(unsigned int*)((char*)this + local_c * 4 + 8))
    {
        if (exp == 0)
        {
            local_8 = 1;
        }
        else
        {
            char* p = (char*)this + 8;
            while (true)
            {
                bool b = local_c != 0;
                local_c = local_c - 1;
                if (!b || !(exp <= *(unsigned int*)p || *(unsigned int*)(p + 4) < exp))
                {
                    break;
                }
                local_8 = local_8 + 1;
                p = p + 4;
            }
        }
    }
    else
    {
        local_8 = (int)(unsigned char)*(char*)((char*)this + 4) - 1;
    }
    return local_8;
}

unsigned int CMemberExpTbl::GetMaxMemberExp()
{
    return *(unsigned int*)((char*)this +
                            ((unsigned int)(unsigned char)*(char*)((char*)this + 4) - 1) * 4 +
                            8);
}

unsigned char CMemberExpTbl::IsMemberExpLevelUp(unsigned int exp)
{
    unsigned int count = (unsigned int)(unsigned char)*(char*)((char*)this + 4);
    if (exp == 1)
    {
        return 0;
    }
    if (exp < *(unsigned int*)((char*)this + (count - 1) * 4 + 8))
    {
        char* p = (char*)this + 0xc;
        while (count = count - 1, count != 0)
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
    return 1;
}

void CMemberExpTbl::GetMemberExpLevel(unsigned int exp, unsigned int& lo, unsigned int& hi,
                                      unsigned char& lv)
{
    char* p = (char*)this + 8;
    unsigned char count = *(unsigned char*)((char*)this + 4);
    unsigned char l = 1;
    if (exp == 0)
    {
        lo = *(unsigned int*)p;
        hi = *(unsigned int*)((char*)this + 0xc);
        lv = 1;
    }
    else
    {
        while (count != 0)
        {
            count--;
            if (*(unsigned int*)p < exp && exp <= *(unsigned int*)(p + 4))
            {
                lo = *(unsigned int*)p;
                hi = *(unsigned int*)(p + 4);
                lv = l;
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

