#ifndef GAME_CGAMEMANAGER_H_
#define GAME_CGAMEMANAGER_H_

// df_game_r CGameManager（G2 单例对象池/房间管理器）还原（2026-08-17）。
// 布局依据 ORIG 构造（0x82930c2）/析构（0x8293d1a）逐子对象推导：
//   +0x000 Mutex(0x18)           +0x018 short（getNextUID 递增 UID）
//   +0x01c std::set<std::string> +0x034 std::set<std::string>（免 GameGuard 名单）
//   +0x04c StaticPool<CUser,600> +0x07c StaticPool<CTradeSpace,300>
//   +0x0ac StaticPool<CParty,600>+0x0dc StaticPool<PvP_Room,600>
//   +0x10c StaticPool<WarRoom,40>+0x13c StaticPool<WongWork::CDeathTower,600>
//   +0x16c StaticPool<WongWork::CBossTower,600>
//   +0x19c StaticPool<advancealtar::StageControl,600>
//   +0x1cc StaticPool<QuickParty::CQuickParty,300>
//   +0x1fc StaticPool<BlueMarble,300>
//   +0x22c std::map<uint,CUser*>（accId→CUser）+0x244 同上（备用表）
//   +0x25c hash_map<int,PvP_Room*> +0x270 hash_map<int,CParty*>
//   +0x284 hash_map<int,WarRoom*> +0x298 hash_map<int,CDeathTower*>
//   +0x2ac hash_map<int,CBossTower*> +0x2c0 hash_map<int,StageControl*>
//   +0x2d4 hash_map<int,CQuickParty*> +0x2e8 hash_map<int,BlueMarble*>
//   +0x2fc..+0x318 各管理器指针（总尺寸 0x31c）
// StaticPool<T,N> 布局 0x30（ORIG 0x82ae390 起）：
//   +0x00 CNode*（new CNode[N] 数组基） +0x04 std::queue<CNode*>(0x28)
//   +0x2c 错误码。
// CNode = { T data; int magic(0x8f21); char bUse; }（步长 sizeof(T)+8）。
// 池化对象（CTradeSpace/PvP_Room/...）权威头属后续批次，本头以尺寸垫 + 桩 ctor/dtor；
// 跨类方法调用在 CGameManager.cpp 以 asm-label extern 对齐 ORIG 符号。

#include <ext/hash_map>

#include <map>
#include <queue>
#include <set>
#include <string>
#include <utility>

#include "GameTypes.h"
#include "CUser.h"
#include "CParty.h"
#include "InterfacePacketBuf.h"
#include "PvP_Room.h"
#include "WarRoom.h"
#include "BlueMarble.h"

// ============================================================================
// StaticPool<T,N> —— ORIG 真实实现（objdump 0x82ae390 起逐函数核对）
// ============================================================================
template <class T, int N>
class StaticPool
{
public:
    struct CNode
    {
        T data;        // +0x00（sizeof(T) 字节）
        int magic;     // +sizeof(T)，恒 0x8f21
        char bUse;     // +sizeof(T)+4，引用计数
    };

    StaticPool();
    ~StaticPool();

    T* Acquire();
    T* Get(int index);
    void Free(T* p);
    int GetIndex(T* p);
    int GetLastErrorCode() const { return m_nErrorCode; }
    static int Size() { return N; }
    int Remain() { return (int)m_Queue.size(); }

private:
    CNode* m_pNode;              // +0x00
    std::queue<CNode*> m_Queue;  // +0x04
    int m_nErrorCode;            // +0x2c
};

// ============================================================================
// 池化对象最小声明（权威头属后续批次；尺寸 = ORIG 池节点数据区大小）
// ============================================================================

class CTradeSpace
{
public:
    CTradeSpace();
    ~CTradeSpace();
private:
    char m_pad[0xf70];
};

struct map_item;
struct MSG_MONSTER_DIE;

enum ENUM_TOWER_STATE
{
    TOWER_STATE_NONE = 0,
    TOWER_STATE_READY = 1,
    TOWER_STATE_PLAYING = 2,
    TOWER_STATE_CLEAR = 3,
    TOWER_STATE_REWARD = 4,
    TOWER_STATE_EPLP = 5,
};

enum ENUM_STAGE_STATE
{
    STAGE_STATE_NONE = 0,
    STAGE_STATE_START = 1,
    STAGE_STATE_FINISH = 2,
};

namespace WongWork
{
    class CDeathTower;

    class CDeathTower
    {
    public:
        class CDungeonMgr
        {
        public:
            void reset();
            bool initDungeonMgr(int dungeonIdx);
            CDungeon* getDungeon() const;
            CMap* getStageMap(int stage);
            int getEndStage();
        private:
            CDungeon* m_pDungeon;
            char m_pad4[4];
        };

        class CStage
        {
        public:
            void reset();
            void resetCurrentStage();
            bool setState(ENUM_STAGE_STATE state);
            int getState();
            bool checkClearStage();
            int killMonster(int idx, map_monster& monster);
            bool moveNextStage(unsigned int endStage);
            void onStartStage();
            void onFinishStage();
            int getStageClearTime();
            int getStageMapIndex();
            int getCurrentStage();
            bool peekItem(int idx, map_item& item);
            bool pickupItem(int idx, map_item& item);
            int dropItem(const map_item& item);
            bool consistMap(CDungeonMgr* dungeonMgr, CMap* map, CDeathTower* tower);
            void makeStagePacket(PacketGuard& packet);
        private:
            int m_currentStage;     // +0x00
            unsigned int m_startTick;  // +0x04
            unsigned int m_finishTick; // +0x08
            char m_pad0c[4];         // +0x0c (MapInfo start)
            int m_stageMapIndex;       // +0x10
            std::map<int, map_monster> m_monsterMap;  // +0x18
            char m_pad30[0x18];        // +0x30 - 0x18 = 0x18
            std::map<int, map_item> m_itemMap;  // +0x30
            int m_instIdCounter;       // +0x48
            int m_dropItemCnt;         // +0x4c
            char m_pad50[0xf8 - 0x50]; // +0x50
            int m_state;               // +0xf8
        };

        class CPlayData
        {
        public:
            void reset();
            int getStartMemberCnt();
            void makeStartMemberInfo(CParty* party);
            void resetMemberReady();
            void setMemberAlive(int idx, bool alive);
            int isMemberAlive(int idx);
            int getLastRoutedTurn();
            void setMemberReady(int idx, bool ready);
            int isMemberReady(int idx);
            bool checkEnterStartMap(int idx);
            bool checkAllMemberReady(int memberCnt);
            void addPlayTime(unsigned int time);
            unsigned int getPlayTime();
            unsigned int getLastPlayTime();
            Inven_Item* getRewardItem(int idx);
            int getRewardItemCount(int idx);
            int getRewardExp(int idx);
            int m_field0;
            char m_memberAlive[4];
            char m_memberReady[4];
            int m_allReady;
            unsigned int m_playTime;
            unsigned int m_lastPlayTime;
            char m_pad18[0xf0];     // +0x18..+0x108
            int m_rewardItemCount[4]; // +0x108
            int m_rewardExp[4];       // +0x118
            int m_lastRoutedTurn;     // +0x128
            char m_pad12c[0xa2c - 0x12c];
        };
    CDeathTower();
    ~CDeathTower();

    void _beginTowerClearProcess(bool);
    int _checkMemberDie();
    int _checkMemberReady();
    int _checkRenewMyRecord(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);
    int _checkRenewTopRecord(unsigned int, unsigned int, unsigned int, unsigned int);
    int _checkStartGameCondition(unsigned char&);
    int _checkTimerKey(TIMER_MESSAGE, unsigned int);
    void _destroy();
    int _genTimerKey(TIMER_MESSAGE);
    bool _makeDropItem(CUser*, char, int, int, map_item&);
    void _onClear(bool);
    void _onFinishDeathTower();
    void _onFinishStage();
    void _onPrepareFinishTower();
    void _onStartDeathTower();
    void _onStartStage();
    CUser* _pickupItem(CUser*, char*, int, const map_item&, bool, char, const CItem*);
    int _pickupItemMoney(CUser*, int, const map_item&, int*, int*);
    void attachParty(CParty*);
    void deathTowerCardStatistic(int, CUser*, const Inven_Item*);
    int genTimerKey(TIMER_MESSAGE);
    CDungeonMgr* getCDungeonMGr();
    CStage* getCStage();
    int getIdx();
    int getTowerState();
    int handleDieUser(CUser*);
    int handleDropItem(CUser*, int, int, char, int, int);
    int handleFinishLoading(CUser*);
    int handleKillMonster(CUser*, unsigned short, unsigned short, const MSG_MONSTER_DIE&, int*);
    int handleLeaveUser(CUser*);
    int handleMoveMap();
    int handlePickupItem(CUser*, int, bool, char);
    int handleSelectEPLPCommand(CUser*, int);
    int handleStageCommand(CUser*, unsigned char);
    int handleStartGame(int, char, ENUM_DUNGEON_TYPE);
    int handleUseStackable(CUser*, ENUM_ITEMSPACE, unsigned short);
    int onLeaveUser(CUser*);
    int onTimer(TIMER_MESSAGE, unsigned int);
    void reset();
    void setIdx(int);
    int setTowerState(ENUM_TOWER_STATE);

private:
    CParty* m_party;        // +0x00
    int m_idx;              // +0x04
    int m_towerState;       // +0x08
    CDungeonMgr m_dungeonMgr;  // +0x0c
    CStage m_stage;         // +0x14
    char m_pad14[0xfc];     // +0x14..+0x110
    CPlayData m_playData;   // +0x110
    char m_pad110[0xa2c];   // +0x110..+0xb3c
    int m_timerKeys[10];    // +0xb3c (TIMER_MESSAGE 0x48..)
    char m_pad_end[0x18];   // +0xb64
};

class CBossTower
{
public:
    CBossTower();
    ~CBossTower();
private:
    char m_pad[0xb18];
};
}

namespace advancealtar
{
class StageControl
{
public:
    StageControl();
    ~StageControl();
private:
    char m_pad[0xc0];
};
}

namespace QuickParty
{
class CQuickParty
{
public:
    CQuickParty();
    ~CQuickParty();
private:
    char m_pad[0x28];
};
}

// ---- 补充 StaticPool 所需类型（占位尺寸，后续按 ORIG 修正）----

class CACHE_CHARACTER_TYPE
{
public:
    CACHE_CHARACTER_TYPE() {}
    ~CACHE_CHARACTER_TYPE() {}
private:
    char m_pad[0x100];
};

namespace expert_job
{
class CEnchanter
{
public:
    CEnchanter() {}
    ~CEnchanter() {}
private:
    char m_pad[0x100];
};

class CDisjointer
{
public:
    CDisjointer() {}
    ~CDisjointer() {}
private:
    char m_pad[0x100];
};
}

namespace pvp_assault
{
class CAssaultPlace
{
public:
    CAssaultPlace() {}
    ~CAssaultPlace() {}
private:
    char m_pad[0x100];
};
}

namespace private_store
{
class CPrivateStore
{
public:
    CPrivateStore() {}
    ~CPrivateStore() {}
private:
    char m_pad[0x100];
};
}

namespace exchange_server
{
class CSession
{
public:
    CSession() {}
    ~CSession() {}
private:
    char m_pad[0x100];
};
}

namespace WongWork
{
class CMailBox
{
public:
    struct stAddNewMailInput { char m_pad[0x100]; };

    CMailBox();
    ~CMailBox();

    class CMail
    {
    public:
        CMail() {}
        ~CMail() {}
    private:
        char m_pad[0x100];
    };

    int AddNewMail(const stAddNewMailInput&);
    void ClearLetterKeepCount();
    void DecLoadedLetterCount();
    int DeleteLetterKeepCount(unsigned int);
    int FindPackageLoadLack(unsigned int);
    int GetLastLoadIdx();
    int GetLastLoadLetterIdx();
    int GetLetterKeepCount();
    int GetLoadedLetterCount();
    CMail* GetMail(unsigned int);
    int GetNotLoadedMailCount();
    int GetPackageLoadLack(unsigned int*, unsigned int);
    int GetRecvSize();
    int GetRemainSize();
    void IncNotLoadedMailCount();
    void Init();
    void InsertLetterKeepCount(unsigned int);
    bool IsLoaded();
    int RemoveMail(unsigned int);
    void SetLastLoadLetterIdx(unsigned int);
    void SetLoadState(bool, long);
    void SetLoadedLetterCount(int);
    void SetNotLoadedMailCount(int);
    void SetPackageLoadLack(const unsigned int*, int, std::set<unsigned int>&);
    int getMailLoadCount();
    CMail* getNextMail();
    void incMailLoadCount();
    void reset();
    void setMailIterator();

private:
    char m_pad[0x100];
};
}

#include <cstdio>
typedef FILE _IO_FILE;

// ---- DynamicPool 所需类型 ----
namespace user_creature
{
class CEgg
{
public:
    CEgg() {}
    ~CEgg() {}
private:
    char m_pad[0x100];
};

class CCreature
{
public:
    CCreature() {}
    ~CCreature() {}
private:
    char m_pad[0x100];
};
}

namespace WongWork
{
class Avatar_Item
{
public:
    Avatar_Item() {}
    ~Avatar_Item() {}
private:
    char m_pad[0x100];
};

namespace IPG
{
class SIPGData
{
public:
    SIPGData() {}
    ~SIPGData() {}
private:
    char m_pad[0x100];
};
}
}

// ---- 管理器最小声明（本批只做指针持有 + new/delete）----
class CraneMinigameManager;
class CAuctionAveragePrice;
class CPremiumLetheManager;
class CConditionEventManager;
class CSpecialItemRoutingManager;
class CSharedServerMessageManager;
namespace QuickParty
{
class CQuickPartySystemManager;
class CQuickPartyRewardManager;
}

enum ENUM_CHARAC_LINK_ACTION_TYPE
{
    ENUM_CHARAC_LINK_ACTION_TYPE_0 = 0
};

// ============================================================================
// CGameManager
// ============================================================================
class CGameManager
{
public:
    CGameManager();
    ~CGameManager();

    bool init();

    // ---- 用户池 ----
    CUser* createUser();
    short getNextUID();
    int GetIdx(CUser* user);
    CUser* GetUserByAccId(unsigned int accId);
    CUser* getUserByUnique(short uniqueId);
    CUser* getUser(int slotId, int increId);
    void insertUserByAccID(CUser* user);
    bool isConnectUserByAccId(unsigned int accId);
    void returnUserPool(CUser* user);
    void check_user_var(CUser* user);

    // ---- 队伍 ----
    CParty* GetParty();
    CParty* GetParty(int index);
    int GetPartyIdx(CParty* party);
    void PutParty(CParty* party);
    void CheckOutParty(CUser* user, bool flag);
    unsigned int CheckOutQuickParty(CParty* party, bool flag);

    // ---- 交易 ----
    CTradeSpace* GetTradeSpace();
    CTradeSpace* GetTradeSpace(int index);
    int GetTradeIdx(CTradeSpace* tradeSpace);
    void PutTradeSpace(CTradeSpace* tradeSpace);
    void CheckOutTrade(CUser* user);

    // ---- PvP ----
    PvP_Room* GetPvp();
    PvP_Room* GetPvp(int index, CUser* user, int param);
    int GetPvpIdx(PvP_Room* room);
    void PutPvp(PvP_Room* room);
    void CheckOutPvp(CUser* user, bool flag);
    PvP_Room* FindQuickJoinablePvPRoom(bool quick, CUser* user);

    // ---- 快速队伍 ----
    QuickParty::CQuickParty* GetQuickParty();
    QuickParty::CQuickParty* GetQuickParty(int index);
    int GetQuickPartyIdx(QuickParty::CQuickParty* quickParty);
    void PutQuickParty(QuickParty::CQuickParty* quickParty);

    // ---- 公会战房间 ----
    WarRoom* GetWarRoom();
    WarRoom* GetWarRoom(int index);
    int GetWarRoomIdx(WarRoom* room);
    void PutWarRoom(WarRoom* room);
    void CheckOutWarRoom(CUser* user);
    WarRoom* FindJoinableWarRoom(CUser* user);
    void WarRoomAlloc();
    void WarRoomCountAdjustByChannelInfo();
    void WarRoomCountManage(int hourIndex);
    void WarRoomCountManageTest(int hourIndex);
    void PrintWarRoomList();
    void SendWarRoomToAll(int index);

    // ---- 死亡/领主塔 ----
    WongWork::CDeathTower* getDeathTower();
    WongWork::CDeathTower* getDeathTower(int index);
    int getDeathTowerIdx(WongWork::CDeathTower* tower);
    void returnDeathTower(WongWork::CDeathTower* tower);
    void checkOutDeathTower(CUser* user);
    WongWork::CBossTower* getBossTower();
    WongWork::CBossTower* getBossTower(int index);
    int getBossTowerIdx(WongWork::CBossTower* tower);
    void returnBossTower(WongWork::CBossTower* tower);
    void checkOutBossTower(CUser* user);

    // ---- 前置祭坛 ----
    advancealtar::StageControl* getAdvanceAltar();
    advancealtar::StageControl* getAdvanceAltar(int index);
    int getAdvanceAltarIndex(advancealtar::StageControl* control);
    void returnAdvanceAltar(advancealtar::StageControl* control);
    void checkOutAdvanceAltar(CUser* user);
    void onTimeAdvanceAltar();

    // ---- 蓝弹珠 ----
    BlueMarble* getBlueMarble();
    BlueMarble* getBlueMarble(int index);
    int getBlueMarbleIdx(BlueMarble* marble);
    void putBlueMarble(BlueMarble* marble);
    void checkOutBlueMarble(CUser* user);
    void allocBlueMarble();
    BlueMarble* findJoinableBlueMarble();

    // ---- 列表发送 ----
    void SendPartyList(CUser* user);
    void SendPvpList(CUser* user);
    void SendWarRoomList(CUser* user);
    void Send_userinfos_to_upper_server(unsigned char channelType);
    void send_userinfos_to_cutoff_server();
    unsigned int insert_game_world(CUser* user);
    void updateDBLinkCharacConnectState(
        unsigned int accId, unsigned int characNo,
        ENUM_CHARAC_LINK_ACTION_TYPE action);
    void user_disconnect(CUser* user);
    bool user_exit(CUser* user);

    // ---- 管理器 ----
    QuickParty::CQuickPartySystemManager* GetQuickPartySystemManager();
    QuickParty::CQuickPartyRewardManager* GetQuickPartyRewardManager();
    CPremiumLetheManager* GetPremiumLetheManager();
    CSharedServerMessageManager* GetSharedServerMessageManager();
    CSpecialItemRoutingManager* GetSpecialItemRoutingManager();
    CConditionEventManager* GetConditionEventManager();
    CAuctionAveragePrice* GetAuctionAveragePriceManager();
    CraneMinigameManager* GetCraneMinigameManager();

    bool IsNoGameGuardApplingUser(std::string& name, char* addr);

    // ---- FindUserByUnique（getUserByUnique 的局部谓词，ORIG 有独立 ctor 符号）----
    struct FindUserByUnique
    {
        FindUserByUnique(short uniqueId);
        bool operator()(std::pair<const unsigned int, CUser*>& p) const;
        short m_uniqueId;
    };

private:
    Mutex m_mutex;                       // +0x000（0x18）
    short m_uid;                         // +0x018
    char m_pad1a[2];                     // +0x01a
    std::set<std::string> m_noGameGuard; // +0x01c
    std::set<std::string> m_gameGuard2;  // +0x034
    StaticPool<CUser, 600> m_userPool;                     // +0x04c
    StaticPool<CTradeSpace, 300> m_tradeSpacePool;         // +0x07c
    StaticPool<CParty, 600> m_partyPool;                   // +0x0ac
    StaticPool<PvP_Room, 600> m_pvpRoomPool;               // +0x0dc
    StaticPool<WarRoom, 40> m_warRoomPool;                 // +0x10c
    StaticPool<WongWork::CDeathTower, 600> m_deathTowerPool;    // +0x13c
    StaticPool<WongWork::CBossTower, 600> m_bossTowerPool;      // +0x16c
    StaticPool<advancealtar::StageControl, 600> m_stagePool;    // +0x19c
    StaticPool<QuickParty::CQuickParty, 300> m_quickPartyPool;  // +0x1cc
    StaticPool<BlueMarble, 300> m_blueMarblePool;               // +0x1fc
    std::map<unsigned int, CUser*> m_userByAccId;    // +0x22c
    std::map<unsigned int, CUser*> m_userByAccId2;   // +0x244
    __gnu_cxx::hash_map<int, PvP_Room*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<PvP_Room*> > m_pvpRoomMap;    // +0x25c
    __gnu_cxx::hash_map<int, CParty*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<CParty*> > m_partyMap;        // +0x270
    __gnu_cxx::hash_map<int, WarRoom*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<WarRoom*> > m_warRoomMap;     // +0x284
    __gnu_cxx::hash_map<int, WongWork::CDeathTower*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<WongWork::CDeathTower*> > m_deathTowerMap;  // +0x298
    __gnu_cxx::hash_map<int, WongWork::CBossTower*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<WongWork::CBossTower*> > m_bossTowerMap;    // +0x2ac
    __gnu_cxx::hash_map<int, advancealtar::StageControl*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<advancealtar::StageControl*> > m_stageMap;  // +0x2c0
    __gnu_cxx::hash_map<int, QuickParty::CQuickParty*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<QuickParty::CQuickParty*> > m_quickPartyMap;  // +0x2d4
    __gnu_cxx::hash_map<int, BlueMarble*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<BlueMarble*> > m_blueMarbleMap;  // +0x2e8
    QuickParty::CQuickPartySystemManager* m_pQuickPartySystemMgr;  // +0x2fc
    QuickParty::CQuickPartyRewardManager* m_pQuickPartyRewardMgr;  // +0x300
    CPremiumLetheManager* m_pPremiumLetheMgr;                     // +0x304
    CSharedServerMessageManager* m_pSharedServerMessageMgr;      // +0x308
    CConditionEventManager* m_pConditionEventMgr;                 // +0x30c
    CSpecialItemRoutingManager* m_pSpecialItemRoutingMgr;         // +0x310
    CAuctionAveragePrice* m_pAuctionAveragePriceMgr;              // +0x314
    CraneMinigameManager* m_pCraneMinigameMgr;                    // +0x318
};

CGameManager* G_CGameManager();

#endif  // GAME_CGAMEMANAGER_H_
