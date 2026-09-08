#ifndef GAME_CINVENTORY_H_
#define GAME_CINVENTORY_H_
#include <cstddef>
// SIG_CREATURE_ITEM_DETAIL_INFO 权威定义已纯移动至 GamePODTypes.h（2026-09-04
// DBLoad，叶子 POD 头免拉本头重量级链；guard 宏随迁保持幂等）。
#include "GamePODTypes.h"
struct SIG_POSTAL_CREATURE_TEMP_PERIOD;    // ORIG 全局域；仅证 +0 uid/+4 tempPeriod(-1 空)；指针传递即可

// ============================================================================
// df_game_r 还原 —— CInventory（第八批 H3 + 第九批 I2 对齐，ORIG 布局）
// 布局依据 CInventory.md 各函数反汇编逐成员推导：
//   +0x00 CUser* m_pParent        +0x04 int m_money
//   +0x08 int m_coin              +0x0c int m_payCoin
//   +0x10 int m_eventCoin         +0x14 int m_field14
//   +0x18 int m_inventoryCapacity +0x1c Inven_Item m_invenItem[26]
//   +0x650 Inven_Item* m_pEquipSlot(0x138)   +0x654 Inven_Item* m_pAvatarSlot(0x69)
//   +0x658 AvatarCoin m_avatarCoin +0x65c char m_pad65c[0x28]
//   +0x684 AvatarVisibleData m_avatarVisible[12] +0x6e4 Inven_Item* m_pCreatureSlot(0xf2)
//   +0x6e8 user_creature::CCreatureMgr m_creatureMgr +0x734 WongWork::CAvatarItemMgr
// CAvatarItemMgr 真实尺寸 0x84（权威头 CAvatarItemMgr.h，替代本地最小镜像 m_pad[0x4c]），
// CInventory 总尺寸 0x7b8（匹配 CharacInfoFields::m_inven 的 +0xda..0x892 区段）。
// Inven_Item 尺寸 0x3d；equip 312 / avatar 105 / creature 242 槽。
// CItem/CStackableItem/CEquipItem/Inven_Item/CDataManager 使用权威头（I1 批次），
// 本头不再重复声明；CUser/CUserCharacInfo/cUserHistoryLog 等尚无权威头的类保留
// 最小本地声明，成员签名按 ORIG nm 符号逐一核对（const 性/枚举类型名/引用）。
// ============================================================================

#include <stddef.h>
#include <utility>
#include <vector>
#include <map>

#include "CItem.h"
#include "CStackableItem.h"
#include "CEquipItem.h"
#include "CDataManager.h"
#include "CAvatarItemMgr.h"   // 权威 WongWork::CAvatarItemMgr（真实布局 0x84，替代本地最小镜像）
#include "CExpandEquipslot.h" // 权威 CExpandEquipslot（charac_expand::CData 派生，布局 0x899；
                              // 唯一声明点，删除本头原局部最小镜像避免 ODR）
#include "InterfacePacketBuf.h"
#include "CUser.h"          // 权威 CUser/cUserHistoryLog/共享枚举（G1-3 统一）
#include "AvatarCoin.h"     // 权威 AvatarCoin（m_avatarCoin +0x658；替代原内嵌空桩占位）

// ---- 枚举（值以 CInventory 反汇编中立即数为准） ----
#ifndef DNF_ENUM_INVEN_TYPE_DEFINED
#define DNF_ENUM_INVEN_TYPE_DEFINED
enum INVEN_TYPE
{
    INVEN_TYPE_INVEN = 0,
    INVEN_TYPE_EQUIP = 1,
    INVEN_TYPE_AVATAR = 2,
    INVEN_TYPE_CREATURE = 3,
    INVEN_TYPE_5 = 5,
    INVEN_TYPE_6 = 6
};
#endif

#ifndef DNF_ENUM_EITEMADDREASON_DEFINED
#define DNF_ENUM_EITEMADDREASON_DEFINED
enum eItemAddReason
{
    eItemAddReason_0 = 0,
    eItemAddReason_1 = 1,
    eItemAddReason_2 = 2,
    eItemAddReason_7 = 7,
    eItemAddReason_8 = 8,
    eItemAddReason_0xd = 0xd
};
#endif

#ifndef DNF_ENUM_EMONEYADDREASON_DEFINED
#define DNF_ENUM_EMONEYADDREASON_DEFINED
enum eMoneyAddReason
{
    eMoneyAddReason_0 = 0,
    eMoneyAddReason_4 = 4,
    eMoneyAddReason_7 = 7,
    eMoneyAddReason_0x1a = 0x1a
};
#endif

#ifndef DNF_ENUM_EITEMDELREASON_DEFINED
#define DNF_ENUM_EITEMDELREASON_DEFINED
enum eItemDelReason
{
    eItemDelReason_0 = 0,
    eItemDelReason_3 = 3
};
#endif

#ifndef DNF_ENUM_EMONEYSUBREASON_DEFINED
#define DNF_ENUM_EMONEYSUBREASON_DEFINED
enum eMoneySubReason
{
    eMoneySubReason_0 = 0
};
#endif

#ifndef DNF_ENUM_ECOINSUBREASON_DEFINED
#define DNF_ENUM_ECOINSUBREASON_DEFINED
enum eCoinSubReason
{
    eCoinSubReason_0 = 0
};
#endif

#ifndef DNF_ENUM_ECOINADDREASON_DEFINED
#define DNF_ENUM_ECOINADDREASON_DEFINED
enum eCoinAddReason
{
    eCoinAddReason_0 = 0
};
#endif

#ifndef DNF_ENUM_EAVATARITEMADDREASON_DEFINED
#define DNF_ENUM_EAVATARITEMADDREASON_DEFINED
enum eAvatarItemAddReason
{
    eAvatarItemAddReason_0 = 0,
    eAvatarItemAddReason_0x14 = 0x14
};
#endif

#ifndef DNF_ENUM_ITEMSPACE_DEFINED
#define DNF_ENUM_ITEMSPACE_DEFINED
enum ENUM_ITEMSPACE
{
    ENUM_ITEMSPACE_0 = 0
};
#endif

// ---- CUser 依赖的全局枚举（ORIG mangling 需要枚举类型名；权威 CUser.h 后续批次统一） ----
#ifndef DNF_ENUM_CHARAC_EXPAND_TYPE_DEFINED
#define DNF_ENUM_CHARAC_EXPAND_TYPE_DEFINED
enum ENUM_CHARAC_EXPAND_TYPE
{
    ENUM_CHARAC_EXPAND_TYPE_0 = 0,
    ENUM_CHARAC_EXPAND_TYPE_9 = 9
};
#endif

#ifndef DNF_ENUM_DBLOG_ITEM_TYPE_DEFINED
#define DNF_ENUM_DBLOG_ITEM_TYPE_DEFINED
enum ENUM_DBLOG_ITEM_TYPE
{
    ENUM_DBLOG_ITEM_TYPE_0 = 0
};
#endif

#ifndef DNF_ENUM_SECURITY_PROTCTION_DEFINED
#define DNF_ENUM_SECURITY_PROTCTION_DEFINED
enum SECURITY_PROTCTION { SECURITY_PROTCTION_0 = 0 };
#endif

enum VALUE_STATISTIC_FIELD
{
    VALUE_STATISTIC_FIELD_0 = 0
};

#include "GamePODTypes.h"  // stAvatarEmblemInfo_t/stAvatarExpansionInfo_t 唯一定义（去重，布局一致）

struct stEquipCondition;
struct stEquipConditionForChronicle;

#include "md5.h"   // md5_context/md5_starts/md5_update/md5_finish 权威归口本头外（md5.h）

namespace OS_API
{
int GetDateTimeTick();
int snprintf(char* buf, unsigned int n, const char* fmt, ...);
void Sleep(int ms);            // ORIG T 0x0858c4e0 usleep(ms*1000)(定义 LinkFinalDeps.cpp)
}

// ORIG T 0x08109098:_Z14NumberToStringyi(64 位版;u32 版声明见下行;
// 两版本定义 LinkFinalDeps.cpp,槽位=gNumberToStringBuffer[radix*0x40])
char* NumberToString(unsigned long long value, int radix);

namespace user_creature
{
class CCreature;
class CArtifact
{
public:
    CArtifact();                                    // ORIG W 0x833ef0c（调 Reset）
    void Reset();                                   // ORIG T 0x8336f60：清零唯一指针字段
    bool EquipArtifact(const Inven_Item* item);     // ORIG T 0x8336f6e
    float GetExperienceAmount();                    // ORIG T 0x83336fc8（非 const）
    int GetCreatureMinimumLevel();                  // ORIG T 0x83337012（非 const）
    CEquipItem* m_pItem;                            // +0x00（ORIG 存取均按此唯一 4B 字段）
};

// ============================================================================
// CCreatureItem（ORIG 0x083340c82 ctor / 0x0833f016 GetRemainDate 布局建模）。
// 布局（逐字段反汇编实证）：
//   +0x00 vptr（ORIG 多态：ctor 写 vtable 0x8c2f4d8；CCreature 继承之）
//   +0x04 m_itemId            （GetItemId/SetItemId 0x833efa4/b0）
//   +0x08 m_creatureItemType  （GetCreatureItemType/SetCreatureItemType 0x833ef98/e2）
//   +0x0c m_uid               （GetUid 0x833efbe）
//   +0x10 m_slotNo            （GetSlotNo/SetSlotNo 0x833efca/0x8337050）
//   +0x14 m_noCharge byte     （GetNoCharge 0x833efd6 movzbl）
//   +0x15 m_stat byte         （GetStat 0x833eff0 movzbl）
//   +0x18 m_expireTime        （GetRemainDate 0x833f016 读取；Set 0x833ef20 清零）
// 仅建模本任务所需字段；其余 ORIG 字段以 m_pad 保持尺寸语义。
// ============================================================================
// user_creature::CREATURE_ITME_TYPE（拼写 ITME 为 ORIG mangled 原样）：
//   蛋池=0 / 宠物池=1（GetCreatureEggPoolSize/CCreatureMgr::Hatch 实证）。
enum CREATURE_ITME_TYPE
{
    CREATURE_ITME_TYPE_EGG = 0,
    CREATURE_ITME_TYPE_CREATURE = 1
};

class CCreatureItem
{
public:
    CCreatureItem();   // ORIG C1/C2 W 0x08340c82：写 vtable（外联定义 CreatureOnlySimple.cpp）
    int GetRemainDate(int nowSec) const;  // ORIG _ZNK...13GetRemainDateEi 0x833f016（W）

    // ---- ORIG W 内联访问器（nm --defined-only 实证，体为单字段读写）----
    int GetItemId();                      // W 0x833efa4（外联定义 CreatureOnlySimple.cpp；ORIG 调用形态为 call）
    void SetItemId(int itemId) { m_itemId = itemId; }           // W 0x833efb0
    CREATURE_ITME_TYPE GetCreatureItemType();  // W 0x833ef98（定义 CCreatureMgr.cpp，ORIG 外联调用形态；返回类型不入 mangled，Set 内同型直存 LHS-first 实证）
    void SetCreatureItemType(CREATURE_ITME_TYPE type);   // ORIG W 0x833efe2（外联 CreatureOnlySimple.cpp；ORIG 无 int 版符号）
    int GetUid();                                               // W 0x833efbe（外联定义
                                                                //   CreatureOnlySimple.cpp；
                                                                //   SetStomach 0x08339ee0+0x69
                                                                //   实证调用点为 call）
    int GetSlotNo() { return m_slotNo; }                        // W 0x833efca
    unsigned char GetNoCharge();   // ORIG W 0x833efd6（movzbl +0x14）；外联定义 CreatureOnlySimple.cpp
    unsigned char GetStat();       // ORIG W 0x833eff0（movzbl +0x15）
    void SetExpireTime(int expireTime) { m_expireTime = expireTime; }  // W 0x833effc
    int getExpireTime() const { return m_expireTime; }          // W 0x833f00a（_ZNK）
    void SetSlotNo(int slotNo);                                 // ORIG T 0x08337050（写 +0x10）
    // ORIG _ZN13user_creature13CCreatureItem3SetEPS0_ W 0x0833ef20：深拷贝 src 的
    //   m_itemId/m_creatureItemType/m_uid/m_slotNo/m_noCharge/m_stat/m_expireTime
    //   （蛋 → 宠物项复制语义；定义 CCreatureMgr.cpp）。
    void Set(CCreatureItem* src);

    int m_vptr;            // +0x00（ORIG vptr 槽；本镜像以整型保持偏移）
    int m_itemId;          // +0x04
    CREATURE_ITME_TYPE m_creatureItemType;  // +0x08（ORIG SetCreatureItemType 0x833efe2
                                            //  store LHS-first ⇒ 字段必为 enum 同型，
                                            //  探针 /tmp/bl8_probe3.cpp）
    int m_uid;             // +0x0c
    int m_slotNo;          // +0x10
    unsigned char m_noCharge; // +0x14
    unsigned char m_stat;     // +0x15
    char m_pad16[2];       // +0x16..0x17
    int m_expireTime;      // +0x18（GetRemainDate 读此；Set 0x833ef20 清零）
};
class CCreatureItemConverter
{
public:
    // ORIG 0x08336eae（_ZN13user_creature22CCreatureItemConverter10SetEggItemEP10Inven_Item，
    // T）：槽物为蛋（ITEM_TYPE 5）时把它转成孵化蛋。返回非 0 表示成功。
    // 注：ORIG 调用方（_processCreature / check_insert_lottery_reward）
    // 以 Inven_Item* 为唯一实参（无 this），故为 static。
    static bool SetEggItem(Inven_Item* item);   // ORIG 调用点 P22 `xor$1` ⇒ bool（P9 返回 0/1 全宽 mov 同 int）
};
class CCreatureMgr
{
public:
    CCreatureMgr();   // ORIG T 0x08339196（map/CArtifact[3] 隐式成员构造）
    CCreatureMgr(const CCreatureMgr& src);  // ORIG W C1 0x86992cc
    void Reset();
    bool CheckDiedCreature();   // ORIG T _ZN13user_creature12CCreatureMgr17CheckDiedCreatureEv 0x08339acc（TimerCreature 调用；定义缺口记报告）
    void SetUser(CUser* user);
    void TurnStomach(bool fighting);
    int UseItem(Inven_Item* item, int slot);  // ORIG _ZN13user_creature12CCreatureMgr7UseItemEP10Inven_Itemi
    bool SwapCreatureItem(Inven_Item* a, int slotA, int typeA,
                          Inven_Item* b, int slotB, int typeB);
    // ---- SwapCreatureItem 调用链（Prod_DB_Misc 批次；签名与 ORIG mangled 一致）----
    bool SwapCreature(Inven_Item* item1, int slot1, int type1,   // ORIG T 0x08339420
                      Inven_Item* item2, int slot2, int type2);
    bool SwapArtifact(Inven_Item* item1, int slot1, int type1,   // ORIG T 0x08339752
                      Inven_Item* item2, int slot2, int type2);
    bool IsArtifactEquipSlot(int slot);                          // ORIG T 0x083393fc
    bool EquipArtifact(int slot, Inven_Item* item);              // ORIG T 0x083396c8
    bool DismantleArtifact(int slot, Inven_Item* item);          // ORIG T 0x08339718
    bool EquipCreatureItem(CCreature* dismantleItem,             // ORIG T 0x083393a2
                           CCreature* equipItem);
    bool Equip(CCreature* creature);                             // ORIG T 0x083392f6
    bool Dismantle(CCreature* creature);                         // ORIG T 0x0833934c
    int GetExtraGold(int gold) const;                            // ORIG T 0x0833c31a（Prod_CParty get_item 依赖）
    int GetOverSkillExtraGold(int gold) const;                   // ORIG T 0x0833c6f6
    bool SetKeepRightItem(CUser* user, Inven_Item* item);  // ORIG T 0x0833c00c（定义 CCreatureMgr.cpp）
    bool GainExp(int exp);                               // ORIG _ZN13user_creature12CCreatureMgr7GainExpEi
    bool IsEquippedCreature() const;                     // ORIG _ZNK13user_creature12CCreatureMgr18IsEquippedCreatureEv
    bool IsGrowCreature_Equipped_Creature() const;       // ORIG _ZNK13user_creature12CCreatureMgr32IsGrowCreature_Equipped_CreatureEv
    int VerifyEquipedCreatureQuest() const;  // ORIG _ZNK13user_creature12CCreatureMgr26VerifyEquipedCreatureQuestEv T 0x0833d6fa
    bool InsertCreatureItem(Inven_Item* item, int a,     // ORIG _ZN13user_creature12CCreatureMgr18InsertCreatureItemEP10Inven_Itemiiii
                            int b, int c, int d);
    int GetCreatureItemId() const;                 // ORIG _ZNK13user_creature12CCreatureMgr17GetCreatureItemIdEv
    const char* GetCreatureName() const;           // ORIG _ZNK13user_creature12CCreatureMgr15GetCreatureNameEv
    bool UseFeed();                              // ORIG _ZN13user_creature12CCreatureMgr7UseFeedEv
    bool CheckItem(CUser* user, Inven_Item* item, bool& b);  // ORIG T 0x0833c240（定义 CCreatureMgr.cpp）
    int ResponseCreature();                        // ORIG T 0x0833a6d8（定义 CCreatureMgr.cpp）
    bool RegisterCreatureItem(CCreatureItem* item, int uid);  // ORIG T 0x0833a77e（定义 CCreatureMgr.cpp）
    void setTempExpireTime(int time);            // ORIG W 0x833f10c
    bool IsCreatureEquipmentScope(int slot);       // ORIG 0x833926c（非 const）
    bool IsDieCreature() const;                    // ORIG _ZNK13user_creature12CCreatureMgr13IsDieCreatureEv
    int GetEquipedCreatureLevel() const;           // ORIG _ZNK13user_creature12CCreatureMgr24GetEquipedCreatureLevelEv
    int SendCreatureItemList() const;              // ORIG _ZNK13user_creature12CCreatureMgr20SendCreatureItemListEv（透传 CUser::Send 的 int）
    bool RecvMail(Inven_Item* item, int slot, int reason);   // ORIG 0x0833ab9a（bool 返回）
    bool SendMail(Inven_Item* item, int slot, int reason);   // ORIG _ZN13user_creature12CCreatureMgr8SendMailEP10Inven_Itemii 0x0833aa32
    bool DeleteCreatureItem(int param_1, int param_2);       // ORIG _ZN13user_creature12CCreatureMgr18DeleteCreatureItemEii 0x0833b606
    bool UnregisterCreatureItem(int uid);                    // ORIG _ZN13user_creature12CCreatureMgr22UnregisterCreatureItemEi 0x0833a854
    bool RegistTempPeriod(int a, int b, int c);              // ORIG 0x0833d7de（bool 返回）
    int getRemainTempPeriod(int uid, int nowSec) const;      // ORIG 0x0833d88c（const）
    int GetRemainDate(int uid, int nowSec) const;            // ORIG _ZNK...13GetRemainDateEii 0x0833d78e
    CCreatureItem* FindCreatureItem(int uid) const;          // ORIG 0x08339292（_ZNK）
    CArtifact* GetArtifact(int slot);  // ORIG _ZN13user_creature12CCreatureMgr11GetArtifactEi T 0x833be56（槽 0x17..0x1a → m_artifacts[0..3]）
    bool GetCreatureItem(SIG_LOAD_INVENTORY* sig) const;      // ORIG 0x833b80c 尾 mov$1 直返；GetInventoryInfo 调用点 xor$1 直判= bool 形（mangled 无返回类型 ABI 零变）
    // ---- ORIG 布局（Reset.md 0x833a45a / InsertCreatureItem.md 0x833b204 反汇编）----
    // [b28c-30 SIGFIX] 返回类型 void→bool：ORIG __PRETTY_FUNCTION__ 串为
    // "bool user_creature::CCreatureMgr::RenameCreature(const char*, int, int)"
    //（P174/P159），Ghidra C 尾 `return bVar5`；mangled 不含返回类型。
    bool RenameCreature(const char* name, int creatureIdx, int slot);  // ORIG _ZN13user_creature12CCreatureMgr14RenameCreatureEPKcii 0x08339b6e
    bool Hatch(int addInfo);                                           // ORIG _ZN13user_creature12CCreatureMgr5HatchEi 0x08339fa0
    bool Evolute(int a, int b, bool flag);                             // ORIG T 0x0833beb4 _ZN13user_creature12CCreatureMgr7EvoluteEiib

    // ---- OnlyOrig_CreatureJob 批追加（定义 CreatureOnlyMgrRest.cpp）----
    int SetStomach(int stomachValue);                                  // ORIG T 0x08339ee0（尾 `mov $1,%eax` 恒返 1）
    bool ChangeOwner(Inven_Item* item, int slotFrom, int mapSizeHint,  // ORIG T 0x0833ad34
                     int param_4, int param_5);
    int ItemEvolute(int itemIdx);                                      // ORIG T 0x0833d6c2
    bool ChangeSlotNo(Inven_Item* item, int newSlot, int param_3);     // ORIG T 0x0833aedc
    bool LoadArtifact(int slot, Inven_Item* item);                     // ORIG T 0x0833be7c
    bool AuctionRegist(Inven_Item* item, int param_2, int param_3);    // ORIG T 0x0833b09c
    bool DenySwapArtifact(Inven_Item* a, int s1, int s2,               // ORIG T 0x0833989c
                          Inven_Item* b, int s3, int s4);
    int GetCreatureIndex();                                            // ORIG T 0x08338f36
    bool LoadCreatureItem(Inven_Item* item,                            // ORIG T 0x0833b68c
                          SIG_CREATURE_ITEM_DETAIL_INFO* detail);
    bool SaveCreatureItem();                                           // ORIG T 0x0833bbbe
    int SetOverSkillTime(const int& itemIdx);                          // ORIG T 0x0833c41e
    bool IsSaveCreatureItem();                                         // ORIG T 0x0833ba30
    bool UnRegistTempPeriod(int uid);                                  // ORIG T 0x0833d962
    bool EventEvoluteCreature(int creatureId, int minLevel,            // ORIG T 0x0833da1e
                              int slot);
    bool SendCreatureItemInfo(SIG_CREATURE_ITEM* sig);                 // ORIG T 0x0833b498
    bool LoadCreatureTempPeriod(SIG_POSTAL_CREATURE_TEMP_PERIOD* sig); // ORIG T 0x0833d9d8
    bool SetCreatureItemAddInfo(int slot, int value);                  // ORIG T 0x0833b620
    bool InsertCreatureItemResult(SIG_CREATURE_ITEM* sig);             // ORIG T 0x0833b38c
    bool UpdateCreatureItemResult(SIG_CREATURE_ITEM* sig);             // ORIG T 0x0833b592
    ~CCreatureMgr();                                                   // ORIG T D1/D2 0x08339226（~CCreatureMgr.md）
    CCreatureMgr& operator=(const CCreatureMgr& src);                  // ORIG W 0x869d614【objdump】
    int getExpireDate(int uid) const;                                  // ORIG T 0x0833d758
    int GetCreatureItemSize() const;                                   // ORIG W 0x8531fbe【objdump】
    bool IsExistedCreatureItem(int slot) const;                        // ORIG T 0x0833a942
    bool checkMaxCreatureLevel(int itemId, bool growFlag) const;       // ORIG T 0x0833baee
    bool isGrowCreatureMaxLevel() const;                               // ORIG T 0x0833bbb2
    std::map<int, CCreatureItem*> m_creatureItems;  // +0x00（24B）
    CCreature* m_equippedCreature;                  // +0x18
    CUser* m_user;                                  // +0x1c
    CArtifact m_artifacts[3];                       // +0x20..+0x2b（各 4B）
    int m_tempExpireDelta;                          // +0x2c（Reset 置 -1）
    std::map<int, int> m_field30;                   // +0x30
    bool m_isMaxLevelGrowCreature;                 // +0x48（Reset 置 1；ORIG isGrowCreatureMaxLevel movzbl 按 bool 读取）
    char m_pad49[3];                                // +0x49，总 0x4c
};
}
// AvatarCoin 权威声明见 AvatarCoin.h（本头原内嵌空桩占位与权威头冲突，
// 已删除；m_avatarCoin 成员 +0x658 使用同一权威类，方法由 AvatarCoin.cpp /
// Arad_AvatarRoulette.cpp 提供真实定义）。

// WongWork::CGenUniqueNo 权威声明见 CGenUniqueNo.h（char* genIPGNo，
// 完整 ENUM_IPGNO_TYPE）；本头不再局部重复声明。
#include "CGenUniqueNo.h"
// ---- 外部依赖（其它 TU 提供；成员签名按 ORIG nm 核对） ----
class CSlotBoundChecker;
class Store;
class CValueStatistic;
class PacketGuard;
class CSecu_ProtectionField;

extern Store* G_Store();
extern CValueStatistic* GetInstanceValueStatistic();
extern CSlotBoundChecker g_slotBoundChecker;  // ORIG object (addr-taken at 0x8500384)

class CSecu_ProtectionField
{
public:
    // ORIG _ZN21CSecu_ProtectionField5CheckEP5CUser18SECURITY_PROTCTION（0x08288a02）：
    // thiscall 成员函数。user 为空返回 1；否则 GetProtectionType(type) 为 0 返回 0，
    // 再依次 CheckRestoreType → CheckTradingType → CheckEtcType，首个非 0 返回。
    int Check(CUser* user, SECURITY_PROTCTION type);
    // ORIG 0x0828873a（_ZN21CSecu_ProtectionField9CheckItemEP5CUser18SECURITY_PROTCTIONm）：
    // 物品 index 命中受保护集合时委托 Check(user,type)，否则 0。
    int CheckItem(CUser* user, SECURITY_PROTCTION type, unsigned long itemIndex);
    // ORIG 0x08288786（_ZN21CSecu_ProtectionField9CheckItemEP5CUser18SECURITY_PROTCTIONPK5CItem）：
    // item/user 任一为 0 则 1，否则取 item->get_index() 走上面的重载。
    int CheckItem(CUser* user, SECURITY_PROTCTION type, const CItem* item);
    int GetOppositeErr(int err);
    int GetCashShopErr(int err);        // ORIG W 0x82881a2：商城错误码映射
    unsigned int GetProtectionType(SECURITY_PROTCTION type);
    int CheckRestoreType(CUser* user, unsigned long protType);   // ORIG m EP5CUserm 0x08288bf2
    int CheckTradingType(CUser* user, unsigned long protType, SECURITY_PROTCTION type); // ORIG m 0x082880c46…0x08288c46
    int CheckEtcType(CUser* user, SECURITY_PROTCTION type);
    // ---- LinkMisc5 收口（定义 CSecu_ProtectionField.cpp）----
    Inven_Item* GetInvenItemPtr(CUser* user, char space, short slot);  // ORIG T 0x082886ae
    CItem* GetItemPtr(CUser* user, char space, short slot);            // ORIG T 0x082884d6
    int CheckItemRarity(CUser* user, SECURITY_PROTCTION type,
                        const CItem* item, const Inven_Item* itemRef); // ORIG T 0x082887d0
    int CheckOpposite(CUser* user, SECURITY_PROTCTION type, CUser* opposite); // ORIG T 0x08288994
    int CheckAccountType(CUser* user, unsigned long protType);         // ORIG T 0x08288ac8
    CSecu_ProtectionField();   // ORIG W 0x82aa6d4：构造 m_typeMap 后 init()
    void init();               // ORIG T 0x828824e：填充 0x47 项保护类型表

private:
    std::map<SECURITY_PROTCTION, unsigned int> m_typeMap;   // +0x00（0x18）
 };

namespace GlobalData
{
extern CSecu_ProtectionField* s_pSecuProtectionField;
}

// ORIG C1 0x837355a = map C1 + init; D1 0x837388e = map D1 (both W).
// sole member map<SlotBoundKey,SlotBound> (sizeof 0x18; defs in CDataManager.h).
class CSlotBoundChecker
{
public:
    CSlotBoundChecker() { init(); }
    ~CSlotBoundChecker() {}
    void init();
    void insert(int slot, int capacity, Inven_Item::ITEM_TYPE type, bool a, bool b);
    bool get_slot_bound(Inven_Item::ITEM_TYPE type, int capacity, int& start, int& end) const;
    bool get_item_type(int slot, int capacity, Inven_Item::ITEM_TYPE& outType) const;
private:
    std::map<SlotBoundKey, SlotBound> m_map;
};


class Store
{
public:
    // ORIG 符号（nm -C 核对）：
    //   GetSellItemPrice(Inven_Item&, CItem const*, short, bool, int&) 0x08618ea0
    //   GetSellItemPrice(CUser*, char, short, short, bool, int&, int&)  0x08619008
    //   user_buy_item(CUser*, int, int)                                 0x08618a44
    // 无用户 ctor（ORIG GlobalInstance<Store>::create@0x082711xx 不 call C1，
    // 直接 `movl $0,(%eax); movb $0,0x4(%eax)` = trivial 类 value-init 清零）。
    int GetSellItemPrice(Inven_Item& item, const CItem* pItem, short count,
                         bool flag, int& outPrice);
    int GetSellItemPrice(CUser* user, char invenType, short slot, short count,
                         bool flag, int& outPrice, int& outCount);
    int user_buy_item(CUser* user, int itemId, int count);
    int user_sell_item(CUser* user, char invenType, short slot, short count);
    int repair_equip(CUser* user, char invenType, short slot,
                     unsigned short count, unsigned short& outEndurance);
private:
    // ORIG sizeof(Store)==8：+0x0 int / +0x4 byte（全二进制无读取点，
    // 仅 create 内联构造清零；字段用途【推断：原版遗留占位】）。
    int m_field0;    // +0x00
    char m_field4;   // +0x04
};

class CValueStatistic
{
public:
    struct stValueStatistic
    {
        int m_values[0x47][0x1e];  // +0x00 [level][field]（0x47×0x1e int）
        void reset();
    };
    bool AddValueStatistic(VALUE_STATISTIC_FIELD field, CUser* user, unsigned int value);
    int InsertValueStatistic();
private:
    std::map<ENUM_SERVER_GROUP, stValueStatistic> m_data;
};

#include "PacketGuard.h"  // 权威 PacketGuard（§7 单声明点；本地最小版与
                          // Game/PacketGuard.h 冲突导致重定义）

struct stEquipCondition
{
    // ORIG 布局（Wave6-CUser.FieldMapScout objdump 证据：ctor 0x86937d6 清
    // +0..+3/+4w/+7、置 +6=1；getCompoundUpgrade 0x8682082 读 +1<<7 步进、+2 选组；
    // _ProcCompoundCore 按值传参 t16 → sizeof 0x10）。除 ctor/getCompoundUpgrade
    // 用例外的字段语义为【推断】。
    unsigned char m_upgrade;        // +0x00（强化等级）
    unsigned char m_tableIdx;       // +0x01（升级表行选择 <<7）
    unsigned char m_amplifySelect;  // +0x02（0=Upgrade 表 / 非0=Table2nd）
    unsigned char m_pad3;
    unsigned short m_field4;        // +0x04（ctor 清零 word）
    unsigned char m_flag6;          // +0x06（ctor 置 1）
    unsigned char m_pad7;           // +0x07（ctor 清零）
    unsigned int m_field8;          // +0x08

    // ORIG C1 0x86937d6: per-field stores (+0..+3 movb, +4 movw, +6=1, +7 movb),
    // not memset; +6=1 precedes +7=0 (ORIG 0x086937f3 -> 0x086937fb order).
    stEquipCondition()
    {
        m_upgrade = 0;         // ORIG +0x00 movb $0x0,(%eax)
        m_tableIdx = 0;        // ORIG +0x01
        m_amplifySelect = 0;   // ORIG +0x02
        m_pad3 = 0;            // ORIG +0x03
        m_field4 = 0;          // ORIG +0x04 movw $0x0,0x4(%eax)
        m_flag6 = 1;           // ORIG +0x06 movb $0x1 (before +7)
        m_pad7 = 0;            // ORIG +0x07 movb $0x0
    }
};

// stEquipConditionForChronicle —— ORIG 输出布局（0xc 字节）。
// 写入证据：CInventory::CheckEquipmentItemCondition@085083d0 对 cond 逐偏移
// 写指令（+0/+1/+2/+4w/+6/+7/+8i）；消费方 WongWorkMiscB_A.cpp 按错误码回读。
struct stEquipConditionForChronicle
{
    unsigned char m_upgrade;        // +0x00 ← Inven_Item::GetUpgrade()
    unsigned char m_hasAmplify;     // +0x01（识别出强化选项=1）
    unsigned char m_amplifyType;    // +0x02 ← amp.getAbilityType()
    unsigned char m_pad3;           // +0x03
    unsigned short m_amplifyValue;  // +0x04 ← amp.getAbilityValue()
    unsigned char m_hasField0Item;  // +0x06（Inven_Item 字节0 != 0）
    unsigned char m_reSealCount;    // +0x07 ← Inven_Item::GetReSealCount()
    // b28c-L2: int→unsigned int，与源侧 Inven_Item::m_fieldd（b28c-L6 已实证
    // unsigned）同型 ⇒ 复现 ORIG 0x08508xxx 尾段单段直载
    // `mov -0x48(%ebp),%edx; mov 0x14(%ebp),%eax; mov %edx,0x8(%eax)`
    // （异型时 4.4 恒插 `mov %eax,%edx` 中转，P211 双向开关）。sizeof 0xc/偏移不变。
    unsigned int m_fieldd;          // +0x08 ← Inven_Item::m_fieldd(+0x0d)
};

bool CheckEquipable(const Inven_Item& a, const Inven_Item& b);
int IsEquipable(CUser* user, const CItem* item, int slot);
bool checkStackableLimit(unsigned long itemIdx, int count);
bool IsCreatureItemType(int itemType);
bool isEquipableItemType(int itemType);
int getItemChecksum(int slot, int itemIdx, int a, int b, int c);
int finishItemChecksum(int* sum, int money, int coin, int a, int b);
int my_compare_unsigned_short(const void* a, const void* b);
void write_log_gain_money(eMoneyAddReason reason, int amount, CUser* user);
void write_log_use_money(eMoneySubReason reason, int amount, CUser* user);
char* NumberToString(unsigned int value, int radix);
bool isGainedGoldFromDungeonReason(eMoneyAddReason reason);
extern int _CompareSlot(const void* a, const void* b);
extern int _S_CHARAC_JOB_MASTARY[];
int GetInvenTypeFromItemSpace(ENUM_ITEMSPACE space);  // ORIG 返回 int（INVEN_TYPE），非 const char*
int GetInvenTypeFromItemSpace(ENUM_ITEMSPACE space);   // ORIG W 0x080f7845（switch 映射；定义 CTitleBook.cpp）
// ORIG T 0x0865f1e1 _Z17_checkItemMovablePK5CUser14ENUM_ITEMSPACEimS2_im：
// exchange_interspace(_expand) 前置校验（定义由后续物品域批次补，声明先行）。
bool _checkItemMovable(const CUser* user, ENUM_ITEMSPACE fromSpace, int fromSlot,
                       unsigned long fromItemIdx, ENUM_ITEMSPACE toSpace, int toSlot,
                       unsigned long toItemIdx);

class CInventory
{
public:
    CInventory();
    CInventory(const CInventory& other);
    ~CInventory();
    CInventory& operator=(const CInventory& other);

    void reset();
    void setCopy(const CInventory& other);

    // ---- 简单访问器（weak inline，ORIG 单指令） ----
    unsigned int GetCoin() const;
    void SetCoin(unsigned int coin);
    unsigned int GetPayCoin() const;
    void SetPayCoin(unsigned int coin);
    unsigned int GetEventCoin() const;
    void SetEventCoin(unsigned int coin);
    unsigned int getTotalCoin() const;
    bool IsCoinUsable() const;
    int get_money() const;
    void set_money(unsigned int money);
    int get_inventory_capacity() const;
    void set_inventory_capacity(int capacity);
    void set_parent(CUser* parent);
    void SetInventoryMemory(InventoryMemory* mem);
    user_creature::CCreatureMgr* GetCreatureMgrW();
    const user_creature::CCreatureMgr* GetCreatureMgrR() const;
    WongWork::CAvatarItemMgr* GetAvatarItemMgrW();
    const WongWork::CAvatarItemMgr* GetAvatarItemMgrR() const;

    // ---- 槽/数据访问 ----
    bool CheckValidSlot(int invenType, int slot) const;
    Inven_Item* GetInvenRef(int invenType, int slot);
    const Inven_Item* GetInvenRef(int invenType, int slot) const;
    Inven_Item* GetInvenStart(int invenType) const;
    Inven_Item GetInvenSlot(int invenType, int slot) const;
    int GetInvenSlotByRef(int invenType, int slot, Inven_Item& item);
    void GetInvenData(int invenType, void* out, int size) const;
    int GetInvenData(int itemIdx, Inven_Item& out) const;
    void SetInvenData(int invenType, void* in, int size);
    void ResetSlot(int invenType, int slot);
    void StartEndPos(Inven_Item::ITEM_TYPE type, int& start, int& end) const;
    void getStartEndPos(Inven_Item::ITEM_TYPE type, int& start, int& end) const;
    int GetItemType(int itemIdx) const;
    int GetSlotItemType(int slot) const;
    Inven_Item::ITEM_TYPE GetItemSpace(int invenType, int slot) const;
    int get_empty_slot(INVEN_TYPE invenType, Inven_Item::ITEM_TYPE type) const;
    int get_inven_slot_no(int itemIdx) const;
    int get_avatar_slot_no(int avatarUid) const;
    int get_item_count(int itemIdx) const;
    int get_item_count(int itemIdx, unsigned long param) const;
    int InvenEmptycount(Inven_Item::ITEM_TYPE type) const;
    int QuickEmptyCount() const;
    int GetRemainCapacity(INVEN_TYPE invenType, Inven_Item::ITEM_TYPE type) const;
    int check_slot_empty(int slot) const;
    // ORIG 0x08505019 `mov $0x1,%eax` / 0x08505031 `mov $0x0,%eax`（dword 直写）
    // 且 0x08271062 尾 `call;leave;ret` 无 movzbl ⇒ 返回 int（技巧33）。
    // 返回类型 bool（batch-13 Main 终裁：P22 判据——0x08505667 `if()` 直落
    // `test %al,%al; je` 仅 bool 产；31 调用点 `xor $1,%eax; test %al,%al` 形
    // = bool + `!fn()`。配对律：caller 链同步 bool（GlobalData 外层已翻）。
    bool check_empty_count(Inven_Item::ITEM_TYPE type, int count) const;
    int check_item_exist(int itemIdx) const;
    // 返回类型 bool（batch-13 X1 前提/P22 判据，Main 授权：ORIG 消费点
    // `xor $0x1,%eax; test %al` 直落仅 bool 产；mangled 无返回类型零 ABI）。
    bool possible_to_insert(int itemIdx) const;
    static bool isEquipmentPlaceableAtInventory(int slot);
    // SIGFIX b28c-L2（Main 117 件清单）：ORIG rodata __PRETTY_FUNCTION__ 串实证
    // "bool CInventory::CheckExistItemInInventoryOrEquipment(int, int&, bool&) const"
    // （P174 串定返回类型；返回体只有 0/1）。
    bool CheckExistItemInInventoryOrEquipment(int itemIdx, int& slot, bool& bFound) const;
    int doesExistEquipItem(int itemIdx, int slot);
    int getExistEquipItem(int itemIdx, int slot, int count);
    // SIGFIX b28c-L2：ORIG 串 "bool CInventory::removeDupliacteEquipItem()"。
    bool removeDupliacteEquipItem();
    int CheckNeedItemList(
        const std::vector<std::pair<int, int> >& items) const;
    // ORIG _ZNK10CInventory27CheckEquipmentItemConditionE10INVEN_TYPEsR28stEquipConditionForChronicle
    // 返回错误码 int：0=成功、0x11/0x13/0xd8=失败（085083d0 的 eax 直接值；
    // 调用方 WongWorkMiscB_A.cpp 按 err != 0 → 携码退出消费，非布尔）。
    int CheckEquipmentItemCondition(INVEN_TYPE type, short slot,
                                    stEquipConditionForChronicle& cond) const;
    int CalNeedSlot(int* out,
                    const std::vector<std::pair<int, int> >& items) const;
    int CalBlankSlot(int* out,
                     const std::vector<std::pair<int, int> >& items) const;
    // ORIG _ZNK10CInventory12CalBlankSlotEiRKSt6vectorISt4pairIiiESaIS3_EE T 0x08508536
    int check_vilmark_dungeon_item_exist(int itemIdx) const;
    void ShowAllExistItemsForErrorCheck() const;
    void checkEquipmentState(int invenType, int slot, int& a, int& b, int& c,
                            int& d, int& e, int& f);
    int CheckEquipmentItemCondition(   // ORIG 0x08507f1a：返回错误码 0/1/0x11/0x12/0x13/0xd5/0xd8（bool 会截断，LinkF1 修正）
        const std::vector<std::pair<int, int> >& items, stEquipCondition& cond,
        Inven_Item& outItem) const;
    // SIGFIX b28c-L2：ORIG 串 "bool CInventory::MakeItemList(INVEN_TYPE, void *) const"。
    bool MakeItemList(INVEN_TYPE invenType, void* out) const;
    bool MakeItemPacket(INVEN_TYPE invenType, int slot, PacketGuard& packet) const;
    int MakeSecuRewarditemPacket(INVEN_TYPE invenType, int slot, PacketGuard& packet) const;
    // ORIG _ZNK10CInventory13MakeEquipListEPvb13ENUM_USERINFO14ENUM_EQUIPSLOT：
    // out 为空返回 0，否则写完列表返回 1（eax）。
    int MakeEquipList(void* out, bool flag, ENUM_USERINFO userInfo,
                      ENUM_EQUIPSLOT equipSlot) const;
    void getHashCode(unsigned char* out);
    int getPackedCodeArray1(unsigned long* out, int maxCount);
    size_t getSortedKeyArray(unsigned long* out, int maxCount);
    // ORIG 0x0850ae7a：返回 finishItemChecksum 的 eax（尾调用值流出）；mangled
    // 不编码返回类型，符号不变。CUser::getCurCharacInvenCheckSum 直接返回其结果。
    int makeCheckSum(int a, int b) const;
    void GetExpertJobAddtionalExp(ENUM_EXPERT_JOB_TYPE type, int param,
                                  STItemScript::SEXPERTJOB::stExpertJobAdditionalExp& out) const;
    float GetExpertJobCompoundMaterialVariation(ENUM_EXPERT_JOB_TYPE type, int param) const;
    float GetExpertJobCompoundRateVariation(ENUM_EXPERT_JOB_TYPE type, int param) const;
    void GetExpertJobCompoundResultVariation(
        ENUM_EXPERT_JOB_TYPE type, int param,
        STItemScript::SEXPERTJOB::stExpertCompoundResultVariation& out) const;
    float GetExpertJobSelfDisjointBigWinRate(ENUM_EXPERT_JOB_TYPE type, int param) const;
    float GetExpertJobSelfDisjointResultVariation(ENUM_EXPERT_JOB_TYPE type, int param) const;
    unsigned char getEquipWeaponUpgradeSeparate() const;
    int getValidMoney(int add);

    // ---- 货币 ----
    int gain_money(int amount, eMoneyAddReason reason, bool bLog, int param);
    bool use_money(int amount, eMoneySubReason reason, bool bLog);  // ORIG bool：调用点 xor $0x1;test %al,%al（WarRoom 086bfd14 等）
    void UpdateDailyCoin(int value);
    int UseCoin(eCoinSubReason reason);
    int UseUpCoin(unsigned int amount);

    // ---- 物品操作 ----
    int use_item(int slot, int count, int invenType);
    int try_use_item(int slot, int count, int invenType) const;
    int use_equip(int slot);
    bool delete_item(INVEN_TYPE invenType, int slot, int count, eItemDelReason reason,
                     bool bLog);  // ORIG bool：调用点 xor $0x1;test %al,%al
    void update_item(INVEN_TYPE invenType, int slot, Inven_Item item);
    int move_item(INVEN_TYPE invenType, int slot, INVEN_TYPE dstType, int dstSlot);
    int MoveItemToEmptySlotOfInventory(int slot, int start, int end);
    int ChangeEquip(INVEN_TYPE invenType, int slot, int equipSlot);
    int insertItemIntoInventory(Inven_Item item, eItemAddReason reason, bool bLog,
                                bool b);
    int insertItemIntoEquipment(Inven_Item item, eItemAddReason reason, bool bLog,
                                bool b);
    int stackInvenItemIntoQuickSlot(Inven_Item item, eItemAddReason reason, bool bLog);
    int insertInvenItemIntoQuickSlot(Inven_Item item, eItemAddReason reason, bool bLog);
    int stackInvenItemIntoCorrespondingSpace(Inven_Item item, eItemAddReason reason,
                                             bool bLog);
    int insertInvenItemIntoCoresspondingSpace(Inven_Item item, eItemAddReason reason,
                                              bool bLog);
    int tryInsertItemIntoInventory(Inven_Item item) const;
    int tryInsertItemIntoEquipment(Inven_Item item) const;
    int tryStackInvenItemIntoQuickSlot(Inven_Item item) const;
    int tryInsertInvenItemIntoQuickSlot(Inven_Item item) const;
    int tryStackInvenItemIntoCorrespondingSpace(Inven_Item item) const;
    int tryInsertInvenItemIntoCoresspondingSpace(Inven_Item item) const;
    int insert_item_special_slot(Inven_Item item, int a, int b,
                                 eMoneyAddReason moneyReason, eItemAddReason itemReason);
    int insertAvatarIntoInventory(Inven_Item item, eAvatarItemAddReason reason, bool bLog);
    int insertAvatarIntoSpecificSlot(Inven_Item item, int slot,
                                     eAvatarItemAddReason reason, bool bLog);
    int tryInsertAvatarIntoInventory(Inven_Item item) const;
    int getAvatarEmptySlot(int itemIdx, int param) const;
    int insertItemIntoCreature(Inven_Item& item, eItemAddReason reason, bool bLog,
                               bool b);
    int insertItemIntoCreatureStackable(Inven_Item item, eItemAddReason reason, bool bLog);
    int insertItemIntoCreatureEquipment(Inven_Item item, eItemAddReason reason, bool bLog);
    int insertItemIntoArtifactEquipment(Inven_Item item, eItemAddReason reason, bool bLog);
    int insertCreatureIntoSpecificSlot(Inven_Item item, int slot, eItemAddReason reason,
                                       bool bLog);
    int tryInsertItemIntoCreature(Inven_Item& item) const;
    int GetEmptyCreatureSlot(Inven_Item& item) const;
    int GetExistCreatureSlot(Inven_Item& item) const;
    void WriteCreatureLog(INVEN_TYPE invenType, int a, int b, int c, int d,
                          eItemAddReason reason) const;
    int RemoveKCItem();
    static int RemoveKCItem(Inven_Item* a, Inven_Item* b);
    int RemoveFPItem();
    static int RemoveFPItem(Inven_Item* a, Inven_Item* b);
    bool IsExistStackable(int itemIdx, Inven_Item::ITEM_TYPE type) const;
    // ORIG 0x850a082 只读 0xc(%ebp)、caller 0x85098fb 无 this outgoing → static。
    static bool IsOverStackableItem(int itemIdx, unsigned char count);

    // ---- 事件物品 ----
    int insert_event_items(const std::vector<std::pair<int, int> >& items,
                           std::vector<int>& outSlots, eMoneyAddReason moneyReason,
                           eItemAddReason itemReason, bool bLog, const char* a,
                           const char* b);
    int tryInsertEventItems(const std::vector<std::pair<int, int> >& items);
    int delete_event_items(const std::vector<std::pair<int, int> >& items,
                           std::vector<int>& outSlots, bool& bFound,
                           eMoneySubReason moneyReason, eItemDelReason itemReason,
                           bool bLog);
    bool tryDeleteEventItems(const std::vector<std::pair<int, int> >& items);
    // ORIG 返回 out.size()（accept_quest 使用返回 %eax；Ghidra 误标 void，见
    // docs/class_func_reports/CInventory/erase_repeated_item.md 尾部 size() 调用）
    int erase_repeated_item(const std::vector<std::pair<int, int> >& items,
                            std::vector<std::pair<int, int> >& out);

    // ---- 排序 ----
    int sort(INVEN_TYPE invenType);
    int SortInvenSlot(Inven_Item::ITEM_TYPE type);
    void MoveClearAvatar(int a, int b, int c, int d, int e, int f);
    void SendAvatarEvent(int a, int b, int c, int d, int e) const;
    void SetClearAvatar(int a, int b);
    void SetClearAvatar(unsigned long* arr);
    int GetClearAvatar(int a) const;
    void GetClearAvatar(unsigned long* arr) const;
    bool IsEquipAvatar() const;
    bool isAnyExpiredAvatarItem() const;
    void ExpiredAvatarItem(PacketGuard* packet);
    int AddAvatarItem(int itemIdx, int avatarUid, bool b, char c, char d,
                      const char* agency, eAvatarItemAddReason reason, bool e,
                      unsigned short f);
    // ORIG 0x08509e78：m_pParent 空返回 false，投递 SIG 后返回 true（eax）。
    bool _SaveAvatarItem(int slot, int expireDate, bool b, char c,
                         const stAvatarEmblemInfo_t& emblem, const char* agency,
                         unsigned long hidden, const stAvatarExpansionInfo_t& expansion);
    void CheckNFix();
    int checkToFix() const;
    void ChecknFixWineAndMoru();
    int FindItemLock(unsigned char b, ENUM_ITEMSPACE& space, int& slot) const;
    void SendItemLockList(const Inven_Item* items, int count,
                          ENUM_ITEMSPACE space) const;
    void SendItemLockListInven() const;
    void SendItemLockListAvatar() const;
    void SendItemLockListCreature() const;

    struct AvatarVisibleData
    {
        void init();
        int m_field0;    // +0x00
        short m_field4;  // +0x04
        short m_field6;  // +0x06
    };

public:
    // ---- 成员布局 ----
    CUser* m_pParent;             // +0x00
    int m_money;                  // +0x04
    unsigned int m_coin;          // +0x08（ORIG SetCoin 装载序=同 signedness 直存 ⇒ unsigned）
    unsigned int m_payCoin;       // +0x0c
    unsigned int m_eventCoin;     // +0x10
    int m_field14;                // +0x14
    unsigned int m_inventoryCapacity; // +0x18（ORIG set_inventory_capacity arg先 ⇒ int形参存unsigned成员）
    Inven_Item m_invenItem[26];   // +0x1c
    Inven_Item* m_pEquipSlot;     // +0x650
    Inven_Item* m_pAvatarSlot;    // +0x654
    AvatarCoin m_avatarCoin;      // +0x658
    // b28c-L2: int→unsigned long（与 SetClearAvatar(unsigned long*)/GetClearAvatar
    // 的形参同型）。证据：ORIG 0x84e99b4 SetClearAvatar(Pm) store 段 `mov (%eax),%edx`
    // 直载 + `add $0x194,%ecx`（探针 l2_probe g3 同型形逐条一致，异型形恒多中转）。
    // sizeof/偏移不变（4 字节 ×10，@+0x65c）。
    unsigned long m_clearAvatar[10];  // +0x65c
    AvatarVisibleData m_avatarVisible[12];  // +0x684
    Inven_Item* m_pCreatureSlot;  // +0x6e4
    user_creature::CCreatureMgr m_creatureMgr;  // +0x6e8
    WongWork::CAvatarItemMgr m_avatarItemMgr;   // +0x734（权威类型 0x84，CInventory 总 0x7b8）
};

// ORIG 调用点怪癖：ChangeEquip 以 4 参自由函数形态调用 GetInvenSlot
// （压栈 out,this,1,slot），符号实际指向成员 _ZNK10CInventory12GetInvenSlotEii。
// 为还原逐字节压栈形态保留此声明；链接桩由主 agent 提供。
int GetInvenSlot(Inven_Item* out, CInventory* self, int invenType, int slot);

#endif  // GAME_CINVENTORY_H_
