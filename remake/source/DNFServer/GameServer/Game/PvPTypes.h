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
// ---- CUser / CParty：仅以指针/引用出现在本头接口（WriteGuildWarPvPLog /
//      PvpUserTable::Set），前置声明即可。不得 include CUser.h/CParty.h：
//   本头经 CDataManager.h→CItemGeneratorMgr→CItemGenerator 进入
//   CUserCharacInfo.h 的 include 闭包，若在此展开 CUser.h，会在
//   CUserCharacInfo.h/CDataManager.h 解析中途重入 CUser.h，导致其基类
//   CUserCharacInfo 不完整（CUser.h:1457 expected class-name）。
class CUser;
class CParty;
#include "GameTypes.h"
#include "GameEnums.h"
#include "NpcScript.h"        // STScriptFileList（BlueMarble m_mapFileList/m_tileFileList 各 0x18）

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
    /* +0x08（ORIG get_room_info 0x085d92d2 movzbl 单级装载=单字节）。
     * [b28d-solo-OnlinePreliminaryMatch] uchar→bool：ORIG _MakeRoom
     * @08561050 传 create_room 第 5 参(bool) 为 `movzbl 0x10(%eax),%eax;
     * movzbl %al,%edi` 无 test/setne；4.4 对 uchar 字段隐式转 bool 恒插
     * test;setne。写点 PvP_Room.cpp:1937 赋 m_isExistPassword（0/1 语义），
     * 消费点 CFairMatchRooms.cpp:619 / CLeagueMatch.cpp:836 均按布尔用。
     * sizeof/偏移/mangled 零影响（1 字节不变）。 */
    bool byte8;           // +0x08
    std::string passwd;  // +0x0c
};

// ---- 子对象前向声明 ----
class PvP_Room;

// ---- STPvPGuild（0xc：uint guildKey + int point + char count；ORIG ctor 0x085df7d0 W） ----
class STPvPGuild
{
public:
    STPvPGuild();

    unsigned int m_guildKey;  // +0x00
    int m_point;              // +0x04
    char m_count;             // +0x08
};

// ---- PvP_GuildWar_Log（0x68；权威单声明点，实现见 PvP_GuildWar_Log.cpp） ----
class PvP_GuildWar_Log
{
public:
    PvP_GuildWar_Log();      // ORIG _ZN16PvP_GuildWar_LogC1Ev 0x085dda46
    ~PvP_GuildWar_Log();     // ORIG D1Ev 0x085dda86
    void Clear();            // ORIG 0x085dda9a
    void SetpvpStartTime(long t);  // ORIG _ZN16PvP_GuildWar_Log17SetpvpStartTimeEl 0x085df7f0 W（long，非 long long）
    int InsertPvPGuild(unsigned int guildKey, int point);  // ORIG 0x085ddd80
    void WriteGuildWarPvPLog(int roomIdx, char mode, CUser** users);  // ORIG 0x085ddad0

    char m_guildCnt;         // +0x00（Clear 置 0；ctor 不初始化，与 ORIG 一致）
    int m_startTime;         // +0x04
    STPvPGuild m_guilds[8];  // +0x08（0x60）→ sizeof 0x68
};

static_assert(sizeof(STPvPGuild) == 0xc, "STPvPGuild ABI drift");
static_assert(offsetof(PvP_GuildWar_Log, m_startTime) == 0x4, "PvP_GuildWar_Log startTime offset drift");
static_assert(offsetof(PvP_GuildWar_Log, m_guilds) == 0x8, "PvP_GuildWar_Log guilds offset drift");
static_assert(sizeof(PvP_GuildWar_Log) == 0x68, "PvP_GuildWar_Log ABI drift");

// ---- cElection<int,V,C>（ORIG 布局：cVoter m_voters[V]，每 voter 持 C 票；
//      <int,8,8>=0x100、<int,4,4>=0x40；GetElected 对票矩阵按列求和取最大） ----
template <class T, int V, int C>
class cElection
{
public:
    class cVoter
    {
    public:
        cVoter();
        void Init();          // std::fill 清零本 voter 的 C 票（ORIG 0x82beb62/0x85e0304）
        int m_votes[C];
    };

    cElection();              // 逐 voter 构造后 Init()（ORIG 0x82aafc4/0x85dff3a）
    void Init();              // 全部 voter Init（ORIG 0x82b585a/0x85e02d2）
    int GetElected() const;   // 列求和最大者；全零返回 -1（ORIG 0x85c03a8/0x85dff78）
    void Vote(int voter, int candidate);        // votes[voter][candidate]=1
    void CancelVoter(int voter);                // 该 voter 票清零
    void CancelCandidate(int candidate);        // 各 voter 该候选票清零

private:
    cVoter m_voters[V];
};

// ---- PvpUserTable（0x34c）—— 权威完整定义（ORIG 0x85d48f0 起，16 方法，
//      实现唯一归属 PvpUserTable.cpp；布局自 ORIG 构造/析构与各函数寻址推导：
//      Clear/Set/CheckDan 寻址 +0x120/+0x344、SetWinTeam/Clear 写 +0x320、
//      Calculate memset 0x100/0x120 与 minPoint +0x324、GetGhostVsTeamPoint
//      累计 +0x100、GetTeamVsTeamPoint 矩阵 (t1*8+t2)*4） ----
class PvpUserTable
{
public:
    PvpUserTable();
    ~PvpUserTable();
    void Clear();
    int  Set(int team, CUser* user);          // ORIG 返回 undefined4（int）
    // caller 侧证据：ORIG 0x085d5081 `call _ZN12PvpUserTable11IsValidTeamEi` 后接
    // `xor $0x1,%eax; test %al,%al; je` —— 只有 callee 按 bool 返回时 4.1 才出
    // xor+testb 形（技巧 1）；声明 int 时 4.4 出 `test;sete;test` 多一条。
    // callee 体 0x085d4a14 = `cmpl $0; js; cmpl $7; jg; mov $1; jmp; mov $0`
    // 常量岛形。mangled 无返回类型 ⇒ 零 ABI 风险（技巧 33）。
    bool IsValidTeam(int team);
    // b28d-54：int→bool（P248 档①）。ORIG GetExpectVal 0x085d4b4a/0x085d4b5f 两处调用点
    //   均 `call; xor $1,%eax; test %al,%al; je` ⇒ callee 返回 bool；int 恒 sete 物化。
    //   返回类型不入 mangled，调用点无需改。
    bool IsValidConvertedGrade(int grade);
    int  GetTeamUserCount(int team);
    float GetExpectVal(int grade1, int grade2, CUser* user);
    int  Calculate();
    bool GetTeamVsTeamPoint(int team1, int team2);  // SIGFIX b28c：ORIG 串
                                                    // "bool PvpUserTable::GetTeamVsTeamPoint(int, int)"
    int  GetGhostVsTeamPoint(int team, int unused);
    int  GetResult(int team, bool isWinTeam, int mode);
    int  GetLeavePenalty(int team);
    int  GradeFilter(int grade);
    void CheckDan();
    void SetWinTeam(int team);
    int  GetWinTeam();
    int  GetUserVsTeamPoint(int userIdx, int myTeam, int enemyTeam,
                            CUser* user);

    struct UserSlot
    {
        CUser* user;         // 槽内 +0x00
        unsigned char flag;  // 槽内 +0x04（胜方标记）
    };

    float m_teamVsTeam[8][8]; // +0x000 队伍间分差矩阵
    float m_teamPoint[8];     // +0x100 队伍期望分累计
    UserSlot m_users[8][8];   // +0x120 用户槽表（8 字节/项）
    int m_winTeam;            // +0x320
    float m_minPoint[8];      // +0x324 各队伍最小分（GetLeavePenalty）
    unsigned char m_dan[8];   // +0x344 丹位标记
};

static_assert(sizeof(PvpUserTable) == 0x34c, "PvpUserTable ABI drift");
static_assert(offsetof(PvpUserTable, m_users) == 0x120, "PvpUserTable users offset drift");
static_assert(offsetof(PvpUserTable, m_winTeam) == 0x320, "PvpUserTable winTeam offset drift");
static_assert(offsetof(PvpUserTable, m_minPoint) == 0x324, "PvpUserTable minPoint offset drift");
static_assert(offsetof(PvpUserTable, m_dan) == 0x344, "PvpUserTable dan offset drift");

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
    CMTRand(const unsigned long& s);   // ORIG _ZN7CMTRandC1ERKm（CMTRand.cpp 定义）
    void seed(unsigned long s);
    int randInt();
    // ORIG 真实返回 unsigned long（调用集 setb 无符号比较实证，如
    // Dispatcher_MonstercardBind::calcurate_rarity @0x081d842e `movzwl slot,%edx;
    // cmp %edx,%eax; setb`；mangled ERKm 不含返回类型，链接零影响）。
    unsigned long randInt(const unsigned long& range);
private:
    char m_pad[0x9c8];
};

// ---- BlueMarbleScriptManager（MapScript/InfoScript 宿主；权威声明点本头） ----
class BlueMarbleScriptManager
{
public:
    BlueMarbleScriptManager();
    static BlueMarbleScriptManager* getInsance();
    // ORIG（CDataManager::init 调用链）：
    //   _ZN23BlueMarbleScriptManager12initTileListEPKcS1_ / 11initMapListEPKcS1_ /
    //   18initBlueMarbleInfoEPKcS1_
    bool initTileList(const char* dir, const char* list);
    bool initMapList(const char* dir, const char* list);
    int initBlueMarbleInfo(const char* dir, const char* file);
    void clear();
    int getMapTotalCount();
    // ORIG _ZN23BlueMarbleScriptManager6getMapEi（0x088d66b6 T）：m_maps(+0xd4) find，未命中 0。
    // BlueMarble.cpp 5 处调用（setMaxBoardSize/getRandomDungeon/tileType/giveupDungeon）；
    // 真实定义在 BlueMarbleScriptManager.cpp。
    BlueMarbleMapScript* getMap(int idx);
    BlueMarbleTileScript* getTile(int zone);
    // ---- ORIG 布局（ctor 0x88d614a / clear 0x88d622c / init* 三报告实证）----
    STScriptFileList m_mapFileList;              // +0x00（ORIG STScriptFileList@this+0）
    STScriptFileList m_tileFileList;             // +0x18（ORIG STScriptFileList@this+0x18）
    BlueMarbleInfoScript m_info;                 // +0x30（0x8c，initBlueMarbleInfo 以 this+0x30 调 importScript）
    std::map<int, BlueMarbleTileScript*> m_tiles;    // +0xbc
    std::map<int, BlueMarbleMapScript*> m_maps;      // +0xd4
};  // sizeof 0xec（getInsance new(0xec) 实证）

static_assert(sizeof(BlueMarbleScriptManager) == 0xec, "BlueMarbleScriptManager size drift");

static_assert(offsetof(BlueMarbleScriptManager, m_tiles) == 0xbc, "BlueMarbleScriptManager tiles offset drift");
static_assert(offsetof(BlueMarbleScriptManager, m_maps) == 0xd4, "BlueMarbleScriptManager maps offset drift");

// 数据对象 ABI 自检（类型定义点：BlueMarbleInfoScript.h）。
static_assert(sizeof(BlueMarbleRandomItemInfo) == 0x8, "BlueMarbleRandomItemInfo ABI drift");
static_assert(sizeof(BlueMarbleRewardInfo) == 0x8, "BlueMarbleRewardInfo ABI drift");
static_assert(sizeof(BlueMarbleRandomDungeonInfo) == 0xc, "BlueMarbleRandomDungeonInfo ABI drift");

// 2026-09-05 裸偏移收尾（§4.4 单声明点）删除两个无消费者的重复视图类型：
//   · BlueMarbleScriptConfig —— 与 BlueMarbleInfoScript 前 0x20 字节
//     （m_playerMax/m_enterLevel/m_enterMaxLevel/m_enterCount/m_dieCount/
//      m_diceMin/m_diceMax/m_itemWeightTotal，+0x30..+0x4c 相对 manager）重复；
//     消费点（BlueMarble.cpp）已改走 m_pScript->m_info.<字段>。
//   · BlueMarbleHistoryMailData —— 与权威 MSG_MAILBOX_SEND（PacketTypes.h，
//     pack(1)/sizeof 0x145：m_name11@+0x11、m_itemId39@+0x39、m_extra3d@+0x3d）
//     重复，且本头无 #pragma pack，其自然对齐会把 m_addInfo 推到 +0x3c，
//     与 ORIG `mov %eax,-0x236(%ebp)`（=+0x39）矛盾 ⇒ 直接删除，
//     消费点 BlueMarble::insertItem 已改读权威 MSG_MAILBOX_SEND 字段。

// 2026-08-24 布局重构后断言（旧 m_mapTotalCount/m_randomItems/m_rewardItems
// 扁平字段已由 m_mapFileList/m_tileFileList/m_info 取代，见类内注释）。
static_assert(offsetof(BlueMarbleScriptManager, m_info) == 0x30, "BlueMarbleScriptManager info offset drift");

#endif  // GAME_PVPTYPES_H_
