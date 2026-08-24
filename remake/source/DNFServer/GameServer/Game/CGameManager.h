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
#include "SigTypes.h"    // SIG_* 完整定义（CMailBoxHelper 等使用）
#include "CUser.h"
#include "CParty.h"
#include "InterfacePacketBuf.h"
#include "PvP_Room.h"
#include "CExpertJob.h"      // expert_job::CEnchanter/CDisjointer 完整类型
#include "WarRoom.h"
#include "BlueMarble.h"
#include "Inven_Item.h"   // CMailBox::CMail 内嵌 Inven_Item（完整类型）
#include "CTradeSpace.h"   // 真实 CTradeSpace（0xf70）——替换原尺寸垫占位
#include "CBossTower.h"    // 真实 WongWork::CBossTower（0xb18）——替换原尺寸垫占位
#include "QuickParty.h"    // 真实 QuickParty::CQuickParty / CQuickPartySystemManager——替换本地占位

#include "StaticPool.h"   // StaticPool<T,N> 权威叶子头（2026-08-23 自本头提取）

// ============================================================================
// 池化对象最小声明（权威头属后续批次；尺寸 = ORIG 池节点数据区大小）
// ============================================================================


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

// CBossTower 类声明见 CBossTower.h（唯一声明点；CGameManager.h 顶部 include）
}

#include "advancealtar_StageDifficulty.h"  // StageDifficulty::T（StageControl::selectStage 参数；叶子头无环）
namespace advancealtar
{
class CharacData;   // 完整定义 advancealtar_ProcStage.h（StageControl::getCharacData 返回类型）
class ProcStage;    // 完整定义 advancealtar_ProcStage.h（StageControl::getProcStage 返回类型）
class StageControl
{
public:
    StageControl();
    ~StageControl();
    int getIndex() const;         // ORIG 0x81348ba
    void setIndex(int idx);       // ORIG 0x82a669c
    char leaveUser();             // ORIG 0x812fc4e（返回 0/1，见 class_func_reports）
    char onTimerStageTick();      // ORIG 0x812fb98（恒返回 1）
    void reset();                 // ORIG 0x812fa54
    int selectStage(CUser* user, int stageId, advancealtar::StageDifficulty::T diff);  // ORIG T 0x0812fa7a
    void sendStageInfo();         // ORIG T 0x081300f6：向当前用户发 0x209 舞台信息包
    // 子对象访问（ORIG W thunk objdump 实证；定义 advancealtar_StageControl.cpp）
    ProcStage* getProcStage();    // 0x081348ae：return (ProcStage*)(this+0x04)
    CharacData* getCharacData();  // 0x081348a0：return (CharacData*)(this+0x98)
    void destroy();               // ORIG T 0x081300ce：reset() + returnAdvanceAltar(this)
private:
    char m_pad[0xc0];
};
}

// QuickParty 类声明见 QuickParty.h（唯一声明点；CGameManager.h 顶部 include）


// ---- StaticPool 元素类型（布局按 ORIG 反汇编；实现见 CGameManager.cpp）----

// CACHE_CHARACTER_TYPE 完整定义在 CGameManager.cpp（SIG_LOAD_* 子对象布局，
// ORIG C1 0x8492262）；此处仅前置声明供 StaticPool 声明使用。
class CACHE_CHARACTER_TYPE;
// ORIG 全局枚举（mangled `17ENUM_ASSAULT_TEAM`，无 namespace 前缀）。
enum ENUM_ASSAULT_TEAM
{
    ENUM_ASSAULT_TEAM_0 = 0
};

namespace pvp_assault
{

// ORIG 枚举（pvp_assault 域；mangled NS_18ENUM_ASSAULT_STATEE）。
enum ENUM_ASSAULT_STATE
{
    ENUM_ASSAULT_STATE_0 = 0
};

// ORIG CAssaulter::C1 0x82e661e：清零前两个 int；步长 0x20（CAssaultPlace 内 [8]）。
class CAssaulter
{
public:
    CAssaulter() : m_field00(0), m_field04(0), m_state08(0) {}

    CUser* GetUser();                    // ORIG W 0x82f0542（读 +0x00）
    bool Empty();                        // ORIG W 0x82f055c（= IsAttachAble，+0x00==0）
    ENUM_ASSAULT_TEAM GetTeam();         // ORIG W 0x82f0570（读 +0x04）
    unsigned short GetUserUniqueId();    // ORIG T 0x82e6662（user->get_unique_id()）
    void SetState(ENUM_ASSAULT_STATE s); // ORIG W 0x82f057c（写 +0x08）
    ENUM_ASSAULT_STATE GetState();       // ORIG W 0x82f058a（读 +0x08）
    int GetHp();                         // ORIG T 0x82e6688：return *(int*)(this+0x10)
    void DetachUser();                   // ORIG T 0x82e6812：清槽位 + 卸 creature
    void GiveupPanalty();                // ORIG T 0x82e6868（依赖 cUserHistoryLog::GiveupPvP）

    unsigned int m_field00;   // CUser* m_user
    unsigned int m_field04;   // ENUM_ASSAULT_TEAM m_team
    unsigned int m_state08;   // ENUM_ASSAULT_STATE
    char m_pad0c[0x4];
    int m_hp10;               // +0x10（GetHp 读）
    char m_pad14[0x0c];
};

// ORIG Point2d::C1 0x82f046a（默认空）/C1Eii 0x82f0472 / operator- 0x82f0496。
class Point2d
{
public:
    Point2d() {}
    Point2d(int x, int y) : m_x(x), m_y(y) {}

    int m_x;
    int m_y;
};

// ORIG PartyInfo::C1 0x82f05ee：清零单个 int；Set/Get 即该 int 的读写。
class PartyInfo
{
public:
    PartyInfo() : m_field00(0) {}

    unsigned int m_field00;
};

// ORIG CAssaultPlace::C1 0x82f0614：CAssaulter[8]@+0x000（步长 0x20）->
//   Point2d@+0x118 -> PartyInfo[2]@+0x12c（步长 4）；D1 0x82f13b6 反向析构数组。
class CAssaultPlace
{
public:
    CAssaulter m_assaulters[8];     // +0x000
    char m_pad100[4];               // +0x100（GetAssaulterCount 读 +0x100）
    int m_field104;                 // +0x104（图标包 put_int）
    char m_pad108[4];               // +0x108
    int m_state;                    // +0x10c（GetState）
    short m_view110;                // +0x110（图标包 put_short）
    short m_view114;                // +0x114（图标包 put_short）
    Point2d m_pos;                  // +0x118（IsArea 比较 x/y）
    PartyInfo m_parties[2];         // +0x12c
    // ---- 方法声明（CAssaultMgr 批次使用；实现后续批次按报告还原） ----
    bool IsArea(int vill, int area);                        // ORIG W 0x82f06ae
    bool ExistUser(CUser* user);                            // ORIG T 0x82e8366
    bool CheckPlayable();                                   // ORIG W 0x82f06ea
    int GetState();                                         // ORIG W 0x82f06fe
    void MakeViewAssaultIconOnePlayer(PacketGuard* p, bool flag);
                                                            // ORIG T 0x82e89c0
    bool SendPacket(PacketGuard& p);                        // ORIG T 0x82e6bf6
    bool SendPacket(PacketGuard& p, ENUM_ASSAULT_TEAM team);// ORIG T 0x82e6c56
    bool GetTeam(CUser* user, ENUM_ASSAULT_TEAM& team);     // ORIG T 0x82e6cdc
    void LeaveAssaultPlace(CUser* user, bool flag);         // ORIG T 0x82eb450
    static void SendUserState(CUser* user);                 // ORIG T 0x82e83c4（ORIG 调用点仅压一个栈参——静态形态）

    // ---- 前置内部函数（上述方法依赖；ORIG 权威符号）----
    bool _CheckPlayable();                                  // ORIG T 0x82e9142
    void _GetTeamCount(int* counts);                        // ORIG T 0x82e7232
    int _GetUserIndex(CUser* user, bool strict);            // ORIG T 0x82e849a
    void LeaveAssaultPlace(int index, bool delFlag,
                           bool penaltyFlag);               // ORIG T 0x82ead6a

    // ---- 攻击 LeaveAssaultPlace 链私有 helper（ORIG 权威；自包含部分已还原）----
    bool _IsGiveupPanalty(bool flag);                       // ORIG T 0x82eacf4
    bool _IsBroadcastRunaway(CUser* user);                  // ORIG T 0x82eab1a
    void _BroadcastRunaway(CUser* user, int index);         // ORIG T 0x82eab7a
    void _MakeLeaveUser(PacketGuard* p, int index);         // ORIG T 0x82e9046
    void _MakeCloseAssault(PacketGuard* p, int tick);       // ORIG T 0x82e88d6
    void _MakeUserState(PacketGuard* p, CUser* user);       // ORIG T 0x82e8d60
    int  _SendCloseAssault(CUser* user, int tick);          // ORIG T 0x82e7b2a
};

// ORIG T 0x82e564c：team 0→6、1→4、其他→0。
int GetDirectionByTeam(ENUM_ASSAULT_TEAM team);

}  // namespace pvp_assault

namespace exchange_server
{

// ORIG CSession::C1 0x848ffd0：基类 CNetwork<95000,450000>（尺寸 0xb8+N1+N2 =
//   0x851a0）-> 置基类 m_field0c=1 -> 清零 +0x851a0/+0x851a4。
class CSession : public CNetwork<95000, 450000>
{
public:
    CSession();

private:
    unsigned int m_field851a0;  // +0x851a0
    unsigned int m_field851a4;  // +0x851a4
};

// ---- 角色缓存管理器自由函数（CUser::CacheCharacterMemory 等调用；ORIG）----
// 实现见 CGameManager.cpp（CCacheCharacterMgr + GetInstanceCacheCharacterMgr）。
CACHE_CHARACTER_TYPE* AcquireCharacterMemory(int characNo);       // 0x848f220
void FreeCharacterMemory(CACHE_CHARACTER_TYPE* mem);              // 0x848f23c
bool CacheCharacterMemory(int characNo, CACHE_CHARACTER_TYPE* mem);// 0x848f258

}  // namespace exchange_server

#ifndef DNF_GAME_MSG_CHECK_MAIL_RECEIVER_DEFINED
#define DNF_GAME_MSG_CHECK_MAIL_RECEIVER_DEFINED
// ============================================================================
// MSG_CHECK_MAIL_RECEIVER —— 收件人校验请求负载（ORIG size 0x3f88=16264 字节）
//   位于全局作用域（ORIG mangled `_ZN23MSG_CHECK_MAIL_RECEIVER...` 无 WongWork 前缀）。
//   ORIG 符号（nm -C df_game_r）：
//     _ZN23MSG_CHECK_MAIL_RECEIVER3setILNS_22ENUM_MAILBOX_CHECKTYPEE0E16MSG_MAILBOX_SENDEEvRKT0_  @0x8236e06
//     _ZN23MSG_CHECK_MAIL_RECEIVER3setILNS_22ENUM_MAILBOX_CHECKTYPEE1E21MSG_BUY_CERASHOP_ITEMEEvRKT0_ @0x82378ee
//     _ZN23MSG_CHECK_MAIL_RECEIVER3setILNS_22ENUM_MAILBOX_CHECKTYPEE2E22MSG_MULTI_MAILBOX_SENDEEvRKT0_ @0x8238086
//   set<CHECKTYPE,T>(T const&) 语义：*(int*)this = CHECKTYPE; memcpy((char*)this+4, &msg, sizeof(T))。
//   布局：+0 枚举标记，+4 起写入消息 T（ReqDBCheckReceiver 据 +0 类型解释 +4 载荷）。
//   （报告中 set<2> memcpy 0x1a6 / set<0> memcpy 0x145 表明各消息 T 尺寸不同，
//    模板按 sizeof(T) 复制即可还原；set<1> 使用 MSG_BUY_CERASHOP_ITEM(0x3f84)。）
// ============================================================================
struct MSG_CHECK_MAIL_RECEIVER
{
    enum ENUM_MAILBOX_CHECKTYPE
    {
        ENUM_MAILBOX_CHECKTYPE_0 = 0,
        ENUM_MAILBOX_CHECKTYPE_1 = 1,
        ENUM_MAILBOX_CHECKTYPE_2 = 2,
        ENUM_MAILBOX_CHECKTYPE_3 = 3
    };

    template <enum ENUM_MAILBOX_CHECKTYPE TYPE, class MSG_T>
    void set(MSG_T const& msg)
    {
        *(int*)this = (int)TYPE;
        const char* src = reinterpret_cast<const char*>(&msg);
        char* dst = reinterpret_cast<char*>(this) + 4;
        for (unsigned int i = 0; i < sizeof(MSG_T); ++i)
            dst[i] = src[i];
    }

    char m_pad[0x3f88 - 4];   // 载荷区（+4 起），总大小 0x3f88=16264
};
#endif

#include "CMailBoxHelper.h"   // WongWork::CMailBox / CMailBoxHelper 权威叶子头
                              // （2026-08-23 自本头原样提取；MSG_CHECK_MAIL_RECEIVER 定义保留在上方）
 
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

// ORIG：user_creature 域枚举（CStomach::GetStomachValue/IsSaveStomach、
// CCreature::SetState 使用；值 1..3 由 SetState/GetStomachValue 分支实证，
// 命名为推断）。
enum CREATURE_STATE
{
    CREATURE_STATE_0 = 0,
    CREATURE_STATE_1 = 1,   // 战斗/召唤态（SetState 计时器分支）
    CREATURE_STATE_2 = 2,   // 休眠态（Reset 初值）
    CREATURE_STATE_3 = 3    // 死亡/待复活态（SetState 复活分支）
};

class CStomach
{
public:
    // ORIG GetStomachValue 0x8336cfc：+0 为 int 上次时间、+4 为 float 饱食度
    //（CCreature 内偏移 +0x34/+0x38；旧注释两字段顺序颠倒，已按汇编修正）。
    int m_lastTime;     // +0x00（CCreature +0x34）
    float m_value;      // +0x04（CCreature +0x38）

    // ORIG _ZN13user_creature8CStomach15GetStomachValueENS_14CREATURE_STATEEb T 0x8336cfc
    float GetStomachValue(CREATURE_STATE state, bool growFlag);
    // ORIG _ZN13user_creature8CStomach15SetStomachValueEf W 0x833eef6：写 +4（m_value）
    void SetStomachValue(float value);
    // ORIG _ZN13user_creature8CStomach14IncrementValueEf T 0x08336e66：
    // m_value += amount；>100 则钳到 100.0f；返回新值。
    float IncrementValue(float amount);
};                                    // 共 8B（+0x34..+0x3b）

// ORIG 内部包类型（CDBMsgSender::Init 0x833f7a8：0x89 走 BigStreamPool，
// CCreatureMgr::RecvMail 使用 0x8a；其余值待 DB 域批次核对）。
enum ENUM_INTERNALPACKET
{
    ENUM_INTERNALPACKET_0x89 = 0x89,
    ENUM_INTERNALPACKET_RECV_MAIL = 0x8a   // 推断命名
};

class CDBMsgSender
{
public:
    CDBMsgSender(ENUM_INTERNALPACKET type, CUser* user);  // ORIG C1 0x833f8ae W
    ~CDBMsgSender();                                      // ORIG D1/D2 0x833f8d8 W

    bool Init(ENUM_INTERNALPACKET type, CUser* user);     // ORIG 0x833f7a8 T
    bool Send();                                          // ORIG 0x833f950 W
    // ORIG _ZN13user_creature12CDBMsgSender15UseFeedCreatureEii W 0x0833fc5a：
    // GetInBuffer<SIG_CREATURE_ITEM> 清 0x74；m_hdr|=2；+0x30=uid、+0x44=饱食度。
    bool UseFeedCreature(int uid, int stomachValue);
    // ORIG _ZN13user_creature12CDBMsgSender18InsertCreatureItemEP5CUserP10Inven_Itemiiiii
    // W 0x0833faaa：物品校验（find_item/非堆叠）后按 InsertCreatureItem 消息填库。
    bool InsertCreatureItem(CUser* user, Inven_Item* item, int slot, int reason,
                            int mapSize, int days, int expireTick);
    // ORIG _ZN13user_creature12CDBMsgSender8RecvMailEP10Inven_Itemiiii W 0x833ff34
    bool RecvMail(Inven_Item* item, int characNo, int slot, int reason, int mapSize);
    // ORIG _ZN13user_creature12CDBMsgSender8SendMailEP10Inven_Itemiiii W 0x833fe8c
    bool SendMail(Inven_Item* item, int characNo, int slot, int reason, int mapSize);

    template <class T> T* GetInBuffer()
    {
        if (m_stream == 0)
            return 0;
        CStreamGuard guard(m_stream, false);
        return guard.GetInBuffer<T>();
    }

    Stream* m_stream;   // +0x00
    CUser* m_user;      // +0x04
    // ORIG _ZN13user_creature12CDBMsgSender15EvoluteCreatureEii W 0x833fd74：
    // buf 清 0x74；m_hdr|=0x40；compat.m_itemIdx(+0x08)=新宠物物品 idx；
    // m_addInfo2(+0x30)=宠物 uid。Evolute/SelectEvolute 进化落库用。
    bool EvoluteCreature(int uid, int itemIdx);
    // ORIG _ZN13user_creature12CDBMsgSender16SwapCreatureItemEP5CUserP10Inven_ItemiS4_i
    // W 0x0833fa0c：SIG_SWAP_CREATURE_ITEM 清 0x74 后填双物品 idx/slot/空标志。
    bool SwapCreatureItem(CUser* user, Inven_Item* item1, int slot1,
                          Inven_Item* item2, int slot2);
};
class COverSkillTime
{
public:
    int m_time;  // ORIG ctor 0x833d636 清零（4B）
    bool checkTime();  // ORIG T 0x0833d65e：(curSec-m_time)∈(0,20.0) 浮点判定
};


class CCreatureScript;  // user_creature 域内前置（权威声明见 CCreatureScript.h）

// ORIG CCreature 继承 CCreatureItem（实证：SetState/UseFeed 以
// CCreatureItem::GetUid(this) 取 +0x0c；IsNoCharge(+0x14) 与
// CCreatureItem::GetNoCharge 同址；CCreature ctor 0x833705e 先跑基类构造）。
// 本镜像将基区（+0x00..+0x1b）展开为具名字段，偏移与 CCreatureItem 一致。
// 访问器符号（ORIG W，0x833f04c..0x833f0e6 / GetStomach 0x083384ee /
// UseFeed T 0x083373d4）：
//   IsSave/GetName/GetLevel/GetExp/IsNoCharge/SetNoCharge/IsGrowCreature/
//   GetCreatureId/GetOverSkillTime/GetStomach。
// 注意 IsGrowCreature ORIG 语义 = (+0x4c ^ 1) != 0（0x833f0ca xor $1 实证，
// 即 Reset 后的 +0x4c==1 返回 false）；GainExp/UseFeed/vaildItemExp 则直接
// 判 +0x4c==1 / ==0。
class CCreature
{
public:
    CCreature();   // ORIG _ZN13user_creature9CCreatureC1Ev T 0x833705e
    ~CCreature();
    void Reset();  // ORIG _ZN13user_creature9CCreature5ResetEv W 0x83370b0
    int GetState();                                       // ORIG W 0x0833f064（+0x3c）
    void SetState(CUser* user, CREATURE_STATE state);     // ORIG T 0x0833715a
    bool Equip(CUser* user);                              // ORIG T 0x08337330（战斗态→state1，否则 state3）
    bool Dismantle(CUser* user);                          // ORIG T 0x08337382（state2）

    bool IsDieCreature();                                 // ORIG T 0x8338478
    // ORIG _ZN13user_creature9CCreature7vaildItemExpEi 0x83378aa
    bool vaildItemExp(int itemIdx);
    // ORIG _ZN13user_creature9CCreature7GainExpEiP5CUser 0x83378fc
    bool GainExp(int exp, CUser* user);
    // ---- GainExp 依赖链（CreatureExperience 批次；签名与 ORIG mangled 一致）----
    void SetItemId(int itemId);                              // ORIG T 0x83375ae
    int GetCreatureId(int itemId);                           // ORIG T 0x833769c（重载：itemIdx→种族 id）
    int GetExpLevel(int exp);                                // ORIG T 0x83374d2
    bool Evolute(CUser* user);                               // ORIG T 0x83381ea
    bool SelectEvolute(CUser* user);                         // ORIG T 0x8337e48
    bool IsAvailableArtifact(int slot);                      // ORIG T 0x83376f6
    float GetAddExp(int artifactSlot, CUser* user);          // ORIG T 0x8337724
    void CalculateExp(int exp, bool& levelUp, CUser* user);  // ORIG T 0x83377b6
    bool GainExpCreature(int exp, CUser* user);              // ORIG T 0x8337bbc
    bool GainItemExpCreature(int exp, CUser* user);          // ORIG T 0x83379c0
    // ORIG _ZN13user_creature9CCreature11isMaxLevelEv 0x833756e
    bool isMaxLevel();
    // ORIG _ZN13user_creature9CCreature7UseFeedEP5CUserRi T 0x083373d4：
    // 喂食：复活死亡宠广播 → IncrementValue(30) → 状态包，out 写新饱食度。
    bool UseFeed(CUser* user, int& stomachValue);

    // ---- ORIG W 访问器 ----
    bool IsSave();                    // W 0x833f04c：movzbl +0x40
    const char* GetName();            // W 0x833f058：返回 this+0x1c
    int GetStomach();                 // W/T 0x083384ee：(int)GetStomachValue(+0x3c,+0x4c)
    int GetExp();                     // W 0x833f08e：+0x30
    int GetLevel();                   // W 0x833f09a：+0x2c
    bool IsNoCharge();                // W 0x833f0a6：movzbl +0x14
    void SetNoCharge(bool flag);      // W 0x833f0b2：写 +0x14
    bool IsGrowCreature();            // W 0x833f0ca：(+0x4c ^ 1) != 0
    int GetCreatureId();              // W 0x833f0da：+0x48
    COverSkillTime* GetOverSkillTime();  // W 0x833f0e6：this+0x50

    int IsAbleEvolute(CUser* user) const;   // ORIG T 0x083380fc（RestoreUserQuestStubs 批次）
    int IsAbleSelectEvolute(CUser* user);   // ORIG T 0x08337d58（RestoreUserQuestStubs 批次）
    // ---- 布局（总 0x100；基区按 CCreatureItem 展开为具名字段）----
    int m_vptr;                     // +0x00（ORIG vptr 槽占位）
    int m_itemId;                   // +0x04（CCreatureItem::GetItemId）
    int m_creatureItemType;         // +0x08（Reset=1；列表包过滤要求 ==1）
    int m_uid;                      // +0x0c（CCreatureItem::GetUid）
    int m_slotNo;                   // +0x10
    unsigned char m_noCharge;       // +0x14（Reset=0；IsNoCharge/SetNoCharge）
    unsigned char m_stat;           // +0x15
    char m_pad16[2];                // +0x16
    int m_expireTime;               // +0x18（CCreatureItem::getExpireTime）
    char m_name[13];                // +0x1c（GetName；Reset memset 0xd）
    unsigned char m_pad29[3];       // +0x29
    int m_level;                    // +0x2c（Reset=1；GainExp/isMaxLevel 比较）
    int m_exp;                      // +0x30（原 m_field30；CalculateExp/包序列化）
    CStomach m_stomach;             // +0x34
    int m_state;                    // +0x3c（原 m_field3c；Reset=2；GetState/SetState）
    unsigned char m_saveFlag;       // +0x40（原 m_field40；SetState/GainExpCreature=1；IsSave）
    unsigned char m_pad41[3];       // +0x41
    CCreatureScript* m_script;     // +0x44（Reset=0）
    int m_creatureId;               // +0x48（原 m_field48；Reset=0；GetCreatureId）
    unsigned char m_isGrow;        // +0x4c（Reset=1；语义见类注释）
    unsigned char m_field4d;       // +0x4d（Reset=5）
    unsigned char m_pad4e[2];      // +0x4e
    COverSkillTime m_overSkill;    // +0x50
    char m_pad54[0xac];            // → 总 0x100（保持镜像尺寸）
};

}

namespace WongWork
{
// Avatar_Item 权威定义见 AvatarItem.h（ORIG packed，sizeof 0x100）。此处仅前置声明；
// 需要完整类型（如 CGameManager.cpp 的 DynamicPool<Avatar_Item> 显式实例化）时 include "AvatarItem.h"。
class Avatar_Item;

// WongWork::IPG::SIPGData 权威定义见 TaiwanCash.h（ORIG sizeof 0x2dc，
// DynamicPool<SIPGData> MoreAlloc 节点步长 0x2e4 实证；本头原 0x100 局部镜像
// 已删除，DynamicPool 显式实例化由 TaiwanCash.cpp 的隐式实例化供符号）。
namespace IPG { struct SIPGData; }
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
    int userPoolRemain() { return m_userPool.Remain(); }  // MainLoop 心跳条件用

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
