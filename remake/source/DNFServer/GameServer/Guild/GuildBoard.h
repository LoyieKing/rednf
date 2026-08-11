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
    unsigned char isWebGuildBoardAction();
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
    unsigned int m_field0;   // +0
    unsigned char m_webAction;   // +4
    char m_pad5[3];
    int m_loadState;         // +8
    std::map<unsigned int, STGuildBoardDBInfo, std::greater<unsigned int> > m_board;  // +0xc
    unsigned int m_dbAccessTime;   // +0x24
    char m_rest[0x54 - 0x28];      // +0x28
};

#endif
