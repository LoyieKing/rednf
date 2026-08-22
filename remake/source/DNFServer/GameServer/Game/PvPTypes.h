#ifndef GAME_PVP_DEPS_H_
#define GAME_PVP_DEPS_H_

// ============================================================================
// df_game_r PvP 链（PvP_Room / WarRoom / BlueMarble）依赖子对象最小声明
// （G2-3 批次，2026-08-17）。
// 尺寸依据 ORIG 三类的构造/析构推导：
//   PvP_GuildWar_Log 0x68（+0xc4）   cElection<int,8,8> 0x100（+0x12c）
//   PvpUserTable 0x34c（+0x268）     PvpResultRecvFlag 0x8（+0x5b4/+0x5bc）
//   CRelayBattleMgr 0x7c（+0x620）   CDeathMatchBattleMgr 0x44（+0x69c）
//   CMatchingSystem 0x18（+0x6e0）   WarField 0xf8（WarRoom +0x34）
//   CMTRand 0x9c8（BlueMarble new[4]）BuffInfo 0xc / TileIndexInfo 0x8
//   BlueMarbleUserInfo 0x28          BlueMarbleScriptManager（getMap 宿主）
// 方法为最小桩（PvP_deps.cpp 定义，real 符号），后续对应批次 TU 交付后覆盖。
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

namespace BlueMarbleUserState
{
enum T
{
    T_0 = 0
};
}

namespace BlueMarbleUserGrade
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
class BlueMarbleScriptManager;
class BlueMarbleMapScript;
class BlueMarbleInfoScript;

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

// ---- CRelayBattleMgr（0x7c，PvP_Room +0x620） ----
class CRelayBattleMgr
{
public:
    void Reset();
    void SetParent(PvP_Room* room);
    void OnCreateRoom(int a);
    void OnJoinRoom(int a);
    void OnLeaveRoom(int a);
    void OnDiePlayer(int a, int b, bool c);
    void OnChangeTeam(int a, int b);
    void OnReady(int a, bool b);
    void OnRequestFight(int a);
    void OnStart();
    int GetKillCount(int a);
    int GetAllKillUserIndex(int* out);
private:
    char m_pad[0x7c];
};

// ---- CDeathMatchBattleMgr（0x44，PvP_Room +0x69c） ----
class CDeathMatchBattleMgr
{
public:
    void Reset();
    void SetParent(PvP_Room* room);
    void OnCreateRoom(int a);
    void OnStart();
    void IncreaseKillCount(int a);
    void IncreaseDeathCount(int a);
    int getKillCount(int a);
    int getDeathCount(int a);
    int getRanking(CUser** users, int* out) const;
    bool checkGameOverForSinglePlay(int a);
    bool checkWinnerForSinglePlay(CUser** users, bool* win, PvpUserTable& table);
    bool checkGameOverForTeamPlay(CUser** users, int teamCnt, unsigned char* teamWin);
    bool checkWinnerForTeamPlay(CUser** users, bool* win, unsigned char* teamWin,
                                PvpUserTable& table);
    void InsertTimerKilledUserForRevive(int roomIdx, int a);
private:
    char m_pad[0x44];
};

// ---- CMatchingSystem（0x18） ----
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
    IMatchingSystem* GetMatchingSystem(ENUM_PVP_MATCHING_TYPE type);
private:
    IMatch* m_systems[6];   // +0x00（0x18）
};

// ---- WarField（0xf8，WarRoom +0x34；复杂方法经 asm-label extern 调用） ----
class WarField;

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

// ---- BuffInfo（0xc）/ TileIndexInfo（0x8，BlueMarbleUserInfo 内嵌） ----
class BuffInfo
{
public:
    BuffInfo();
    void reset();
private:
    char m_pad[0xc];
};

class TileIndexInfo
{
public:
    TileIndexInfo();
    void reset();
private:
    char m_pad[0x8];
};

// ---- BlueMarbleUserInfo（0x28） ----
class BlueMarbleUserInfo
{
public:
    BlueMarbleUserInfo();
    ~BlueMarbleUserInfo();
    void setScript(BlueMarbleScriptManager* script);
    void setUser(CUser* user);
    void setUserState(BlueMarbleUserState::T state);
    void setGrade(BlueMarbleUserGrade::T grade);
    void setItem(unsigned int itemIdx);
    void setGold(int gold);
    void setExp(int exp);
    void setDrop(int drop);
    void setDungeon(int dungeon);
    void setBoardZone(int zone, int pos);
    void resetBlueMarbleUserInfo();
    void resetBuff();
    void resetIndex();
    CUser* getUser() const;
    int getGrade() const;
    int getItem() const;
    int getGold() const;
    int getExp() const;
    int getDrop() const;
    int getDungeon() const;
    int getBoardZone() const;
    BlueMarbleUserState::T getUserState() const;
private:
    char m_pad[0x28];
};

// ---- BlueMarble 数据对象（尺寸来自报告字段访问；脚本批次细化） ----
struct BlueMarbleRandomItemInfo
{
    int m_itemIndex;  // +0x00
    int m_weight;     // +0x04
};

struct BlueMarbleRewardInfo
{
    int m_a;  // +0x00
    int m_b;  // +0x04
};

struct BlueMarbleRandomDungeonInfo
{
    int m_minLevel;     // +0x00
    int m_maxLevel;     // +0x04
    int m_dungeonIndex; // +0x08
};

class BlueMarbleInfoScript
{
public:
    int getBossDungeonIndex(int a);
    int getRandomDungeonIndex(int a);
    int getUniqueDungeonIndex(int a);
private:
    char m_pad[0x80];
};

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

#endif  // GAME_PVP_DEPS_H_
