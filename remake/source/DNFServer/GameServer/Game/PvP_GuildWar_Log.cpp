#include "LogManager.h"
// ============================================================================
// df_game_r PvP_GuildWar_Log（G2-4 公会战 PvP 日志，0x68 字节）
// 逐函数对照 docs/class_func_reports/PvP_GuildWar_Log.md 与 ORIG 反汇编实现；
// 布局：+0x00 char m_guildCnt、+0x04 int m_startTime、+0x08 STPvPGuild[8]（0xc 每项）。
// 注意：PvP_deps.h 桩签名 SetpvpStartTime(long long) 与 ORIG mangling El（long）
// 不符，本 TU 以 ORIG 为准（long）；STPvPGuild ctor 一并实现（ORIG 085df7d0 弱）。
// ============================================================================

#include <cstdio>
#include <cstring>

#include "CSystemTime.h"
#include "CUser.h"           // CUser / CUserCharacInfo（基类，offset 0）
#include "CEnvironment.h"    // G_CEnvironment() / get_file_name()

// ---- CMyFileLog（ORIG ctor 0810786c / operator() 08107898，外部定义）----
class CMyFileLog
{
public:
    CMyFileLog(const char* file, int line);
    void operator()(const char* fmt, ...);

    char m_pad[8];
};

// ---- STPvPGuild（0xc：uint key + int point + char count；ctor 085df7d0）----
class STPvPGuild
{
public:
    STPvPGuild();

    unsigned int m_guildKey;  // +0x00
    int m_point;              // +0x04
    char m_count;             // +0x08
};

STPvPGuild::STPvPGuild()
{
    m_guildKey = 0;
    m_point = 0;
    m_count = 0;
}

// ---- PvP_GuildWar_Log（0x68）----
class PvP_GuildWar_Log
{
public:
    PvP_GuildWar_Log();
    ~PvP_GuildWar_Log();
    void Clear();
    void SetpvpStartTime(long t);
    int InsertPvPGuild(unsigned int guildKey, int point);
    void WriteGuildWarPvPLog(int roomIdx, char mode, CUser** users);

    char m_guildCnt;             // +0x00
    int m_startTime;             // +0x04
    STPvPGuild m_guilds[8];      // +0x08（0x60）
};

PvP_GuildWar_Log::PvP_GuildWar_Log()
    : m_startTime(0)
{
}

PvP_GuildWar_Log::~PvP_GuildWar_Log()
{
    Clear();
}

void PvP_GuildWar_Log::Clear()
{
    m_guildCnt = 0;
    m_startTime = 0;
    memset(m_guilds, 0, sizeof(m_guilds));
}

void PvP_GuildWar_Log::SetpvpStartTime(long t)
{
    m_startTime = t;
}

int PvP_GuildWar_Log::InsertPvPGuild(unsigned int guildKey, int point)
{
    for (int i = 0; i <= 7; ++i)
    {
        if (m_guilds[i].m_guildKey == 0)
        {
            m_guilds[i].m_guildKey = guildKey;
            m_guilds[i].m_point = point;
            m_guilds[i].m_count = 1;
            ++m_guildCnt;
            return 1;
        }
        if (m_guilds[i].m_guildKey == guildKey)
        {
            m_guilds[i].m_point += point;
            ++m_guilds[i].m_count;
            return 1;
        }
    }
    return 0;
}

void PvP_GuildWar_Log::WriteGuildWarPvPLog(int roomIdx, char mode, CUser** users)
{
    char logBuf[2048];
    char logPath[256];
    int i;
    int j;
    int k;

    for (i = 0; i < 8; ++i)
    {
        if (users[i] != 0)
        {
            int point = users[i]->get_guildwar_point_per_pvpplay();
            unsigned int guildKey =
                ((CUserCharacInfo*)users[i])->get_charac_guildkey();
            InsertPvPGuild(guildKey, point);
        }
    }
    memset(logBuf, 0, sizeof(logBuf));
    sprintf(logBuf, "Room_Idx:%d\tPvP_Mode:%d\tElasp_Time:%d\tG_Cnt:%d\n",
            roomIdx, (int)mode,
            GlobalData::s_systemTime_.getCurSec() - m_startTime,            (unsigned char)m_guildCnt);
    if ((unsigned char)m_guildCnt < 9)
    {
        for (j = 0; j < (unsigned char)m_guildCnt; ++j)
        {
            if (m_guilds[j].m_guildKey != 0)
            {
                sprintf(logBuf, "%s->\t\tG_Key:%d\tPoint:%d\tM_Cnt:%d\n", logBuf,
                        m_guilds[j].m_guildKey, m_guilds[j].m_point,
                        (unsigned char)m_guilds[j].m_count);
            }
        }
        memset(logPath, 0, sizeof(logPath));
        sprintf(logPath, "./log/%s/GuildWarResult",
                G_CEnvironment()->get_file_name());
        CMyFileLog fileLog(
            "void PvP_GuildWar_Log::WriteGuildWarPvPLog(int, char, CUser**)",
            0x12dc);
        fileLog(logPath, "%s", logBuf);
        Clear();
        for (k = 0; k < 8; ++k)
        {
            if (users[k] != 0)
            {
                users[k]->set_guildwar_point_per_pvpplay(0);
            }
        }
    }
    else
    {
        LogManager::logFormat(
            1, "pvp.cpp",
            "void PvP_GuildWar_Log::WriteGuildWarPvPLog(int, char, CUser**)",
            0x12cc,
            "PvP_GuildWar_Log::WriteGuildWarPvPLog() m_guildCnt(%d) > MAX_PVP_PLAYER(8) ",
            (unsigned char)m_guildCnt);
    }
}
