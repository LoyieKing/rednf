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
    bool IsLoadComplete();
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
    char m_data[0x1910];
};

// from GuildPackets.h
class Packet_DB_Insert_Guild_Cargo_History : public PacketHeader {
public:
    Packet_DB_Insert_Guild_Cargo_History();
    char m_data[0x5c];
};

// from GuildPackets.h
class Packet_Notice_Guild_Cargo : public PacketHeader {
public:
    Packet_Notice_Guild_Cargo();
    char m_data[0x18e4];
};

#endif
