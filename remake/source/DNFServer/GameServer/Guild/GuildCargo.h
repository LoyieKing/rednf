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

// ---- RandomOptionField / RandomOptionSeed / RandomOption / UpgradeSeparateInfo /
//      ReservedCapacity / DnfItemInfo 的权威定义在 Game 域 ----
// 这里曾为「单独编译 guild」复制过一份同名副本（尺寸/偏移与 Game 域完全等价，
// 仅成员名不同），与 Packet_Guild_Guild_Cargo_Push_Item.h 同时 include 时触发
// redefinition（GuildCargo.h:29 vs Game/Inven_Item.h:38 等）。按工程口径删除
// 遗留副本，改为 include 权威头（DnfItemInfo.h 内已 include Inven_Item.h）。
// 数据成员偏移依据（ORIG df_guild_r，二者一致）：
//   RandomOption 0xe（+0/+3/+6 三字段、+9 seed、+0xa 字段、+0xd seed）
//   UpgradeSeparateInfo 1、ReservedCapacity 9、DnfItemInfo 0x35（opt@+0x1d）
#include "../Game/DnfItemInfo.h"

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
