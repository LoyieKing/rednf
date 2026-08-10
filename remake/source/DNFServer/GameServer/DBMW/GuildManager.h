#ifndef DBMW_GUILDMANAGER_H_
#define DBMW_GUILDMANAGER_H_

#include "DBMWCommon.h"


class CGuildManager
{
public:
    CGuildManager();
    ~CGuildManager();
    void clear();
    void clearGuildWar();
    std::vector<std::pair<unsigned int, STGuildWarRankInfo*> >* GetVtGuildWarRankInfo();
    STGuildMemberProxy* GetArrayTempGuildMemberList();
    STGuildWarRankInfo* GetFirstRankGuild();
    void insertGuildWar(STGuildWarRankInfo* info);
    unsigned int getFirstGuildOfGuildWar();
    char InitGuildWarPointList();
    char rank();
    char rankGuildWar();
    void insert(STGuildRankInfo* info);
    std::vector<std::pair<unsigned int, STGuildRankInfo*> >* GetVtGuildRankInfo();
    void printGuildWarRank();
    void GetGuildWarEnterableRank(ST_Guild_War_Info* info);
    std::vector<std::pair<unsigned int, STGuildRankInfo*> > m_rankList;   // +0
    std::vector<std::pair<unsigned int, STGuildWarRankInfo*> > m_warRankList; // +0xc
    STGuildMemberProxy m_members[0x12c];  // +0x18（0x12c×0x41 = 0x4c2c，至 0x4c44）
};

#endif  // DBMW_GUILDMANAGER_H_
