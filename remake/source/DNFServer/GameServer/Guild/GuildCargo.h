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
struct Packet_DB_Insert_Guild_Cargo_History;
struct Packet_Notice_Guild_Cargo;
struct STGuildCargoDBInfo;
struct STGuildCargoLog;

// from GuildDomain.h
enum ENUM_GUILD_CARGO_BEHAVIOR {};

// ---- from GuildDomain.h（自 DNFGuild.h 移入，供 STGuildCargoDBInfo 使用真实数组成员）----
struct RandomOptionField
{
    void reset();
    char m_data[3];
};

struct RandomOptionSeed
{
    void reset();
    char m_data[1];
};

#pragma pack(push,1)
struct RandomOption
{
    void reset();
    unsigned int m_opt0;     // +0（随机选项值）
    unsigned int m_opt1;     // +4
    unsigned int m_opt2;     // +8
    unsigned short m_opt3;   // +0xc
};
#pragma pack(pop)

struct UpgradeSeparateInfo
{
    UpgradeSeparateInfo();
    void reset();
    unsigned char GetUpgradeSeparate() const;
    char m_data[1];
};

#pragma pack(push,1)
struct ReservedCapacity
{
    ReservedCapacity();
    void reset();
    unsigned int m_field0;   // +0
    unsigned int m_field4;   // +4
    unsigned char m_field8;  // +8
};
#pragma pack(pop)

#pragma pack(push,1)
class DnfItemInfo
{
public:
    DnfItemInfo();
    void reset();
    unsigned char m_seal;          // +0
    unsigned int m_itemId;         // +1
    unsigned char m_attr;          // +5
    unsigned int m_addInfo;        // +6
    unsigned short m_endurance;    // +0xa
    unsigned int m_extendInfo;     // +0xc
    unsigned char m_abilityType;   // +0x10
    unsigned short m_abilityValue; // +0x11
    char m_pad[0x1d - 0x13];       // +0x13..0x1c
    RandomOption m_opt;            // +0x1d（0xe 字节）
    UpgradeSeparateInfo m_up;      // +0x2b
    ReservedCapacity m_res;        // +0x2c
};
#pragma pack(pop)

// from GuildDomain.h
struct STGuildCargoDBInfo
{
    STGuildCargoDBInfo();
    DnfItemInfo m_items[0x78];   // +0（0x18d8 字节）
    unsigned int m_capacity;     // +0x18d8（总大小 0x18dc）
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
    bool IsValidSlot(int slot);
    void SetGuildInfo(int guildKey);
    unsigned char IsLoadComplete();
    int CalcItemCount();
    bool IsEmpty();
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
    STGuildCargoDBInfo m_info;      // +0（0x18dc）
    int m_itemCount;                // +0x18dc
    int m_guildKey;                 // +0x18e0
    unsigned char m_load;           // +0x18e4
    char m_pad[3];                  // +0x18e5
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
