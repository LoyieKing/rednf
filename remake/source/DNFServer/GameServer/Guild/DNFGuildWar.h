#ifndef DNFGUILDWAR_H_
#define DNFGUILDWAR_H_

#include <map>
#include <queue>
#include <deque>
#include <list>
#include <algorithm>
#include <string>
#include <vector>
#include "Thread.h"
#include "PacketHeader.h"

#include "PacketHeader.h"
#include "DNFGuildWar.h"

class CApplication;
class CGuildWar;
struct Packet_Notice_DB_Save_Guild_War_Point;
struct STGuildWarInfo;
struct ST_Guild_War_Rank_Info;

// from GuildDomain.h
struct STGuildWarInfo
{
    unsigned int m_guildKey;  // +0
    unsigned int m_point;     // +4
    union {
        char m_data[0x1c];        // +8 .. +0x23（总 0x24，Swap<STGuildWarInfo> 反编译验证）
        unsigned int m_field[7];  // m_field[6] @ +0x20
    };
};

// from GuildDomain.h
class CGuildWar
{
public:
    CGuildWar();
    ~CGuildWar();
    void DBSaveProcess(CApplication* app);
    bool IsGuildWarEnterableGuild(unsigned int guildId);
    void SetGuildWarEvent(bool flag, unsigned char param);
    bool IsGuildWarEventOn();
    void InitGuildWarInfo();
    void Clear_VtGuildWarInfo();
    void AddGuildWarPoint(unsigned int guildId, int point);
    bool Rank();
    void RankProcess();
    int SameRankWork();
    void printGuildWarRank();
    void GetGuildWarInfo(unsigned int* a, unsigned int* b, unsigned short* c);
    void GetGuildWarInfo(ST_Guild_War_Rank_Info* info);
    int Find_GuildWarInfo(unsigned int guildId);
    void Insert_GuildWarInfo(STGuildWarInfo* info);
    bool GetGuildWarInfoDBSave(unsigned int* a, unsigned int* b);
    std::vector<std::pair<unsigned int, STGuildWarInfo*> > m_vtGuildWarInfo;  // +0（0xc）
    bool m_bEventOn;           // +0xc
    char m_bRankCnt;           // +0xd
    char m_bSaveCnt;           // +0xe
    unsigned char m_bParam;    // +0xf
    char m_bRankWorked;        // +0x10
    char m_pad[3];             // +0x11
};

// from GuildPackets.h
class Packet_Notice_DB_Save_Guild_War_Point : public PacketHeader {
public:
    Packet_Notice_DB_Save_Guild_War_Point();
    char m_data[0x51];
};

#endif
