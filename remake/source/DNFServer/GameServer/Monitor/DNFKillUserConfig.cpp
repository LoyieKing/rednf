// df_monitor_r — DNFKillUserConfig（从 MonitorTypes/App/Table 拆分）
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

#include "DNFKillUserConfig.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "Packet_Monitor_Member_Secede.h"
#include "Packet_GM_Request_Mid.h"
#include "Packet_PvPChannelInfo.h"
#include "Packet_PvPChannelUserCount.h"
#include "Packet_Item_Limit_Edition_Sell_Start.h"
#include "Packet_DBMW_Change_Char_Name.h"
#include "Packet_Monitor_Reply_Charac_Info.h"

CKillUSRConfig::CKillUSRConfig() {}

CKillUSRConfig::~CKillUSRConfig() { Clear_Table(); }

// [2026-09-14 修复] ORIG df_monitor_r `CKillUSRConfig::Load_Table`（0x15f 字节，
//   func_size_audit.py 扫出本地只有 0x5 的空壳）。ORIG 逐段实证：
//     rc = Load_Txt_Table_Data(path.c_str(), 0x64)     ← [esp+8]=0x64
//     `cmp [ebp-0x1c],0; jle fail` 然后 `cmp [ebp-0x1c],0x64; jle 成功`
//       ⇒ 等价于 `0 < rc && rc <= 0x64`
//     fail: DNF_LOG_SCOPE_LINE(0x5b, "./log/TableError",
//                               "Kill USR Config Table - ReturnCode = %d\n"(0x811f558), rc)
//           throw CDNFException("CKillUSRConfig::Load_Setup_Table() Exception break!"(0x811f594))
void CKillUSRConfig::Load_Table(const std::string& path)
{
    int rc = Load_Txt_Table_Data(path.c_str(), 0x64);
    if (0 < rc && rc <= 0x64)
    {
        return;
    }
    DNF_LOG_SCOPE_LINE(0x5b, "./log/TableError",
                       "Kill USR Config Table - ReturnCode = %d\n", rc);
    throw CDNFException("CKillUSRConfig::Load_Setup_Table() Exception break!");
}

bool CKillUSRConfig::Parse_Table(char* line, int idx)
{
    if (line[0] == '#')
    {
        return 0;
    }
    #if remake_verify
    tok0;
    tok1;
    tok2;
    tok3;
    #else
    char* tok_arr[5] = {0};   // DNF-FIX: ExplodeString 会写 out[maxCount]，需要 maxCount + 1 个槽
    char*& tok0 = tok_arr[0];
    char*& tok1 = tok_arr[1];
    char*& tok2 = tok_arr[2];
    char*& tok3 = tok_arr[3];
    #endif
    // 死局部：与 ORIG 栈布局对齐（tok 槽位 -0x1c..-0x10 / cfg@-0x20），
    // 同族 coserver 还原已验证该模式。
    int n;
    (void)n;
    if (DNFFLib::ExplodeString(line, " \t\r\n\"", &tok0, 4) == 4)
    {
        ST_KillUSRConfig* cfg = new (std::nothrow) ST_KillUSRConfig;
        if (cfg != 0)
        {
            cfg->m_type = atoi(tok0);
            cfg->m_val = atoi(tok1);
            cfg->m_b = atoi(tok2);
            cfg->m_c = atoi(tok3);
            m_vec.push_back(cfg);
            return 1;
        }
    }
    return 0;
}

void CKillUSRConfig::Clear_Table()
{
    if (!m_vec.empty())
    {
        for (std::vector<ST_KillUSRConfig*>::iterator it = m_vec.begin();
             it != m_vec.end(); ++it)
        {
            ST_KillUSRConfig* p = *it;
            ::operator delete(p);
            p = 0;
        }
        m_vec.clear();
    }
}

std::vector<ST_KillUSRConfig*>* CKillUSRConfig::GetInfo() const
{
    return const_cast<std::vector<ST_KillUSRConfig*>*>(&m_vec);
}

ST_KillUSRConfig::ST_KillUSRConfig()
{
    m_type = 0;
    m_val = 0;
    m_b = 0;
    m_c = 0;
}
