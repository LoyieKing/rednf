#ifndef GAME_PVPTYPES_H_
#define GAME_PVPTYPES_H_

// ============================================================================
// df_game_r PvP 链（PvP_Room / WarRoom / BlueMarble）依赖子对象声明（权威聚合头）。
// 本头为原 PvP_deps.h 的权威继承者（§7 单声明点、约束2 删除所有 *_deps.h）；
// PvP_Room.h / WarRoom.h / BlueMarble.h / MatchingSystem.cpp 经本头引入子对象。
// 尺寸依据 ORIG 三类的构造/析构推导：
//   PvP_GuildWar_Log 0x68（+0xc4）   cElection<int,8,8> 0x100（+0x12c）
//   PvpUserTable 0x34c（+0x268）     PvpResultRecvFlag 0x8（+0x5b4/+0x5bc）
//   CRelayBattleMgr 0x7c（+0x620）   CDeathMatchBattleMgr 0x44（+0x69c）
//   CMatchingSystem 0x18（+0x6e0）   WarField 0xf8（WarRoom +0x34）
//   CMTRand 0x9c8（BlueMarble new[4]）BuffInfo 0xc / TileIndexInfo 0x8
//   BlueMarbleUserInfo 0x28          BlueMarbleScriptManager（getMap 宿主）
// 方法为最小桩（real 符号由各专属 TU 提供），后续对应批次 TU 交付后覆盖。
// 权威子类（CRelayBattleMgr/CDeathMatchBattleMgr/BlueMarble*/WarField）走各自权威头：
//   BlueMarbleMapScript.h / BlueMarbleTileScript.h / BlueMarbleInfoScript.h /
//   BlueMarbleUserInfo.h / WarField.h / CRelayBattleMgr.h / CDeathMatchBattleMgr.h
// ============================================================================

#include <cstddef>
#include <cstring>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <map>

#include "BlueMarbleMapScript.h"
#include "BlueMarbleTileScript.h"
#include "BlueMarbleInfoScript.h"   // 权威 BlueMarbleInfoScript + 数据对象（§7，替代本地 pad 版）
#include "BlueMarbleUserInfo.h"     // 权威 BlueMarbleUserInfo + BuffInfo + TileIndexInfo（§7）
// ---- CRelayBattleMgr / CDeathMatchBattleMgr（权威头，§7） ----
#include "CRelayBattleMgr.h"
#include "CDeathMatchBattleMgr.h"
// ---- WarField（权威头，WarRoom +0x34；map_item/map_monster/MapInfo 唯一声明点） ----
#include "WarField.h"
#include "CUser.h"
#include "CParty.h"
#include "GameTypes.h"
#include "GameEnums.h"

// Shared enums are provided by GameEnums.h.
#ifndef DNF_ENUM_SEAT_STATE_DEFINED
#define DNF_ENUM_SEAT_STATE_DEFINED
enum ENUM_SEAT_STATE
{
    ENUM_SEAT_STATE_0xfe = 0xfe,
    ENUM_SEAT_STATE_0xff = 0xff
};
#endif

// ---- BlueMarble 命名空间枚举（mangling 必须保持 N..1TE / 4TypeE 形态） ----
namespace BlueMarbleState
{
enum T
{
    T_0 = 0
};
}


namespace BlueMarbleBuffType
{
enum T
{
    T_0 = 0
};
}

// ---- PVP_ROOM_INFO（get_room_info 目标结构） ----
struct PVP_ROOM_INFO
{
    std::string title;   // +0x00
    char byte4;          // +0x04
    short short6;        // +0x06
    char byte8;          // +0x08
    std::string passwd;  // +0x0c
};

// ---- 子对象前向声明 ----
class PvP_Room;

// ---- PvP_GuildWar_Log（0x68） ----
class PvP_GuildWar_Log
{
public:
    PvP_GuildWar_Log();
    ~PvP_GuildWar_Log();
    void Clear();
    void SetpvpStartTime(long t);
    void WriteGuildWarPvPLog(int roomIdx, char mode, CUser** users);
private:
    char m_pad[0x68];
};

// ---- cElection<int,8,8>（0x100） ----
template <class T, int V, int C>
class cElection
{
public:
    cElection();
    int GetElected() const;
    void Vote(int a, int b);
    void CancelVoter(int a);
    void CancelCandidate(int a);
    int m_votes[C];
private:
    char m_pad[0x100 - sizeof(m_votes)];
};

// ---- PvpUserTable（0x34c） ----
class PvpUserTable
{
public:
    PvpUserTable();
    ~PvpUserTable();
    void Set(int team, CUser* user);
    int GetResult(int a, bool b, int c);
    int GetWinTeam();
    void SetWinTeam(int team);
    bool IsValidTeam(int team);
    int GetLeavePenalty(int team);
    void Clear();
    void Calculate();
private:
    char m_pad[0x34c];
};

// ---- PvpResultRecvFlag（0x8） ----
class PvpResultRecvFlag
{
public:
    PvpResultRecvFlag();
    ~PvpResultRecvFlag();
    void SetRecvFlag(int flag);
    void Clear();
    bool IsRecv(int flag) const;
private:
    char m_pad[0x8];
};

// ---- IMatchingSystem / CMatchingSystem（0x18） ----
class IMatchingSystem
{
public:
    virtual ~IMatchingSystem() {}
    virtual int v08(...) = 0; virtual int v14(...) = 0; virtual int v18(...) = 0;
    virtual int v1c(...) = 0; virtual int v20(...) = 0; virtual int v28(...) = 0;
    virtual int v2c(...) = 0; virtual int v3c(...) = 0; virtual int v40(...) = 0;
    virtual int v44(...) = 0; virtual int v48(...) = 0; virtual int v4c(...) = 0;
    virtual int v50(...) = 0; virtual int v58(...) = 0; virtual int v60(...) = 0;
    virtual int v70(...) = 0; virtual int v74(...) = 0; virtual int v7c(...) = 0;
};

class IMatch;
class CMatchingSystem
{
public:
    CMatchingSystem();
    ~CMatchingSystem();
    void* GetMatchingSystem(ENUM_PVP_MATCHING_TYPE type);
private:
    IMatch* m_systems[6];   // +0x00（0x18）
};

// ---- CMTRand（0x9c8） ----
class CMTRand
{
public:
    CMTRand();
    void seed(unsigned long s);
    int randInt();
    int randInt(const unsigned long& range);
private:
    char m_pad[0x9c8];
};

// ---- BlueMarbleScriptManager（MapScript/InfoScript 宿主；权威声明点本头） ----
class BlueMarbleScriptManager
{
public:
    BlueMarbleScriptManager();
    static BlueMarbleScriptManager* getInsance();
    void clear();
    BlueMarbleMapScript* getMap(int idx);
    int getMapTotalCount();
    BlueMarbleTileScript* getTile(int zone);
    char m_prefix[0x30];
    int m_mapTotalCount;
    char m_config[0x64];
    std::vector<BlueMarbleRandomItemInfo> m_randomItems;
    std::vector<BlueMarbleRewardInfo> m_rewardItems;
    std::map<int, BlueMarbleTileScript*> m_tiles;
    std::map<int, BlueMarbleMapScript*> m_maps;
};

static_assert(sizeof(BlueMarbleRandomItemInfo) == 0x8, "BlueMarbleRandomItemInfo ABI drift");
static_assert(sizeof(BlueMarbleRewardInfo) == 0x8, "BlueMarbleRewardInfo ABI drift");
static_assert(sizeof(BlueMarbleRandomDungeonInfo) == 0xc, "BlueMarbleRandomDungeonInfo ABI drift");
struct BlueMarbleScriptConfig
{
    int m_maxUser;
    int m_minLevel;
    int m_maxLevel;
    int m_enterLimit;
    int m_field10;
    int m_diceMin;
    int m_diceRange;
    int m_field1c;
};

struct BlueMarbleHistoryMailData
{
    char m_reserved0[0x11];
    char m_title[0x28];
    int m_addInfo;
    int m_addInfo2;
    char m_reserved45[0x104];
};

static_assert(sizeof(BlueMarbleScriptConfig) == 0x20, "BlueMarbleScriptConfig ABI drift");

static_assert(offsetof(BlueMarbleScriptManager, m_mapTotalCount) == 0x30, "BlueMarbleScriptManager map offset drift");
static_assert(offsetof(BlueMarbleScriptManager, m_randomItems) == 0x98, "BlueMarbleScriptManager item offset drift");
static_assert(offsetof(BlueMarbleScriptManager, m_rewardItems) == 0xa4, "BlueMarbleScriptManager reward offset drift");

#endif  // GAME_PVPTYPES_H_