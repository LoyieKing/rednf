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
#include <cstring>
#include <queue>
#include <set>
#include <string>
#include <utility>

#include "CacheCharacterType.h"  // 全局 ::ENUM_INTERNALPACKET 权威定义（ORIG mangled 无嵌套名；CDBMsgSender 族签名依赖）
#include "CUser.h"
#include "CUser.h"
#include "CParty.h"
#include "InterfacePacketBuf.h"
#include "PvP_Room.h"
#include "CExpertJob.h"      // expert_job::CEnchanter/CDisjointer 完整类型
#include "WarRoom.h"
#include "BlueMarble.h"
#include "Inven_Item.h"   // CMailBox::CMail 内嵌 Inven_Item（完整类型）
#include "CInventory.h"   // user_creature::CCreatureItem/CCreatureMgr 完整类型（CEgg 基类）
#include "CTradeSpace.h"  // 真实 CTradeSpace（0xf70）——StaticPool<CTradeSpace,300> 需完整类型
#include "CBossTower.h"    // 真实 WongWork::CBossTower（0xb18）——替换原尺寸垫占位
#include "QuickParty.h"    // 真实 QuickParty::CQuickParty / CQuickPartySystemManager——替换本地占位

#include "StaticPool.h"   // StaticPool<T,N> 权威叶子头（2026-08-23 自本头提取）

// ============================================================================
// 池化对象最小声明（权威头属后续批次；尺寸 = ORIG 池节点数据区大小）
// ============================================================================


struct map_item;
class CTradeSpace;   // 兜底前置（完整类型见上方 CTradeSpace.h）
struct SIG_CREATURE_ITEM_DETAIL_INFO;   // ORIG 全局域；完整布局见 CCreatureMgr.cpp


#include "CDeathTower.h"   // WongWork::CDeathTower 完整定义（唯一声明归口，2026-08-27 迁出）

namespace WongWork
{
    class CDeathTower;
}
#include "advancealtar.h"  // advancealtar::StageControl 完整定义（唯一声明归口；
                           // StaticPool<StageControl,600> / m_stageMap 需要完整类型）。
                           // 原 m_pad[0xc0] 占位段已迁 advancealtar.h（具名布局）。

// QuickParty 类声明见 QuickParty.h（唯一声明点；CGameManager.h 顶部 include）


// ---- StaticPool 元素类型（布局按 ORIG 反汇编；实现见 CGameManager.cpp）----

// CACHE_CHARACTER_TYPE 完整定义在 CGameManager.cpp（SIG_LOAD_* 子对象布局，
// ORIG C1 0x8492262）；此处仅前置声明供 StaticPool 声明使用。
class CACHE_CHARACTER_TYPE;
#include "CAssaultMgr.h"   // pvp_assault 域权威头（OnlyOrig_Assault 迁入；原 251-358 段整体迁出）

#include "ExchangeServer.h"   // exchange_server 权威类/自由函数声明（OnlyOrig_Exchange 批）
using exchange_server::AcquireCharacterMemory;
using exchange_server::CacheCharacterMemory;
using exchange_server::FreeCharacterMemory;

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
        // ORIG 0x8236e06 起：*(int*)this=TYPE; memcpy(this+4, &msg, sizeof(T))。
        *(int*)this = (int)TYPE;
        std::memcpy(reinterpret_cast<char*>(this) + 4, &msg, sizeof(MSG_T));
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
class CEgg : public CCreatureItem    // ORIG 继承 CCreatureItem（ctor 基区构造实证）
{
public:
    CEgg();        // ORIG T 0x08338f4c：基类 ctor → vptr←&_ZTV CEgg+0x8(0x08c2f4a8) → Reset
    // ORIG D1 0x08338f74 仅写 vptr（0x8338f7a movl $0x8c2f4a8,(%eax)）。
    // 外联定义（P210：头内联体使 DynamicPool<CNode>::~CNode 调用点 4.4 保守撑帧
    // 0x18 vs ORIG 0x4；探针 /tmp/bm3/p1-p2）。定义随 CNode 特化置于 CGameManager.cpp。
    ~CEgg();
    void Reset();  // ORIG T 0x08338f82：基类字段清零
    bool IsSave(); // ORIG W 0x0833f102：字面返回 0
    bool Hatch(int* out);   // ORIG T 0x0833912a（定义 CCreatureMgr.cpp）
    bool LoadCreatureItem(CCreatureMgr* mgr, Inven_Item* item,
                          SIG_CREATURE_ITEM_DETAIL_INFO* info, CUser* user);  // ORIG T 0x08338fac
    bool SaveCreatureItem(Inven_Item* item,
                          SIG_CREATURE_ITEM_DETAIL_INFO* info, CUser* user);  // ORIG T 0x083390c8
};  // sizeof == sizeof(CCreatureItem)（ORIG 无扩展字段）

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
// [2026-09-11 J10GameRest·父泳道] ABI 宽度锁（C++98 安全老式断言，不用 static_assert
// 关键字以免 gnu++98 路由 TU 编译失败）：字段 CCreature::m_state(+0x3c) 用它，宽度恒 4B。
typedef char CREATURE_STATE_must_be_4_bytes[sizeof(CREATURE_STATE) == 4 ? 1 : -1];

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
    // ORIG _ZN13user_creature8CStomach13IsSaveStomachENS_14CREATURE_STATEE T 0x8336dee：
    // state==1 → 饱食度>0 才可保存；state 2..3 → 饱食度==100.0f 视为满不可保存；
    // 其余 state 恒真。
    bool IsSaveStomach(CREATURE_STATE state);
    // ORIG _ZN13user_creature8CStomach6ResumeEv T 0x8336e4c：m_lastTime = 当前秒。
    void Resume();   // ORIG _ZN13user_creature8CStomach6ResumeEv T 0x8336e4c：m_lastTime=当前秒（定义 CreatureOnlySimple.cpp）
    CStomach();      // ORIG T 0x83336cd8（定义 CreatureOnlySimple.cpp）
};                                    // 共 8B（+0x34..+0x3b）

// ORIG 内部包类型（CDBMsgSender::Init 0x833f7a8：0x89 走 BigStreamPool，
// CCreatureMgr::RecvMail 使用 0x8a；其余值待 DB 域批次核对）。
// 全局 ::ENUM_INTERNALPACKET 权威定义见 CacheCharacterType.h（守卫
// DNF_GAME_ENUM_INTERNALPACKET_DEFINED；本头顶部已引入）。
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
    // ORIG _ZN13user_creature12CDBMsgSender12UseKeepRightEii W 0x083401ec：
    // GetInBuffer<SIG_CREATURE_ITEM> 清 0x74；m_hdr|=0x80；+0x30=uid、
    // +0x52=flag 低字节。SetKeepRightItem 免充标志落库用。
    bool UseKeepRight(int uid, int flag);
    template <class T> T* GetInBuffer()
    {
        if (m_stream == 0)
            return 0;
        CStreamGuard guard(m_stream, false);
        return guard->GetInBuffer<T>();
    }

    Stream* m_stream;   // +0x00
    CUser* m_user;      // +0x04
    // ORIG _ZN13user_creature12CDBMsgSender15EvoluteCreatureEii W 0x833fd74：
    // buf 清 0x74；m_hdr|=0x40；compat.m_itemIdx(+0x08)=新宠物物品 idx；
    // m_addInfo2(+0x30)=宠物 uid。Evolute/SelectEvolute 进化落库用。
    bool EvoluteCreature(int uid, int itemIdx);
    // ORIG _ZN13user_creature12CDBMsgSender20EventEvoluteCreatureEiii W 0x833fddc：
    // buf 清 0x74；m_hdr|=0x248；compat.m_itemIdx(+0x08)=新宠物物品 idx；
    // m_addInfo2(+0x30)=uid；exp 写 +0x4c；find_item 后 m_days(+0x54)=
    // getUsablePeriod()、m_expireTick(+0x58)=getExpirationDate()。EventEvolute 落库用。
    bool EventEvoluteCreature(int uid, int itemIdx, int exp);
    bool SwapCreatureItem(CUser* user, Inven_Item* item1, int slot1,
                          Inven_Item* item2, int slot2);
    // ORIG _ZN13user_creature12CDBMsgSender16RenameCreatureItemEiPKc W 0x083405aa
    // ORIG _ZN13user_creature12CDBMsgSender16RenameCreatureItemEiPKc W 0x083405aa
    bool ChangeOwner(Inven_Item* item, int characNo, int slot, int reason,
                     int mapSize, int field6c, int field70);
    // ORIG _ZN13user_creature12CDBMsgSender12ChangeSlotNoEP10Inven_Itemiii W 0x083400a4
    bool ChangeSlotNo(Inven_Item* item, int characNo, int slot, int reason);
    // ORIG _ZN13user_creature12CDBMsgSender13AuctionRegistEP10Inven_Itemiiii W 0x081faff2（p2 未读；p3 低字节→+0x51）
    bool AuctionRegist(Inven_Item* item, int characNo, int unused14,
                       int unused18, int mapSize);
    void Cancel();   // ORIG _ZN13user_creature12CDBMsgSender6CancelEv W 0x0833f994：三路按池归还并置空
    //   （Main 授权补充；RenameCreature 落库用，定义随 CCreatureMgr.cpp）。
    bool RenameCreatureItem(int uid, const char* name);
    // ORIG _ZN13user_creature12CDBMsgSender13HatchCreatureEii W 0x083405b4
    //   （Main 授权补充；Hatch 落库用，定义随 CCreatureMgr.cpp）。
    bool HatchCreature(int uid, int flag, int itemIdx);
};
class COverSkillTime
{
public:
    COverSkillTime();          // ORIG T 0x0833d636：m_time 清零（定义 CreatureOnlySimple.cpp）
    int m_time;                // ORIG（4B）
    void SetTime();            // ORIG T 0x0833d644：m_time = 当前秒
    int* GetTime();            // ORIG W 0x0833ef04：返回 &m_time
    bool checkTime();          // ORIG T 0x0833d65e：(curSec-m_time)∈(0,20.0)
};

class CCreatureScript;  // user_creature 域内前置（权威声明见 CCreatureScript.h）
class CCreatureMgr;     // 前置：SaveCreatureItem/LoadCreatureItem 双向引用

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
class CCreature : public CCreatureItem   // [b28d-Main·检查点14] 真继承（ORIG ctor 0x833705e
                                         //   `call CCreatureItem::C1Ev` + `movl $0x8c2f4c0,(%eax)`）
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
    bool UseFeed(CUser* user, int& stomachValue);
    // ORIG _ZN13user_creature9CCreature10SetStomachEi W 0x0833f070：写 +0x38
    // ORIG _ZN13user_creature9CCreature10SetStomachEi W 0x0833f070：写 +0x38
    // m_stomach.m_value = (float)stomachValue。
    void SetStomach(int stomachValue);

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

    // [b28d-solo-CCreatureMgr P248档①] ORIG 调用点 0x8337d11 `call; test %al,%al;
    // je`（无 setcc）⇒ 返回 bool（返回类型不入 mangled，布局零影响）。
    bool IsAbleEvolute(CUser* user) const;  // ORIG T 0x083380fc（RestoreUserQuestStubs 批次）
    bool IsCreatureScript();   // ORIG _ZN13user_creature9CCreature16IsCreatureScriptEv W 0x0833f0f2：return m_script != 0（定义 CreatureOnlyMgrRest.cpp）
    bool IsAbleSelectEvolute(CUser* user);  // ORIG T 0x08337d58（同族 bool 化）
    // ---- Save/Load 落库链（TranslateCCreatureMgrTU 批次；签名与 ORIG mangled 一致）----
    // ---- TranslateCCreatureTU 追加（ORIG 报告齐备，实现落 CCreature.cpp；
    //      经 user_creature 域属主 TranslateCCreatureMgrTU 书面协调授权）----
    bool RenameCreature(const char* name);              // ORIG T 0x83373aa
    bool EvoluteMulty(CUser* user, int creatureId);     // ORIG T 0x8338950
    bool EventEvolute(CUser* user, int creatureId, int slot);  // ORIG T 0x833db2e
    // [R11fixE7 2026-09-12] ORIG prett 串 `bool user_creature::CCreature::ItemEvolute(
    // CUser*, int)`（raw @0x8c2f320，我方原串为 `int ...`）⇒ 返回类型 bool
    //（mangled 不含返回类型；体内 `return true/false`）；ORIG T 0x8338ca0。
    bool ItemEvolute(CUser* user, int itemIdx);         // ORIG T 0x8338ca0
    bool IsEvolutionCreature(int creatureId);           // ORIG T 0x83388ec
    bool IsEvolutionCreatureMulty(int creatureId);      // ORIG T 0x8338922
    bool IsEventEvolutionCreature(int creatureId);      // ORIG T 0x8338bfa
    int GetLearnOverSkillLevel();                       // ORIG T 0x8338c78
    int GetOverSkillRecoveryTime();                     // ORIG T 0x8338c50
    int GetSkillRecoveryTime();                         // ORIG T 0x8338c28
    // ORIG _ZN13user_creature9CCreature16SaveCreatureItemEP10Inven_ItemP29SIG_CREATURE_ITEM_DETAIL_INFOP5CUser
    //   T 0x83387f6：宠物序列化进 SIG_LOAD_INVENTORY 条目（GetCreatureItem 逐条调用）。
    bool SaveCreatureItem(Inven_Item* item, SIG_CREATURE_ITEM_DETAIL_INFO* info,
                          CUser* user);
    // ORIG _ZN13user_creature9CCreature16LoadCreatureItemEPNS_12CCreatureMgrEP10Inven_ItemP29SIG_CREATURE_ITEM_DETAIL_INFOP5CUser
    //   T 0x83385aa：DB 载入侧逆操作（注册/装备/回填槽物）。
    bool LoadCreatureItem(CCreatureMgr* mgr, Inven_Item* item,
                          SIG_CREATURE_ITEM_DETAIL_INFO* info, CUser* user);
    // ORIG _ZN13user_creature9CCreature3DieEP5CUser T 0x8338536：死亡通知广播。
    bool Die(CUser* user);
    // ORIG _ZN13user_creature9CCreature17CheckDiedCreatureEv... 实为 (P5CUser) T 0x83384b8
    bool CheckDiedCreature(CUser* user);
    // ---- 布局（总 0x54；+0x00..+0x18 基区由 CCreatureItem 继承，字段/偏移逐项不变）----
    char m_name[13];                // +0x1c（GetName；Reset memset 0xd）
    unsigned char m_pad29[3];       // +0x29
    int m_level;                    // +0x2c（Reset=1；GainExp/isMaxLevel 比较）
    int m_exp;                      // +0x30（原 m_field30；CalculateExp/包序列化）
    CStomach m_stomach;             // +0x34
    CREATURE_STATE m_state;         // +0x3c（原 m_field3c；Reset=2；GetState/SetState）
                                    // [2026-09-11 J10GameRest] ORIG 0x8337192
                                    //   `mov 0x3c(%eax),%eax; cmp 0x10(%ebp),%eax`（mem 直比）
                                    //   + 0x8337318 LHS-first store ⇒ 字段与 enum 形参同型；
                                    //   宽度 4B ⇒ 布局/偏移/mangled 零变化（探针 /tmp/probe_enum.cpp）。
    bool m_saveFlag;                // +0x40（原 m_field40；SetState/GainExpCreature=1；IsSave，ORIG bool 直返）
    unsigned char m_pad41[3];       // +0x41
    CCreatureScript* m_script;     // +0x44（Reset=0）
    int m_creatureId;               // +0x48（原 m_field48；Reset=0；GetCreatureId）
    bool m_isGrow;                 // +0x4c（Reset=1；ORIG bool 类型，GetStomachValue 直传）
    unsigned char m_field4d;       // +0x4d（Reset=5）
    unsigned char m_pad4e[2];      // +0x4e
    COverSkillTime m_overSkill;    // +0x50
};

class CCreatureItem;   // 前向：CCreatureItemFactory 返回/释放（权威定义见 CInventory.h）

// ORIG 创世宠物项工厂（ctor 0x0833f67a）：两 DynamicPool（+0x00 CEgg 池、
// +0x5c CCreature 池，各 0x3e8 基数），总尺寸 0xb8。AcquireCreatureItem(type)
//   type==0 → 蛋池，type==1 → 宠物池；Acquire 后经 vtable+0 调 C1。
class CCreatureItemFactory
{
public:
    CCreatureItemFactory();   // ORIG C1 0x0833f67a（定义 CCreatureMgr.cpp）
    ~CCreatureItemFactory();  // ORIG D1 0x0833f6d0（定义 CCreatureMgr.cpp）
    CCreatureItem* AcquireCreatureItem(CREATURE_ITME_TYPE type);  // ORIG _ZN13user_creature20CCreatureItemFactory19AcquireCreatureItemENS_18CREATURE_ITME_TYPEE T 0x0833f57c（定义 CCreatureMgr.cpp）
    int GetSize(CREATURE_ITME_TYPE type);          // ORIG T 0x0833f546
    int GetRemainSize(CREATURE_ITME_TYPE type);    // ORIG T 0x0833f510
                                                   // （上两者定义 CreatureOnlySimple.cpp）
    bool FreeCreatureItem(CCreatureItem* item);    // ORIG 0x0833f618
private:
    DynamicPool<CEgg> m_eggPool;             // +0x00
    DynamicPool<CCreature> m_creaturePool;   // +0x5c
};

// ORIG guard 单例（guard var 0x943c070 / instance 0x943c080）。定义 CCreatureMgr.cpp。
CCreatureItemFactory* GetInstanceCreatureItemFactory();

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

// ---- DynamicPool<CEgg/CCreature>::CNode 显式特化声明（b22） ----
// 明确特化须声明在模板所在（全局）命名空间；定义在 CGameManager.cpp
// （写 CNode vptr 0x8c2f478/0x8c2f490 + magic/bUse）。声明前置使所有 include
// 本头的 TU 引用该特化而非实例化通用无-vptr CNode 模板，对齐 ORIG 弱符号
// CNodeC1/D1（仅 CEgg/CCreature 两实例存在，PacketBuf 等无）。
template <> DynamicPool<user_creature::CEgg>::CNode::CNode();
template <> DynamicPool<user_creature::CEgg>::CNode::~CNode();
template <> DynamicPool<user_creature::CCreature>::CNode::CNode();
template <> DynamicPool<user_creature::CCreature>::CNode::~CNode();

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
class CQuickParty;
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
    // [Q6HeaderTypeAxis 2026-09-11 类型轴] ORIG prett = `CUser* CGameManager::
    //   GetUserByAccId(memberIdentificationNumber_t)`（我方实测串 0x32 vs ORIG 0x42）；
    //   同底层 `j`，mangled/ABI 零变化。
    CUser* GetUserByAccId(memberIdentificationNumber_t accId);
    CUser* getUserByUnique(short uniqueId);
    CUser* getUser(int slotId, int increId);
    void insertUserByAccID(CUser* user);
    // [b28d-Main·检查点22] 两个 inline getter 已删：`getUserByAccIdMap()` 在 ORIG
    //   nm 零命中（我方发明；inline 定义仍被 4.4 out-of-line 发射 ⇒ 违例）。
    //   ORIG 实证调用方一律 `call G_CGameManager; lea 0x22c(%eax)` **成员直读**
    //   （TimerScheduleTenSecond 0x08637936 / Inter_CodeChecksumRange 0x084d0648 /
    //    BingoEvent::StartAction 0x080c9bb8 / BingoEvent::dailyresetData 0x080c9d82）
    //   ⇒ 改由 friend 直访 m_userByAccId / m_userByAccId2，见下方 friend 块。
    bool isConnectUserByAccId(unsigned int accId);
    void returnUserPool(CUser* user);
    void check_user_var(CUser* user);

    // ---- 队伍 ----
    CParty* GetParty();
    CParty* GetParty(int index);
    int GetPartyIdx(CParty* party);
    void PutParty(CParty* party);
    void CheckOutParty(CUser* user, bool flag);
    bool CheckOutQuickParty(CParty* party, bool flag);

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
    bool insert_game_world(CUser* user);   // ORIG prett `bool CGameManager::insert_game_world(CUser*)`（P139 串修，返回类型不入 mangling）
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

    // [b28d-Main·检查点22] 两访问器声明已删（ORIG 无 `_ZNK12CGameManager22
    // GetActiveQuickPartyCntEv` / `_ZN12CGameManager28GetOccupiedQuickPartyPoolCntEv`；
    //   全仓零调用者——QuickParty.cpp:135 仅剩注释）。ORIG 的
    //   QuickParty::CQuickParty::destroy(0x082695ca) 直读 m_quickPartyMap/池计数
    //   （该 TU 已是 `friend class QuickParty::CQuickParty`）。
    bool IsNoGameGuardApplingUser(std::string& name, char* addr);

    // ---- FindUserByUnique（getUserByUnique 的局部谓词，ORIG 有独立 ctor 符号）----
    struct FindUserByUnique
    {
        FindUserByUnique(short uniqueId);
        bool operator()(std::pair<const unsigned int, CUser*>& p) ;
        short m_uniqueId;
    };
    // ORIG MainLoop 直调 StaticPool<CUser,600>::Size()/Remain()（+0x4c m_userPool），
    // 故不设 userPool* 发明 wrapper（§4.1）；MainLoop/ProcessServerStatusLog/
    // AcceptThread::dispatch 以 friend 直访私有池。广播 2026-09-06 批准。
    friend void ProcessServerStatusLog();  // ORIG 0x80ede91 访问私有池（GameMain.cpp 定义）
    friend bool MainLoop();                 // ORIG 0x829fa2b 直访私有池（GameMain.cpp 定义）
    friend class AcceptThread;              // GlobalData.cpp AcceptThread::dispatch 直访私有池
    friend class QuickParty::CQuickParty;   // ORIG destroy 0x082695ca 直访 +0x1cc pool
                                            //   /+0x2d4 hash_map（P48 直调形恢复，S23B 广播）
    // [b28d-Main·检查点22] getter 删除后的成员直访授权（friend 不改布局/sizeof/
    //   mangled，亦不发射任何符号；同 getUser/TimerScheduleTenSecond 先例）：
    friend class BingoEvent;                 // ORIG 0x080c9bb8 / 0x080c9d82 直读 +0x22c
    friend class Inter_CodeChecksumRange;    // ORIG 0x084d0648 直读 +0x22c
    // ORIG _Z7getUserj @0x084bb9cf 直访 m_userByAccId2（反汇编 `call G_CGameManager;
    // lea 0x244(%eax),%edx` 无 getter call，证明成员直读）——广播 2026-09-05 无异议。
    friend CUser* getUser(unsigned int uid);
    // ORIG TimerScheduleTenSecond 0x08637936 直访 +0x22c m_userByAccId
    // （`call G_CGameManager; lea 0x22c(%eax),%edx`，-O0 无 inline getter 内联）；
    // 广播 2026-09-09（S409）：friend 不改布局/sizeof/mangled。
    friend class TimerScheduleTenSecond;

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
