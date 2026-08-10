#ifndef GUILDBOARD_H_
#define GUILDBOARD_H_

#include <map>
#include <queue>
#include <deque>
#include <list>
#include <algorithm>
#include <string>
#include <vector>
#include "Thread.h"

#include "PacketHeader.h"

class CGuild;
class CGuildBoard;
class CServerHandler;
class CUser;
struct STGuildBoardDBInfo;

// from GuildDomain.h
enum ENUM_DB_LOAD_STATE {};

// from GuildDomain.h
struct STGuildBoardDBInfo
{
    STGuildBoardDBInfo();
    char m_data[0xa5];
};

// from GuildDomain.h
class CGuildBoard
{
public:
    CGuildBoard();
    ~CGuildBoard();
    void reset();
    void printGuildBoard();
    void setGuildBoardData(unsigned int a, unsigned int b, CGuild* guild, int c,
                           STGuildBoardDBInfo* info);
    void sendGuildBoardData(unsigned int a, unsigned int b, unsigned int c, CUser* user);
    void clearGuildBoardData();
    void deleteGuildBoardData(unsigned int a, unsigned int b, unsigned int c);
    bool isGuildBoardDBAccess();
    bool isWebGuildBoardAction();
    void setGuildBoardDBAccess();
    void setWebGuildBoardAction(bool flag);
    int getGuildBoardDBLoadState();
    void setGuildBoardDBLoadState(ENUM_DB_LOAD_STATE state);
    void sendMessageToDBMW_GuildFund(CServerHandler* handler, int fund, CUser* user);
    void sendMessageToDBMW_GuildLevelUP(CServerHandler* handler, int level, CUser* user);
    void sendMessageToDBMW_GuildAttendance(CServerHandler* handler, int a, int b,
                                           unsigned int c, unsigned int d);
    void sendMessageToDBMW_GuildMasterChanging(CServerHandler* handler, CUser* user,
                                               const char* name);
    char m_data[0x54];   // ORIG 内嵌于 CGuild+0x66c4，sizeof(CGuild)=0x6718
};

#endif
