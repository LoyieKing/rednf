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

// from GuildDomain.h（自 DNFGuild.h 移入，供 STGuildBoardDBInfo 使用真实成员；
// DNFGuild.h 在包含本头文件后再用该类型，定义顺序满足）
// ORIG 反汇编 ctor 校验：+0=0xff, +1=0xff, +2=0, +3..0x20 为 0（memset 0x1e）。
struct STGuildMemberCharacData
{
    STGuildMemberCharacData();
    unsigned char m_field0;   // +0
    unsigned char m_field1;   // +1
    unsigned char m_masterFlag; // +2（setGuildBoardData 中 IsGuildMaster 置 1）
    char m_name[0x1e];        // +3（总大小 0x21）
} __attribute__((packed));

#pragma pack(push,1)
struct STGuildBoardDBInfo
{
    STGuildBoardDBInfo();
    char m_memo[0x78];            // +0（board 留言文本，printGuildBoard 以 %s 输出）
    unsigned int m_guildKey;      // +0x78（推断：board 所属 guild；printGuildBoard 以 %d 输出）
    unsigned int m_boardId;       // +0x7c（m_board map 键，排序索引）
    unsigned int m_writerCharNo;  // +0x80（IsGuildMaster 判断写入者）
    STGuildMemberCharacData m_char; // +0x84（0x21 字节）
};
#pragma pack(pop)

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
    bool m_webAction;            // +4
    char m_pad5[3];
    int m_loadState;         // +8
    std::map<unsigned int, STGuildBoardDBInfo, std::greater<unsigned int> > m_board;  // +0xc
    unsigned int m_dbAccessTime;   // +0x24
    // CGuild+0x66ec：今日成员块（9×uint + ushort + byte = 0x27）
    struct __attribute__((packed)) TodayMemberBlock
    {
        unsigned int m_charNo;    // +0
        unsigned int m_name0;     // +4
        unsigned int m_name1;
        unsigned int m_name2;
        unsigned int m_name3;
        unsigned int m_name4;
        unsigned int m_name5;
        unsigned int m_name6;
        unsigned int m_name7;     // +0x20
        unsigned short m_field24; // +0x24
        unsigned char m_field26;  // +0x26
    };
    TodayMemberBlock m_today;      // +0x28（绝对 +0x66ec）
    char m_pad4f[5];               // 保持 CGuildBoard 0x54
};

#endif
