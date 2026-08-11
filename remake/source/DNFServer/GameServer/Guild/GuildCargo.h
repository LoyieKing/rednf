#ifndef GUILDCARGO_H_
#define GUILDCARGO_H_

#include <map>
#include <queue>
#include <deque>
#include <list>
#include <algorithm>
#include <string>
#include <vector>
#include "Thread.h"
#include "PacketHeader.h"
#include "DNFFunctionLib.h"

#include "PacketHeader.h"

class CGuildCargo;
class CServerHandler;
class CUser;
class DnfItemInfo;
struct Packet_DB_Insert_Guild_Cargo_History;
struct Packet_Notice_Guild_Cargo;
class RandomOption;
struct STGuildCargoDBInfo;
struct STGuildCargoLog;

// from GuildDomain.h
enum ENUM_GUILD_CARGO_BEHAVIOR {};

// from GuildDomain.h
struct STGuildCargoDBInfo
{
    STGuildCargoDBInfo();
    char m_data[0x18dc];
};

// from GuildDomain.h
class CGuildCargo
{
public:
    CGuildCargo();
    ~CGuildCargo();
    void Reset();
    int GetCapacity();
    void SetCapacity(unsigned int capacity);
    int IsValidSlot(int slot);
    void SetGuildInfo(int guildKey);
    unsigned char IsLoadComplete();
    int CalcItemCount();
    int IsEmpty();
    void* GetGuildCargoDBInfo();
    int GetSpecificItemSlot(int itemId);
    void PrintCargo(ENUM_GUILD_CARGO_BEHAVIOR behavior);
    static const char* PrintDnfItemInfo(DnfItemInfo& info);
    int AddItem(DnfItemInfo& info, int slot, int count);
    int InsertItem(DnfItemInfo& info, int& slot, int count, unsigned char a, int b);
    int DeleteItem(DnfItemInfo& info, int slot, int count, unsigned char a, int b, int c);
    int MoveItem(DnfItemInfo& info, DnfItemInfo& info2, int a, int b, int c, int d, int e);
    int CheckInsertItem(int itemId, int count, int slot, unsigned char stackable, int maxStack);
    void SendGuildCargo(CUser* user);
    void GetHistory(STGuildCargoLog* out);
    void InsertHistory(ENUM_GUILD_CARGO_BEHAVIOR behavior, int slot, const char* name,
                       int count, int param, const RandomOption* option);
    void SendHistoryToDBMW(CServerHandler* handler, ENUM_GUILD_CARGO_BEHAVIOR behavior,
                           int slot, const char* name, int count, int param,
                           DnfItemInfo& info);
    void SendGuildCargoToDBMW(CServerHandler* handler, int slot);
    void SetGuildCargoHistory(unsigned int idx, STGuildCargoLog* log);
    void SetGuildCargoDBInfo(STGuildCargoDBInfo& info);
    STGuildCargoDBInfo m_info;     // +0（0x18dc；+0x18d8 的 u32 与容量字段别名）
    char m_tail[0x18e8 - 0x18dc];  // +0x18dc（m_field18dc/m_guildKey/m_load/pad）
    std::deque<STGuildCargoLog> m_history;   // +0x18e8
};

// from GuildPackets.h
#pragma pack(push,1)
class Packet_Notice_Guild_Cargo : public PacketHeader {
public:
    Packet_Notice_Guild_Cargo();
    unsigned int m_a;              // +0xa
    unsigned int m_b;              // +0xe
    STGuildCargoDBInfo m_cargo;    // +0x12
};
#pragma pack(pop)

#endif
