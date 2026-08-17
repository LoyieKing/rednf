#ifndef GAME_CINVENTORY_H_
#define GAME_CINVENTORY_H_

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
// Inven_Item 尺寸 0x3d；equip 312 / avatar 105 / creature 242 槽。
// CItem/CStackableItem/CEquipItem/Inven_Item/CDataManager 使用权威头（I1 批次），
// 本头不再重复声明；CUser/CUserCharacInfo/cUserHistoryLog 等尚无权威头的类保留
// 最小本地声明，成员签名按 ORIG nm 符号逐一核对（const 性/枚举类型名/引用）。
// ============================================================================

#include <stddef.h>
#include <utility>
#include <vector>

#include "CItem.h"
#include "CStackableItem.h"
#include "CEquipItem.h"
#include "CDataManager.h"
#include "InterfacePacketBuf.h"
#include "CUser.h"          // 权威 CUser/cUserHistoryLog/共享枚举（G1-3 统一）

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
    eItemAddReason_8 = 8
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

enum SECURITY_PROTCTION
{
    SECURITY_PROTCTION_0 = 0
};

enum VALUE_STATISTIC_FIELD
{
    VALUE_STATISTIC_FIELD_0 = 0
};

// ---- 头像信息（ORIG W 0x081506e8 / 0x0833421e；布局与 AuctionItem.h 一致） ----
#pragma pack(push, 1)
struct stAvatarEmblemInfo_t
{
    void init();  // ORIG W 0x081506e8（独立 TU 定义）

    struct
    {
        unsigned short emblemSocketType_;  // +0x00
        int emblemItemIdx_;                // +0x02
    } stEmblemSocket[5];                   // +0x00，总 30 字节
};
#pragma pack(pop)

struct stAvatarExpansionInfo_t
{
    void init();  // ORIG W 0x0833421e（独立 TU 定义）

    short color[2];  // +0x00，总 4 字节
};

struct stEquipCondition;
struct stEquipConditionForChronicle;

struct md5_context
{
    char m_pad[224];
};
void md5_starts(md5_context* ctx);
void md5_update(md5_context* ctx, unsigned char* data, int len);
void md5_finish(md5_context* ctx, unsigned char* out);

namespace OS_API
{
int GetDateTimeTick();
}

namespace user_creature
{
class CCreatureMgr
{
public:
    void Reset();
    void SetUser(CUser* user);
    void TurnStomach(bool fighting);
    int UseItem(Inven_Item* item, int slot);  // ORIG _ZN13user_creature12CCreatureMgr7UseItemEP10Inven_Itemi
    bool SwapCreatureItem(Inven_Item* a, int slotA, int typeA,
                          Inven_Item* b, int slotB, int typeB);
    char m_pad[0x4c];  // ORIG 尺寸 0x4c（+0x6e8..+0x733）
};
}

namespace WongWork
{
class CGenUniqueNo
{
public:
    enum ENUM_IPGNO_TYPE
    {
        ENUM_IPGNO_TYPE_6 = 6
    };
    static void genIPGNo(ENUM_IPGNO_TYPE type, unsigned int characNo, char* out);
};

class CAvatarItemMgr
{
public:
    void Reset();
    unsigned long RegistNotAssignedAvatarItem(int itemIdx, int tick, const char* ipgAgency,
                                              bool b, const stAvatarEmblemInfo_t& emblem);
    int GetExpireDate(int avatarUid) const;
    const char* GetIPGAgencyNo(int addInfo) const;
    char m_pad[0x4c];  // +0x734 起
};

}

class AvatarCoin
{
public:
    AvatarCoin();
    void Set(unsigned int value);
    void Add(unsigned int value);
    unsigned int Use();
    char m_pad[4];  // ORIG 尺寸 4（+0x658..+0x65b）
};

// ---- 外部依赖（其它 TU 提供；成员签名按 ORIG nm 核对） ----
class CSlotBoundChecker;
class CExpandEquipslot;
class Store;
class CValueStatistic;
class PacketGuard;
class CSecu_ProtectionField;

extern Store* G_Store();
extern CValueStatistic* GetInstanceValueStatistic();
extern CSlotBoundChecker* g_slotBoundChecker;

class CSecu_ProtectionField
{
public:
    // ORIG 定义符号为 2 参（_ZN21CSecu_ProtectionField5CheckEP5CUser18SECURITY_PROTCTION），
    // 但 ChangeEquip 调用点按 3 参形态（field,user,type）压栈——按调用点 ABI 声明，
    // 链接桩由主 agent 提供。
    static int Check(CSecu_ProtectionField* field, CUser* user, SECURITY_PROTCTION type);
};

namespace GlobalData
{
extern CSecu_ProtectionField* s_pSecuProtectionField;
}

class CSlotBoundChecker
{
public:
    bool get_slot_bound(Inven_Item::ITEM_TYPE type, int capacity, int& start, int& end) const;
    bool get_item_type(int slot, int capacity, Inven_Item::ITEM_TYPE& outType) const;
};

class CExpandEquipslot
{
public:
    Inven_Item* GetInvenRefR(INVEN_TYPE type, int slot) const;
    Inven_Item* getExpandEquipslotW(ENUM_EQUIPSLOT slot);
    bool FindItemLock(unsigned char b, ENUM_ITEMSPACE& space, int& slot) const;
};

class Store
{
public:
    void GetSellItemPrice(Inven_Item& item, const CItem* pItem, short count, bool flag,
                          int& outPrice);
};

class CValueStatistic
{
public:
    void AddValueStatistic(VALUE_STATISTIC_FIELD field, CUser* user, unsigned int value);
};

class PacketGuard : public InterfacePacketBuf
{
public:
    PacketGuard();
    ~PacketGuard();

private:
    int m_nField4;
    char m_cField8;
};

struct stEquipCondition
{
};

struct stEquipConditionForChronicle
{
};

bool CheckEquipable(const Inven_Item& a, const Inven_Item& b);
int IsEquipable(CUser* user, const CItem* item, int slot);
bool checkStackableLimit(unsigned long itemIdx, int count);
bool IsCreatureItemType(int itemType);
bool isEquipableItemType(int itemType);
int getItemChecksum(int slot, int itemIdx, int a, int b, int c);
int finishItemChecksum(int* sum, int money, int coin, int a, int b);
int my_compare_unsigned_short(const void* a, const void* b);
void write_log_gain_money(int reason, int amount, unsigned int user);
void write_log_use_money(int reason, int amount, unsigned int user);
char* NumberToString(unsigned int value, int radix);
bool isGainedGoldFromDungeonReason(eMoneyAddReason reason);
extern int _CompareSlot(const void* a, const void* b);
extern int _S_CHARAC_JOB_MASTARY[];
const char* GetInvenTypeFromItemSpace(ENUM_ITEMSPACE space);
int GetInvenTypeFromItemSpace(int space);

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
    int GetInvenSlot(int invenType, int slot) const;
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
    int check_empty_count(Inven_Item::ITEM_TYPE type, int count) const;
    int check_item_exist(int itemIdx) const;
    int possible_to_insert(int itemIdx) const;
    bool isEquipmentPlaceableAtInventory(int slot);
    int CheckExistItemInInventoryOrEquipment(int itemIdx, int& slot, bool& bFound) const;
    int doesExistEquipItem(int itemIdx, int slot);
    int getExistEquipItem(int itemIdx, int slot, int count);
    int removeDupliacteEquipItem();
    int CheckNeedItemList(
        const std::vector<std::pair<int, int> >& items) const;
    int CalBlankSlot(int* out,
                     const std::vector<std::pair<int, int> >& items) const;
    int CalNeedSlot(int* out,
                    const std::vector<std::pair<int, int> >& items) const;
    int check_vilmark_dungeon_item_exist(int itemIdx) const;
    void ShowAllExistItemsForErrorCheck() const;
    void checkEquipmentState(int invenType, int slot, int& a, int& b, int& c,
                            int& d, int& e, int& f);
    bool CheckEquipmentItemCondition(
        const std::vector<std::pair<int, int> >& items, stEquipCondition& cond,
        Inven_Item& outItem) const;
    bool CheckEquipmentItemCondition(INVEN_TYPE type, short slot,
                                     stEquipConditionForChronicle& cond) const;
    int MakeItemList(int invenType, void* out) const;
    void MakeItemPacket(int invenType, int slot, PacketGuard& packet) const;
    void MakeSecuRewarditemPacket(int invenType, int slot, PacketGuard& packet) const;
    void MakeEquipList(void* out, bool flag, ENUM_USERINFO userInfo,
                       ENUM_EQUIPSLOT equipSlot) const;
    void getHashCode(unsigned char* out);
    int getPackedCodeArray1(unsigned long* out, int maxCount);
    size_t getSortedKeyArray(unsigned long* out, int maxCount);
    void makeCheckSum(int a, int b) const;
    void GetExpertJobAddtionalExp(ENUM_EXPERT_JOB_TYPE type, int param,
                                  STItemScript::SEXPERTJOB::stExpertJobAdditionalExp& out) const;
    float GetExpertJobCompoundMaterialVariation(ENUM_EXPERT_JOB_TYPE type, int param) const;
    float GetExpertJobCompoundRateVariation(ENUM_EXPERT_JOB_TYPE type, int param) const;
    void GetExpertJobCompoundResultVariation(
        ENUM_EXPERT_JOB_TYPE type, int param,
        STItemScript::SEXPERTJOB::stExpertCompoundResultVariation& out) const;
    float GetExpertJobSelfDisjointBigWinRate(ENUM_EXPERT_JOB_TYPE type, int param) const;
    float GetExpertJobSelfDisjointResultVariation(ENUM_EXPERT_JOB_TYPE type, int param) const;
    int getEquipWeaponUpgradeSeparate() const;
    int getValidMoney(int add);

    // ---- 货币 ----
    int gain_money(int amount, eMoneyAddReason reason, bool bLog, int param);
    int use_money(int amount, eMoneySubReason reason, bool bLog);
    void UpdateDailyCoin(int value);
    int UseCoin(eCoinSubReason reason);
    int UseUpCoin(unsigned int amount);

    // ---- 物品操作 ----
    int use_item(int slot, int count, int invenType);
    int try_use_item(int slot, int count, int invenType) const;
    int use_equip(int slot);
    int delete_item(INVEN_TYPE invenType, int slot, int count, eItemDelReason reason,
                    bool bLog);
    void update_item(INVEN_TYPE invenType, int slot, Inven_Item item);
    int move_item(int invenType, int slot, int dstType, int dstSlot);
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
    bool IsOverStackableItem(int itemIdx, unsigned char count);

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
    void erase_repeated_item(const std::vector<std::pair<int, int> >& items,
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
    void _SaveAvatarItem(int slot, int expireDate, bool b, char c,
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
    int m_coin;                   // +0x08
    int m_payCoin;                // +0x0c
    int m_eventCoin;              // +0x10
    int m_field14;                // +0x14
    int m_inventoryCapacity;      // +0x18
    Inven_Item m_invenItem[26];   // +0x1c
    Inven_Item* m_pEquipSlot;     // +0x650
    Inven_Item* m_pAvatarSlot;    // +0x654
    AvatarCoin m_avatarCoin;      // +0x658
    int m_clearAvatar[10];        // +0x65c
    AvatarVisibleData m_avatarVisible[12];  // +0x684
    Inven_Item* m_pCreatureSlot;  // +0x6e4
    user_creature::CCreatureMgr m_creatureMgr;  // +0x6e8
    WongWork::CAvatarItemMgr m_avatarItemMgr;   // +0x734
};

// ORIG 调用点怪癖：ChangeEquip 以 4 参自由函数形态调用 GetInvenSlot
// （压栈 out,this,1,slot），符号实际指向成员 _ZNK10CInventory12GetInvenSlotEii。
// 为还原逐字节压栈形态保留此声明；链接桩由主 agent 提供。
int GetInvenSlot(Inven_Item* out, CInventory* self, int invenType, int slot);

#endif  // GAME_CINVENTORY_H_
