#include "LogManager.h"
// df_game_r CInventory 还原（第八批 H3，2026-08-16）。
// 布局/语义以 docs/class_func_reports/CInventory.md 逐条对照（AE 口径）。
// 依赖类（Inven_Item/CItem/CUser/CAvatarItemMgr/CCreatureMgr 等）由
// H1/H2/后续批次提供，本 TU 仅声明引用。

#include "CInventory.h"

#include <stdlib.h>
#include <string.h>

#include "GameTypes.h"
#include "STItemScript.h"
#include "CCirculationStatistic.h"
#include "LogManager.h"
#include "CTitleBook.h"   // item_lock::CItemLock::MakeItemLockPacket（TSV）
#include "SigTypes.h"     // SIG_LOAD_*（GameWorld.h→CUser.h 依赖；须先于其 include）
#include "GameWorld.h"    // GameWorld::IsEnchantRevisionChannel/IsIntegratedPvPBaseChannel
#include "ServiceRestrictManager.h"   // ServiceRestrictManager::isRestricted（move_item 封印限制）
#include "Arad_DataManager.h"         // ARAD::Singleton<ServiceRestrictManager>::Get()
#include "CSystemTime.h"              // CSystemTime::getCurSec（解封期限/头像剩余天数）
#include "CMailBoxHelper.h"           // WongWork::CMailBoxHelper 静态邮件（insert_event_items 兜底）
#include "RDARScriptStringManager.h"  // g_scriptStringManager_/findString（邮件标题/正文 key）
#include "CCubeStatistic.h"           // GetInstanceCubeStatistic/collectCubeStatistics（reason 8 统计）
#include <algorithm>                  // std::swap<int>/std::swap<INVEN_TYPE>

// GetIntegratedPvPItemAttr(3 参)/_getIntegratedPvPItemAttr 的自由函数（PvP 物品属性，
// ORIG 0x84fc651/0x84fc541）。1 参版本定义于 CDataManagerScripts.cpp，3 参版本见下方。
extern int _getIntegratedPvPItemAttr(const Inven_Item& item);

// 空槽占位 Inven_Item 全局（ORIG 0x943ddc0；定义于 CAccountCargo.cpp）
extern Inven_Item g_emptySlot;

// ============================================================================
// 三个自由辅助函数（ORIG 全局符号，Money 簇自由函数）
//   isGainedGoldFromDungeonReason  @0x850d32a (W)  : reason==4 || reason==0xf
//   write_log_gain_money           @0x84febd9 (T)  : AddGold + 流通统计跳表
//   write_log_use_money            @0x84fed9b (T)  : SubGold + 流通/Value统计跳表
// 语义依据 CInventory.md 逐条对照；amount<0 日志分支忠实 ORIG logFormat 串。
// 注：ORIG 各 case 额外调用 CUser::AddMoneyPlus/AddMoneyMinus(@0x868f266/
//   @0x868f310)，该符号尚未在源实现，此处省略以避免引入新未定义。
// ============================================================================
bool isGainedGoldFromDungeonReason(eMoneyAddReason reason)
{
    // ORIG 0x867d9a2：switch 形态（先 mov 参数到寄存器再 cmp/je 链）。
    switch ((int)reason)
    {
    case 4:
    case 0xf:
        return true;
    }
    return false;
}

void write_log_gain_money(eMoneyAddReason reason, int amount, CUser* user)
{
    // ORIG 0x84febd9：if (amount < 1) { if (amount < 0) logFormat } else { AddGold + switch }。
    // switch 体布局序 = ORIG 块序（jump table 实证）：case 4,0,3,0xf,0xa,0x1a。
    if (amount < 1)
    {
        if (amount < 0)
        {
            LogManager::logFormat(1, "inventory.cpp", __PRETTY_FUNCTION__, 0xd71,
                "write_log_gain_money() reason(%d), gold(%d), gold(%d)",
                reason, amount, user->get_charac_level());
            return;
        }
    }
    else
    {
        if (user != 0)
        {
            user->GetGoldControl()->AddGold((unsigned int)amount, reason);
        }
        switch ((int)reason)
        {
        case 4:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0, user, (unsigned int)amount);
            user->AddMoneyPlus(amount);
            break;
        case 0:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)2, user, (unsigned int)amount);
            user->AddMoneyPlus(amount);
            break;
        case 3:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)3, user, (unsigned int)amount);
            user->AddMoneyPlus(amount);
            break;
        case 0xf:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)6, user, (unsigned int)amount);
            break;
        case 0xa:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)7, user, (unsigned int)amount);
            break;
        case 0x1a:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0xe, user, (unsigned int)amount);
            // ORIG switch 末尾带 default: break（产生 jmp+nop 尾部形态）
        default:
            break;
        }
    }
}

void write_log_use_money(eMoneySubReason reason, int amount, CUser* user)
{
    // ORIG 0x84fed9b：先无条件取 charac_level 存局部，再 if(amount<1){if(<0)log}else{...}。
    // switch 体布局序 = ORIG 块序（jump table 实证）。
    int level = user->get_charac_level();
    if (amount < 1)
    {
        if (amount < 0)
        {
            LogManager::logFormat(1, "inventory.cpp", __PRETTY_FUNCTION__, 0xde9,
                "write_log_use_money() reason(%d), gold(%d), gold(%d)",
                reason, amount, level);
            return;
        }
    }
    else
    {
        if (user != 0)
        {
            user->GetGoldControl()->SubGold((unsigned int)amount, reason);
        }
        switch ((int)reason)
        {
        case 0:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x12, user, (unsigned int)amount);
            user->AddMoneyMinus(amount);
            break;
        case 0xa:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x13, user, (unsigned int)amount);
            user->AddMoneyMinus(amount);
            break;
        case 1:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x14, user, (unsigned int)amount);
            user->AddMoneyMinus(amount);
            break;
        case 0x14:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x16, user, (unsigned int)amount);
            GetInstanceValueStatistic()->AddValueStatistic(
                (VALUE_STATISTIC_FIELD)0x18, user, (unsigned int)amount);
            user->AddMoneyMinus(amount);
            break;
        case 6:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x18, user, (unsigned int)amount);
            break;
        case 0xd:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x19, user, (unsigned int)amount);
            user->AddMoneyMinus(amount);
            break;
        case 4:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x1a, user, (unsigned int)amount);
            user->AddMoneyMinus(amount);
            break;
        case 0x10:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x1b, user, (unsigned int)amount);
            break;
        case 0xb:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x1e, user, (unsigned int)amount);
            break;
        case 0x13:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x1f, user, (unsigned int)amount);
            break;
        case 0x16:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x20, user, (unsigned int)amount);
            break;
        case 0xc:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x21, user, (unsigned int)amount);
            break;
        case 0xf:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x23, user, (unsigned int)amount);
            break;
        case 0x15:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x24, user, (unsigned int)amount);
            break;
        case 0x22:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x25, user, (unsigned int)amount);
            break;
        case 0x21:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x27, user, (unsigned int)amount);
            break;
        case 0x24:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x28, user, (unsigned int)amount);
            break;
        case 0x2a:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x29, user, (unsigned int)amount);
            break;
        case 0x2b:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x2b, user, (unsigned int)amount);
            GetInstanceValueStatistic()->AddValueStatistic(
                (VALUE_STATISTIC_FIELD)0x1d, user, (unsigned int)amount);
            break;
        case 0x2c:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x2c, user, (unsigned int)amount);
            break;
        case 0x2d:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x2d, user, (unsigned int)amount);
            break;
        case 0x2e:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x2e, user, (unsigned int)amount);
            user->AddMoneyMinus(amount);
            break;
        case 0x2f:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x2f, user, (unsigned int)amount);
            // ORIG switch 末尾带 default: break（产生 jmp+nop 尾部形态）
        default:
            break;
        }
    }
}


// ---- 发包依赖（StreamPool/CStreamGuard 经 GameBase.h 链已可见；MsgQueueMgr/GlobalData 本地声明） ----
class MsgQueueMgr
{
public:
    enum QUEUE_IDX
    {
        NETWORK_Q = 0,
        DISPATCH_Q = 1,
        DB_Q = 2,
        TIMER_Q = 3,
        LOGDB_Q = 4
    };

    void put(QUEUE_IDX queue_idx, CStreamGuard& guard);
};

namespace GlobalData
{
extern StreamPool* s_stream_pool;
extern MsgQueueMgr* s_msgq_mgr;
extern CSystemTime s_systemTime_;   // ORIG B 0x941f714（GlobalData.cpp 定义）
}
// ===================== 简单访问器（ORIG weak inline 形态） =====================


// 原 GetInvenSlotSelfHelper（ORIG 隐藏返回指针形态）已并入成员
// CInventory::GetInvenSlot（CInventory.h 返回 Inven_Item），删除替身。

unsigned int CInventory::GetCoin() const
{
    return m_coin;
}

void CInventory::SetCoin(unsigned int coin)
{
    m_coin = coin;
}

unsigned int CInventory::GetPayCoin() const
{
    return m_payCoin;
}

void CInventory::SetPayCoin(unsigned int coin)
{
    m_payCoin = coin;
}

unsigned int CInventory::GetEventCoin() const
{
    return m_eventCoin;
}

void CInventory::SetEventCoin(unsigned int coin)
{
    m_eventCoin = coin;
}

unsigned int CInventory::getTotalCoin() const
{
    return m_coin + m_eventCoin + m_payCoin;
}

bool CInventory::IsCoinUsable() const
{
    return m_coin != 0 || m_payCoin != 0 || m_eventCoin != 0;
}

int CInventory::get_money() const
{
    return m_money;
}

void CInventory::set_money(unsigned int money)
{
    m_money = money;
}

int CInventory::get_inventory_capacity() const
{
    return m_inventoryCapacity;
}

void CInventory::set_inventory_capacity(int capacity)
{
    m_inventoryCapacity = capacity;
}

void CInventory::set_parent(CUser* parent)
{
    m_pParent = parent;
    m_creatureMgr.SetUser(parent);
}

void CInventory::SetInventoryMemory(InventoryMemory* mem)
{
    m_pEquipSlot = (Inven_Item*)mem;
    m_pAvatarSlot = (Inven_Item*)((char*)mem + 0x4a58);
    m_pCreatureSlot = (Inven_Item*)((char*)mem + 0x635d);
}

user_creature::CCreatureMgr* CInventory::GetCreatureMgrW()
{
    return &m_creatureMgr;
}

const user_creature::CCreatureMgr* CInventory::GetCreatureMgrR() const
{
    return &m_creatureMgr;
}

WongWork::CAvatarItemMgr* CInventory::GetAvatarItemMgrW()
{
    return &m_avatarItemMgr;
}

const WongWork::CAvatarItemMgr* CInventory::GetAvatarItemMgrR() const
{
    return &m_avatarItemMgr;
}

void CInventory::AvatarVisibleData::init()
{
    m_field6 = 0;
    m_field4 = m_field6;
    m_field0 = m_field4;
}

int CInventory::GetClearAvatar(int idx) const
{
    if (idx >= 0 && idx <= 9)
        return m_clearAvatar[idx];
    return 0;
}

void CInventory::SetClearAvatar(int idx, int value)
{
    if (idx >= 0 && idx <= 9)
    {
        m_clearAvatar[idx] = value;
    }
}

void CInventory::GetClearAvatar(unsigned long* out) const
{
    for (int i = 0; i < 10; i++)
    {
        out[i] = m_clearAvatar[i];
    }
}

void CInventory::SetClearAvatar(unsigned long* arr)
{
    for (int i = 0; i < 10; i++)
    {
        m_clearAvatar[i] = arr[i];
    }
}

// ORIG 0x8504d0d：比较两个槽位排序键；键 = 槽空(addInfo==0)?0xb:m_field1(ITEM_TYPE)。
int _CompareSlot(const void* va, const void* vb)
{
    const Inven_Item* a = (const Inven_Item*)va;
    const Inven_Item* b = (const Inven_Item*)vb;
    int sa = a->m_addInfo != 0 ? a->m_field1 : 0xb;
    int sb = b->m_addInfo != 0 ? b->m_field1 : 0xb;
    if (sa < sb) return -1;
    if (sa > sb) return 1;
    return 0;
}

// ORIG C1 0x822d52c W：26 个 m_invenItem 逐元素构造（+0x1c 步进 0x3d）→
// m_pEquipSlot(+0x650)/m_pAvatarSlot(+0x654)/m_pCreatureSlot(+0x6e4) 置 0 →
// AvatarCoin C1(+0x658) → CCreatureMgr C1(+0x6e8) → CAvatarItemMgr C1(+0x734)
// → reset()。成员初始化列表按声明序即复现该序列。
CInventory::CInventory()
    : m_invenItem(), m_pEquipSlot(0), m_pAvatarSlot(0), m_avatarCoin(),
      m_pCreatureSlot(0), m_creatureMgr(), m_avatarItemMgr()
{
    reset();
}   // 三槽指针(+0x650/+0x654/+0x6e4)声明先于 AvatarCoin/CCreatureMgr 自动构造，故入 init-list(ORIG 序)

// ORIG D1 0x822d600 W：CAvatarItemMgr D1(+0x734) → CCreatureMgr D1(+0x6e8)；
// 其余成员平凡析构，由编译器逆序发射。
CInventory::~CInventory()
{
}
CInventory::CInventory(const CInventory& other)
{
    m_pParent = other.m_pParent;
    m_money = other.m_money;
    m_coin = other.m_coin;
    m_payCoin = other.m_payCoin;
    m_eventCoin = other.m_eventCoin;
    m_field14 = other.m_field14;
    m_inventoryCapacity = other.m_inventoryCapacity;
    for (int i = 25; i >= 0; i--)
    {
        m_invenItem[i] = other.m_invenItem[i];
    }
    m_pEquipSlot = other.m_pEquipSlot;
    m_pAvatarSlot = other.m_pAvatarSlot;
    m_avatarCoin = other.m_avatarCoin;
    for (int i = 0; i < 10; i++)
    {
        m_clearAvatar[i] = other.m_clearAvatar[i];
    }
    for (int i = 0; i < 12; i++)
    {
        m_avatarVisible[i] = other.m_avatarVisible[i];
    }
    m_pCreatureSlot = other.m_pCreatureSlot;
    m_creatureMgr = other.m_creatureMgr;
    m_avatarItemMgr = other.m_avatarItemMgr;
}

CInventory& CInventory::operator=(const CInventory& other)
{
    m_pParent = other.m_pParent;
    m_money = other.m_money;
    m_coin = other.m_coin;
    m_payCoin = other.m_payCoin;
    m_eventCoin = other.m_eventCoin;
    m_field14 = other.m_field14;
    m_inventoryCapacity = other.m_inventoryCapacity;
    // ORIG 0x869d7a3..0x869d7b5：rep movsl 0x18c dwords + 2 字节 = 0x632 字节，
    // 完整复制 m_invenItem[26]（26×0x3d = 0x632）。旧注释 0x18c 误引
    // GetInvenData（0x84fbfc8）站点，此处必须整数组复制。
    memcpy(m_invenItem, other.m_invenItem, 0x630);
    *reinterpret_cast<unsigned short*>(reinterpret_cast<char*>(m_invenItem) + 0x630) =
        *reinterpret_cast<const unsigned short*>(
            reinterpret_cast<const char*>(other.m_invenItem) + 0x630);
    m_pEquipSlot = other.m_pEquipSlot;
    m_pAvatarSlot = other.m_pAvatarSlot;
    m_avatarCoin = other.m_avatarCoin;
    for (int i = 0; i < 10; i++)
    {
        m_clearAvatar[i] = other.m_clearAvatar[i];
    }
    for (int i = 0; i < 12; i++)
    {
        m_avatarVisible[i] = other.m_avatarVisible[i];
    }
    m_pCreatureSlot = other.m_pCreatureSlot;
    m_creatureMgr = other.m_creatureMgr;
    m_avatarItemMgr = other.m_avatarItemMgr;
    return *this;
}

void CInventory::reset()
{
    m_pParent = 0;
    m_money = 0;
    m_coin = 1;
    m_payCoin = 0;
    m_eventCoin = 0;
    m_field14 = 0;
    memset(m_invenItem, 0, 0x632);
    if (m_pEquipSlot != 0)
    {
        memset(m_pEquipSlot, 0, 0x4a58);
    }
    if (m_pAvatarSlot != 0)
    {
        memset(m_pAvatarSlot, 0, 0x1905);
    }
    if (m_pCreatureSlot != 0)
    {
        memset(m_pCreatureSlot, 0, 0x39aa);
    }
    m_creatureMgr.Reset();
    m_creatureMgr.SetUser(m_pParent);
    m_avatarItemMgr.Reset();
    memset(m_clearAvatar, 0, 0x28);
    m_inventoryCapacity = 0;
    m_avatarCoin.Set(0);
    for (int i = 0; i < 12; i++)
    {
        m_avatarVisible[i].init();
    }
}

void CInventory::setCopy(const CInventory& other)
{
    m_pParent = other.m_pParent;
    m_money = other.m_money;
    m_coin = other.m_coin;
    m_payCoin = other.m_payCoin;
    m_eventCoin = other.m_eventCoin;
    m_field14 = other.m_field14;
    for (int i = 0; i < 26; i++)
    {
        m_invenItem[i].setCopy(other.m_invenItem[i]);
    }
    for (int i = 0; i < 0x138; i++)
    {
        m_pEquipSlot[i].setCopy(other.m_pEquipSlot[i]);
    }
    for (int i = 0; i < 0x69; i++)
    {
        m_pAvatarSlot[i].setCopy(other.m_pAvatarSlot[i]);
    }
    m_creatureMgr.SetUser(m_pParent);
    for (int i = 0; i < 0xf2; i++)
    {
        m_pCreatureSlot[i].setCopy(other.m_pCreatureSlot[i]);
    }
    m_inventoryCapacity = other.get_inventory_capacity();
}

// ===================== 槽 / 数据访问 =====================

bool CInventory::CheckValidSlot(int invenType, int slot) const
{
    if (slot < 0)
    {
        return false;
    }
    switch (invenType)
    {
    case 0:
        if (slot > 0x19)
        {
            return false;
        }
        break;
    case 1:
        if (slot > 0x137)
        {
            return false;
        }
        break;
    case 2:
        if (slot > 0x68)
        {
            return false;
        }
        break;
    case 3:
        if (slot > 0xf1)
        {
            return false;
        }
        break;
    case 5:
        if (slot < 10 || slot > 0x16)
        {
            return false;
        }
        break;
    }
    return true;
}

Inven_Item* CInventory::GetInvenRef(int invenType, int slot)
{
    if (CheckValidSlot(invenType, slot) != 1)
    {
        return 0;
    }
    switch (invenType)
    {
    case 0:
        return &m_invenItem[slot];
    case 1:
        return &m_pEquipSlot[slot];
    case 2:
        return &m_pAvatarSlot[slot];
    case 3:
        return &m_pCreatureSlot[slot];
    case 5:
    {
        CExpandEquipslot* pExpand =
            m_pParent->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)9);
        return pExpand->GetInvenRefW(INVEN_TYPE_5, slot);
    }
    default:
        return 0;
    }
}

const Inven_Item* CInventory::GetInvenRef(int invenType, int slot) const
{
    if (CheckValidSlot(invenType, slot) != 1)
    {
        return 0;
    }
    switch (invenType)
    {
    case 0:
        return &m_invenItem[slot];
    case 1:
        return &m_pEquipSlot[slot];
    case 2:
        return &m_pAvatarSlot[slot];
    case 3:
        return &m_pCreatureSlot[slot];
    case 5:
    {
        CExpandEquipslot* pExpand =
            m_pParent->GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)9);
        return pExpand->GetInvenRefR(INVEN_TYPE_5, slot);
    }
    default:
        return 0;
    }
}

Inven_Item* CInventory::GetInvenStart(int invenType) const
{
    switch (invenType)
    {
    case 0:
        return const_cast<Inven_Item*>(&m_invenItem[10]);
    case 1:
        return m_pEquipSlot;
    case 2:
        return m_pAvatarSlot;
    case 3:
        return m_pCreatureSlot;
    case 5:
    {
        CExpandEquipslot* expand =
            m_pParent->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)9);
        return expand->getExpandEquipslotW((ENUM_EQUIPSLOT)1);
    }
    }
    return 0;
}

Inven_Item CInventory::GetInvenSlot(int invenType, int slot) const
{
    Inven_Item item;
    if (CheckValidSlot(invenType, slot) != 1)
    {
        return item;
    }
    if (m_pEquipSlot == 0 || m_pAvatarSlot == 0 || m_pCreatureSlot == 0)
    {
        return item;
    }
    switch (invenType)
    {
    case 0:
        item = m_invenItem[slot];
        break;
    case 1:
        item = m_pEquipSlot[slot];
        break;
    case 2:
        item = m_pAvatarSlot[slot];
        break;
    case 3:
        item = m_pCreatureSlot[slot];
        break;
    case 5:
    {
        CExpandEquipslot* pExpand =
            m_pParent->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)9);
        if (pExpand != 0)
        {
            item = pExpand->GetInvenSlot(INVEN_TYPE_5, slot);
        }
        break;
    }
    default:
        break;
    }
    return item;
}

int CInventory::GetInvenSlotByRef(int invenType, int slot, Inven_Item& item)
{
    if (CheckValidSlot(invenType, slot) != 1)
    {
        return 0;
    }
    switch (invenType)
    {
    case 0:
        item = m_invenItem[slot];
        break;
    case 1:
        item = m_pEquipSlot[slot];
        break;
    case 2:
        item = m_pAvatarSlot[slot];
        break;
    case 3:
        item = m_pCreatureSlot[slot];
        break;
    case 5:
    {
        CExpandEquipslot* pExpand =
            m_pParent->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)9);
        item = pExpand->GetInvenSlot(INVEN_TYPE_5, slot);
        break;
    }
    default:
        break;
    }
    return 1;
}


void CInventory::GetInvenData(int invenType, void* out, int size) const
{
    switch (invenType)
    {
    case 0:
        if (size == 0x632)
        {
            memcpy(out, m_invenItem, 0x632);
        }
        break;
    case 1:
        if (size == 0x4a58)
        {
            memcpy(out, m_pEquipSlot, 0x4a58);
        }
        break;
    case 2:
        if (size == 0x1905)
        {
            memcpy(out, m_pAvatarSlot, 0x1905);
        }
        break;
    case 3:
        if (size == 0x39aa)
        {
            memcpy(out, m_pCreatureSlot, 0x39aa);
        }
        break;
    case 5:
        if (size == 0x2dc)
        {
            memcpy(out, &m_invenItem[10], 0x2dc);
        }
        break;
    }
}

void CInventory::SetInvenData(int invenType, void* in, int size)
{
    switch (invenType)
    {
    case 0:
        if (size == 0x632)
        {
            memcpy(m_invenItem, in, size);
        }
        break;
    case 1:
        if (size == 0x4a58)
        {
            memcpy(m_pEquipSlot, in, size);
        }
        break;
    case 2:
        if (size == 0x1905)
        {
            memcpy(m_pAvatarSlot, in, size);
        }
        break;
    case 3:
        if (size == 0x39aa)
        {
            memcpy(m_pCreatureSlot, in, size);
        }
        break;
    case 5:
        if (size == 0x2dc)
        {
            memcpy(&m_invenItem[10], in, size);
        }
        break;
    }
}

void CInventory::ResetSlot(int invenType, int slot)
{
    // ORIG 0x84fb32a：switch(invenType) 形态（一次载入寄存器后 cmp/je 二叉判定树）。
    if (CheckValidSlot(invenType, slot) != 1)
    {
        return;
    }
    switch (invenType)
    {
    case 0:
        m_invenItem[slot].reset();
        break;
    case 1:
        m_pEquipSlot[slot].reset();
        break;
    case 2:
        m_pAvatarSlot[slot].reset();
        break;
    case 3:
        m_pCreatureSlot[slot].reset();
        break;
    }
}

void CInventory::StartEndPos(Inven_Item::ITEM_TYPE type, int& start, int& end) const
{
    if (!g_slotBoundChecker.get_slot_bound(type, m_inventoryCapacity, start, end))
    {
        start = 0;
        end = 0;
    }
}

void CInventory::getStartEndPos(Inven_Item::ITEM_TYPE type, int& start, int& end) const
{
    StartEndPos(type, start, end);
}

int CInventory::get_empty_slot(INVEN_TYPE invenType, Inven_Item::ITEM_TYPE type) const
{
    if (invenType == 2)
    {
        for (int i = 0; i < 0x69; i++)
        {
            if (m_pAvatarSlot[i].m_addInfo == 0)
            {
                return i;
            }
        }
    }
    else if (invenType == 3)
    {
        for (int i = 0; i < 0xf2; i++)
        {
            if (m_pCreatureSlot[i].m_addInfo == 0)
            {
                return i;
            }
        }
    }
    else if (invenType == 1)
    {
        int start = 0;
        int end = 0;
        StartEndPos(type, start, end);
        for (int i = start; i <= end; i++)
        {
            if (m_pEquipSlot[i].m_addInfo == 0)
            {
                return i;
            }
        }
    }
    return -1;
}

int CInventory::get_inven_slot_no(int itemIdx) const
{
    for (int i = 0; i <= 0x137; i++)
    {
        if (m_pEquipSlot[i].m_addInfo == itemIdx)
        {
            return i;
        }
    }
    return -1;
}

int CInventory::get_avatar_slot_no(int avatarUid) const
{
    for (int i = 0; i <= 0x68; i++)
    {
        if (m_pAvatarSlot[i].m_addInfo == avatarUid)
        {
            return i;
        }
    }
    return -1;
}

int CInventory::get_item_count(int itemIdx) const
{
    int count = 0;
    if (itemIdx == 1)
    {
        for (int i = 3; i < 0x138; i++)
        {
            if (m_pEquipSlot[i].m_addInfo != 0)
            {
                count++;
            }
        }
    }
    else if (itemIdx < 2)
    {
        if (itemIdx == 0)
        {
            for (int i = 0; i < 0x16; i++)
            {
                if (m_invenItem[i].m_addInfo != 0)
                {
                    count++;
                }
            }
        }
    }
    else if (itemIdx == 2)
    {
        for (int i = 0; i < 0x69; i++)
        {
            if (m_pAvatarSlot[i].m_addInfo != 0)
            {
                count++;
            }
        }
    }
    else if (itemIdx == 3)
    {
        for (int i = 0; i < 0xf2; i++)
        {
            if (m_pCreatureSlot[i].m_addInfo != 0)
            {
                count++;
            }
        }
    }
    return count;
}

int CInventory::get_item_count(int itemIdx, unsigned long param) const
{
    int count = 0;
    if (itemIdx == 1)
    {
        for (int i = 3; i < 0x138; i++)
        {
            if ((unsigned long)m_pEquipSlot[i].m_addInfo == param)
            {
                count += m_pEquipSlot[i].m_addInfo2;
            }
        }
    }
    else if (itemIdx < 2)
    {
        if (itemIdx == 0)
        {
            for (int i = 0; i < 0x16; i++)
            {
                if ((unsigned long)m_invenItem[i].m_addInfo == param)
                {
                    count++;
                }
            }
        }
    }
    else if (itemIdx == 2)
    {
        for (int i = 0; i < 0x69; i++)
        {
            if ((unsigned long)m_pAvatarSlot[i].m_addInfo == param)
            {
                count++;
            }
        }
    }
    else if (itemIdx == 3)
    {
        for (int i = 0; i < 0xf2; i++)
        {
            if ((unsigned long)m_pCreatureSlot[i].m_addInfo == param)
            {
                count++;
            }
        }
    }
    return count;
}

int CInventory::InvenEmptycount(Inven_Item::ITEM_TYPE type) const
{
    int count = 0;
    int start;
    int end;
    StartEndPos(type, start, end);
    if (IsCreatureItemType(type))
    {
        for (int i = start; i <= end; i++)
        {
            if (m_pCreatureSlot[i].m_addInfo == 0)
            {
                count++;
            }
        }
        return count;
    }
    else
    {
        if (type == 8)
        {
            for (int i = 0; i < 0x69; i++)
            {
                if (m_pAvatarSlot[i].m_addInfo == 0)
                {
                    count++;
                }
            }
        }
        else
        {
            for (int i = start; i <= end; i++)
            {
                if (m_pEquipSlot[i].m_addInfo == 0)
                {
                    count++;
                }
            }
        }
        return count;
    }
}

int CInventory::QuickEmptyCount() const
{
    int count = 0;
    for (int i = 3; i < 9; i++)
    {
        if (m_pEquipSlot[i].m_addInfo == 0)
        {
            count++;
        }
    }
    return count;
}

int CInventory::GetRemainCapacity(INVEN_TYPE invenType, Inven_Item::ITEM_TYPE type) const
{
    int count = 0;
    switch (invenType)
    {
    case 0:
    case 6:
        LogManager::logFormat(
            1, "inventory.cpp",
            __PRETTY_FUNCTION__,
            0x4c3, "Invalid inventory type");
        break;
    case 1:
    {
        int start = 0;
        int end = 0;
        StartEndPos(type, start, end);
        switch (type)
        {
        case 0:
        case 1:
        case 3:
        case 4:
            break;
        case 2:
            for (int i = start; i <= end; i++)
            {
                if (m_pEquipSlot[i].m_addInfo == 0)
                {
                    count++;
                }
            }
            break;
        default:
            LogManager::logFormat(
                1, "inventory.cpp",
                __PRETTY_FUNCTION__,
                0x493, "Invalid Item Type");
            break;
        case 10:
            break;
        }
        break;
    }
    case 2:
        for (int i = 0; i < 0x69; i++)
        {
            if (m_pAvatarSlot[i].m_addInfo == 0)
            {
                count++;
            }
        }
        break;
    case 3:
    {
        int start = 0;
        int end = 0;
        StartEndPos(type, start, end);
        for (int i = start; i <= end; i++)
        {
            if (m_pCreatureSlot[i].m_addInfo == 0)
            {
                count++;
            }
        }
        break;
    }
    }
    return count;
}

int CInventory::check_slot_empty(int slot) const
{
    if (slot > 0x137 || slot <= 0)
        return 0;
    if (m_pEquipSlot[slot].m_addInfo == 0)
    {
        return 1;
    }
    return 0;
}

int CInventory::check_empty_count(Inven_Item::ITEM_TYPE type, int count) const
{
    int empty = 0;
    int start;
    int end;
    StartEndPos(type, start, end);
    if (IsCreatureItemType(type))
    {
        for (int i = start; i <= end; i++)
        {
            if (m_pCreatureSlot[i].m_addInfo == 0)
            {
                empty++;
            }
            if (empty >= count)
            {
                return 1;
            }
        }
        return 0;
    }
    else
    {
        for (int i = start; i <= end; i++)
        {
            if (m_pEquipSlot[i].m_addInfo == 0)
            {
                empty++;
            }
            if (empty >= count)
            {
                return 1;
            }
        }
        return 0;
    }
}

int CInventory::check_item_exist(int itemIdx) const
{
    for (int i = 3; i < 9; i++)
    {
        if (m_pEquipSlot[i].m_addInfo == itemIdx)
        {
            return i;
        }
    }
    int type = GetItemType(itemIdx);
    if (type == 0)
    {
        LogManager::logFormat(1, "inventory.cpp",
                              __PRETTY_FUNCTION__, 0x1aa4,
                              "Unknown Type %d", 0);
    }
    else
    {
        int start = 0;
        int end = 0;
        StartEndPos((Inven_Item::ITEM_TYPE)type, start, end);
        if (!IsCreatureItemType(type))
        {
            for (int i = start; i <= end; i++)
            {
                if (m_pEquipSlot[i].m_addInfo == itemIdx)
                {
                    return i;
                }
            }
        }
        else
        {
            for (int i = start; i <= end; i++)
            {
                if (m_pCreatureSlot[i].m_addInfo == itemIdx)
                {
                    return i;
                }
            }
        }
    }
    return -1;
}

// ===================== 物品类型 / 哈希 =====================

int CInventory::GetItemType(int itemIdx) const
{
    CItem* item = G_CDataManager()->find_item(itemIdx);
    if (item == 0)
    {
        if (itemIdx != 0)
        {
            LogManager::logFormat(
                1, "inventory.cpp",
                __PRETTY_FUNCTION__, 0x13c2,
                "No Item %d", itemIdx);
        }
        return 0;
    }
    if (item->IsAvatarItem())
    {
        return 8;
    }
    if (!item->is_stackable())
    {
        if (item->GetItemType() == 0x16)
        {
            return 5;
        }
        if (item->GetItemType() == 0x17)
        {
            return 6;
        }
        return 1;
    }
    if (item->GetItemType() == 9)
    {
        return 7;
    }
    if (item->GetItemType() == 1)
    {
        return 3;
    }
    if (item->GetItemType() == 2)
    {
        return 10;
    }
    if (item->GetItemType() == 8)
    {
        return 4;
    }
    if (item->GetItemType() == 0x14)
    {
        return 9;
    }
    return 2;
}

int CInventory::GetSlotItemType(int slot) const
{
    Inven_Item::ITEM_TYPE type;
    if (g_slotBoundChecker.get_item_type(slot, m_inventoryCapacity, type))
    {
        return type;
    }
    return Inven_Item::ITEM_TYPE_INVALID;
}

void CInventory::getHashCode(unsigned char* out)
{
    md5_context ctx;
    md5_starts(&ctx);
    md5_update(&ctx, (unsigned char*)&m_invenItem[0], 0x53e);
    md5_update(&ctx, (unsigned char*)m_pEquipSlot, 0x4a58);
    md5_update(&ctx, (unsigned char*)m_pAvatarSlot, 0x1905);
    md5_update(&ctx, (unsigned char*)m_pCreatureSlot, 0x39aa);
    md5_finish(&ctx, out);
}

int CInventory::getPackedCodeArray1(unsigned long* out, int maxCount)
{
    int count = 0;
    for (int i = 0; i < 0x1a; i++)
    {
        if (m_invenItem[i].m_addInfo != 0)
        {
            out[count] = m_invenItem[i].getPackedCode1();
            count++;
            if (maxCount <= count)
            {
                return count;
            }
        }
    }
    for (int i = 0; i < 0x138; i++)
    {
        if (m_pEquipSlot[i].m_addInfo != 0)
        {
            out[count] = m_pEquipSlot[i].getPackedCode1();
            count++;
            if (maxCount <= count)
            {
                return count;
            }
        }
    }
    for (int i = 0; i < 0x69; i++)
    {
        if (m_pAvatarSlot[i].m_addInfo != 0)
        {
            out[count] = m_pAvatarSlot[i].getPackedCode1();
            count++;
            if (maxCount <= count)
            {
                return count;
            }
        }
    }
    for (int i = 0; ; i++)
    {
        if (i > 0xf1)
        {
            return count;
        }
        if (m_pCreatureSlot[i].m_addInfo != 0)
        {
            out[count] = m_pCreatureSlot[i].getPackedCode1();
            count++;
            if (maxCount <= count)
            {
                return count;
            }
        }
    }
}

size_t CInventory::getSortedKeyArray(unsigned long* out, int maxCount)
{
    int i = 0;
    for (i = 0; i < maxCount; i++)
    {
        out[i] = 0;
    }
    int count = 0;
    for (i = 0; i < 0x1a; i++)
    {
        Inven_Item* pInvenItem = &m_invenItem[i];
        if (pInvenItem->m_addInfo != 0)
        {
            out[count] = pInvenItem->getKey();
            count++;
            if (maxCount <= (int)count)
            {
                qsort(out, count, 4, my_compare_unsigned_short);
                return count;
            }
        }
    }
    for (i = 0; i < 0x138; i++)
    {
        Inven_Item* pEquipItem = &m_pEquipSlot[i];
        if (pEquipItem->m_addInfo != 0)
        {
            out[count] = pEquipItem->getKey();
            count++;
            if (maxCount <= (int)count)
            {
                qsort(out, count, 4, my_compare_unsigned_short);
                return count;
            }
        }
    }
    for (i = 0; i < 0x69; i++)
    {
        Inven_Item* pAvatarItem = &m_pAvatarSlot[i];
        if (pAvatarItem->m_addInfo != 0)
        {
            out[count] = pAvatarItem->getKey();
            count++;
            if (maxCount <= (int)count)
            {
                qsort(out, count, 4, my_compare_unsigned_short);
                return count;
            }
        }
    }
    for (i = 0; i <= 0xf1; i++)
    {
        Inven_Item* pCreatureItem = &m_pCreatureSlot[i];
        if (pCreatureItem->m_addInfo != 0)
        {
            out[count] = pCreatureItem->getKey();
            count++;
            if (maxCount <= (int)count)
            {
                qsort(out, count, 4, my_compare_unsigned_short);
                return count;
            }
        }
    }
    qsort(out, count, 4, my_compare_unsigned_short);
    return count;
}

// ORIG 0x0850ae7a 返回类型为 int（尾调用 finishItemChecksum 的 eax 流出；
// mangled 不编码返回类型，符号不变）。CUser::getCurCharacInvenCheckSum 直接
// 返回该值（docs/class_func_reports/CUser/getCurCharacInvenCheckSum.md）。
int CInventory::makeCheckSum(int a, int b) const
{
    int sum = 0;
    for (int i = 10; i < 0x16; i++)
    {
        if (m_invenItem[i].m_addInfo != 0)
        {
            unsigned short v = (unsigned short)m_invenItem[i].m_fieldb;
            unsigned char upgrade = m_invenItem[i].GetUpgrade();
            sum += getItemChecksum(i, m_invenItem[i].m_addInfo, 1, upgrade, v);
        }
    }
    for (int i = 3; i < 0x138; i++)
    {
        if (m_pEquipSlot[i].m_addInfo != 0)
        {
            unsigned short v = (unsigned short)m_pEquipSlot[i].m_fieldb;
            unsigned char upgrade = m_pEquipSlot[i].GetUpgrade();
            if (m_pEquipSlot[i].m_field1 == 1)
            {
                sum += getItemChecksum(i, m_pEquipSlot[i].m_addInfo, 1, upgrade, v);
            }
            else
            {
                sum += getItemChecksum(i, m_pEquipSlot[i].m_addInfo,
                                       m_pEquipSlot[i].m_addInfo2, upgrade, v);
            }
        }
    }
    return finishItemChecksum(&sum, m_money, m_coin + m_payCoin + m_eventCoin, a, b);
}

// ===================== 货币（第九批 I2 补全） =====================

void CInventory::UpdateDailyCoin(int value)
{
    if (m_coin < value)
    {
        m_coin = value;
        ((CUserCharacInfo*)m_pParent)->enableSaveInven();
    }
}

int CInventory::UseCoin(eCoinSubReason reason)
{
    // ORIG 0x84faa98：!=0 递降取向（coin→event→pay），history 调用内联、
    // trace 块跳转目标；三 cMyTrace 槽位按书写序 -0x3c/-0x2c/-0x1c。
    if (m_coin != 0)
    {
        m_coin--;
        if (m_pParent != 0)
        {
            m_pParent->m_historyLog.CoinSub(m_coin, 1, reason);
        }
        else
        {
            cMyTrace local_3c(__PRETTY_FUNCTION__, 0x28e, 5);
            local_3c("History ERROR, m_pParent NULL, CoinSub");
        }
    }
    else
    {
        if (m_eventCoin != 0)
        {
            m_eventCoin--;
            if (m_pParent != 0)
            {
                m_pParent->m_historyLog.EventCoinSub(m_eventCoin, 1, reason);
            }
            else
            {
                cMyTrace local_2c(__PRETTY_FUNCTION__, 0x29d, 5);
                local_2c("History ERROR, m_pParent NULL, EventCoinSub");
            }
        }
        else
        {
            if (m_payCoin != 0)
            {
                m_payCoin--;
                if (m_pParent != 0)
                {
                    m_pParent->m_historyLog.PayCoinSub(m_payCoin, 1, reason);
                }
                else
                {
                    cMyTrace local_1c(__PRETTY_FUNCTION__, 0x2ad, 5);
                    local_1c("History ERROR, m_pParent NULL, PayCoinSub");
                }
            }
            else
            {
                m_coin = 0;
                m_payCoin = 0;
                m_eventCoin = 0;
            }
        }
    }
    return m_coin;
}

int CInventory::UseUpCoin(unsigned int amount)
{
    if (amount < m_coin)
    {
        m_coin -= amount;
        amount = 0;
    }
    else
    {
        amount -= m_coin;
        m_coin = 0;
    }
    if (amount < (unsigned int)m_eventCoin)
    {
        m_eventCoin -= amount;
        amount = 0;
    }
    else
    {
        amount -= m_eventCoin;
        m_eventCoin = 0;
    }
    if (amount < (unsigned int)m_payCoin)
    {
        m_payCoin -= amount;
    }
    else
    {
        m_payCoin = 0;
    }
    return m_coin + m_eventCoin + m_payCoin;
}

int CInventory::gain_money(int amount, eMoneyAddReason reason, bool bLog, int param)
{
    int money;
    register int level;
    register const char* accName;
    int limit = 0x7fffffff;
    int oldMoney;
    unsigned int gained;
    unsigned int i;

    if (m_pParent != 0)
    {
        accName = m_pParent->get_acc_name();
        level = ((CUserCharacInfo*)m_pParent)->get_charac_level();
        limit = G_CDataManager()->GetMoneyLimitPerLevel(level, accName);
    }
    oldMoney = m_money;
    if (limit - amount < m_money)
    {
        m_money = limit;
        int money = m_money;
        register int level2 = ((CUserCharacInfo*)m_pParent)->get_charac_level();
        register int characNo = m_pParent->get_charac_no(-1);
        register char* numStr = NumberToString(m_pParent->get_acc_id(), 0);
        cMyTrace local_3c(__PRETTY_FUNCTION__, 0xef1, 4);
        local_3c("over money limit:m_id(%s),charac_no(%d),level(%d) gain_money inven(%d)+add(%d)",
                 numStr, characNo, level2, money, amount);
    }
    else
    {
        m_money += amount;
    }
    if (m_pParent != 0 && isGainedGoldFromDungeonReason(reason))
    {
        gained = m_pParent->getDungeonGainedGold()->incGainedGold(amount);
        for (i = 0; i < gained; i++)
        {
            m_pParent->getHackAnalyzer()->addServerHackCnt(
                m_pParent, (WongWork::ENUM_HACK_TYPE)0x2bf, 1u, 0u, 0u);
        }
    }
    if (bLog && m_pParent != 0)
    {
        m_pParent->m_historyLog.MoneyAdd(m_money, amount, reason);
    }
    if (m_pParent != 0)
    {
        if (reason == 4 || reason == 0x1a)
        {
            write_log_gain_money(reason, m_money - oldMoney - param, m_pParent);
        }
        else
        {
            write_log_gain_money(reason, m_money - oldMoney, m_pParent);
        }
        if (m_money - oldMoney > 0)
        {
            m_pParent->GetGoldControl()->AddGold(m_money - oldMoney, reason);
        }
    }
    return m_money - oldMoney;
}

bool CInventory::use_money(int amount, eMoneySubReason reason, bool bLog)
{
    int oldMoney;
    int limit;

    if (m_money < amount || amount < 0)
    {
        return 0;
    }
    oldMoney = m_money;
    m_money -= amount;
    limit = 0x7fffffff;
    if (m_pParent != 0)
    {
        limit = G_CDataManager()->GetMoneyLimitPerLevel(
            ((CUserCharacInfo*)m_pParent)->get_charac_level(), m_pParent->get_acc_name());
    }
    if (limit < m_money)
    {
        m_money = limit;
        cMyTrace local_34(__PRETTY_FUNCTION__, 0xf30, 4);
        local_34("over money limit:m_id(%s),charac_no(%d),level(%d) use_money inven(%d)-sub(%d)",
                 NumberToString(m_pParent->get_acc_id(), 0), m_pParent->get_charac_no(-1),
                 ((CUserCharacInfo*)m_pParent)->get_charac_level(), m_money, amount);
    }
    if (m_money < 0)
    {
        m_money = 0;
    }
    if (m_pParent != 0)
    {
        write_log_use_money(reason, oldMoney - m_money, m_pParent);
        if (oldMoney != m_money && oldMoney - m_money > 0)
        {
            m_pParent->GetGoldControl()->SubGold(oldMoney - m_money, reason);
        }
    }
    if (bLog && m_pParent != 0 && amount > 0)
    {
        m_pParent->m_historyLog.MoneySub(m_money, amount, reason);
    }
    return 1;
}

int CInventory::getValidMoney(int add)
{
    int limit = 0x7fffffff;
    register const char* name = m_pParent->get_acc_name();
    register int level = ((CUserCharacInfo*)m_pParent)->get_charac_level() - 1;
    limit = G_CDataManager()->GetMoneyLimitPerLevel(level, name);
    if (limit < m_money + add)
    {
        return 1;
    }
    return 0;
}

// ===================== 槽 / 数据访问（第九批 I2 补全） =====================

int CInventory::GetInvenData(int itemIdx, Inven_Item& out) const
{
    int type = GetItemType(itemIdx);
    int start = 0;
    int end = 0;
    if (type == 0)
    {
        return -1;
    }
    StartEndPos((Inven_Item::ITEM_TYPE)type, start, end);
    if (IsCreatureItemType(type))
    {
        for (int i = start; i <= end; i++)
        {
            if (m_pCreatureSlot[i].m_addInfo == itemIdx)
            {
                out = m_pCreatureSlot[i];
                return i;
            }
        }
    }
    else
    {
        for (int i = start; i <= end; i++)
        {
            if (m_pEquipSlot[i].m_addInfo == itemIdx)
            {
                out = m_pEquipSlot[i];
                return i;
            }
        }
        StartEndPos(Inven_Item::ITEM_TYPE_0, start, end);
        for (int i = start; i <= end; i++)
        {
            if (m_pEquipSlot[i].m_addInfo == itemIdx)
            {
                out = m_pEquipSlot[i];
                return i;
            }
        }
    }
    return -1;
}

int CInventory::GetEmptyCreatureSlot(Inven_Item& item) const
{
    int start = 0;
    int end = 0;
    StartEndPos((Inven_Item::ITEM_TYPE)item.m_field1, start, end);
    for (int i = start; i <= end; i++)
    {
        if (m_pCreatureSlot[i].m_addInfo == 0)
        {
            return i;
        }
    }
    return -1;
}

int CInventory::GetExistCreatureSlot(Inven_Item& item) const
{
    int start = 0;
    int end = 0;
    StartEndPos((Inven_Item::ITEM_TYPE)item.m_field1, start, end);
    for (int i = start; i <= end; i++)
    {
        if (m_pCreatureSlot[i].m_addInfo == item.m_addInfo)
        {
            return i;
        }
    }
    return -1;
}

int CInventory::getAvatarEmptySlot(int itemIdx, int param) const
{
    for (int i = itemIdx; i <= param; i++)
    {
        if (m_pAvatarSlot[i].m_addInfo == 0)
        {
            return i;
        }
    }
    return -1;
}

unsigned char CInventory::getEquipWeaponUpgradeSeparate() const
{
    // ORIG 以绝对偏移访问（+0x280 标志、+0x2b1 UpgradeSeparateInfo），逐指令还原。
    if (m_invenItem[10].m_addInfo == 0)
    {
        return 0;
    }
    return m_invenItem[10].m_upgradeSep.GetUpgradeSeparate();
}

int CInventory::getExistEquipItem(int itemIdx, int count, int slot)
{
    int nField = 0;  // ORIG 顶部有未使用局部（-0x8(%ebp) 初始化为 0）
    for (int i = 3; i <= 0xc8; i++)
    {
        if (i != slot && m_pEquipSlot[i].m_field1 == 1 &&
            m_pEquipSlot[i].m_addInfo == itemIdx && m_pEquipSlot[i].m_addInfo2 == count)
        {
            return i;
        }
    }
    return -1;
}

int CInventory::doesExistEquipItem(int itemIdx, int slot)
{
    for (int i = 3; i <= 0xc8; i++)
    {
        if (m_pEquipSlot[i].m_addInfo == itemIdx &&
            m_pEquipSlot[i].m_addInfo2 == slot && m_pEquipSlot[i].m_field1 == 1)
        {
            return 1;
        }
    }
    return 0;
}

int CInventory::MoveItemToEmptySlotOfInventory(int slot, int start, int end)
{
    Inven_Item item;
    int dst = -1;
    int i = start;
    do
    {
        if (end < i)
        {
            break;
        }
        if (m_pEquipSlot[i].m_addInfo == 0)
        {
            dst = i;
            break;
        }
        i++;
    } while (1);
    if (dst == -1)
    {
        return 0x16;
    }
    item.setCopy(m_pEquipSlot[slot]);
    m_pEquipSlot[slot].reset();
    m_pEquipSlot[dst].setCopy(item);
    return 0;
}

bool CInventory::IsEquipAvatar() const
{
    for (int i = 0; i <= 9; i++)
    {
        if (m_invenItem[i].m_addInfo != 0)
        {
            return true;
        }
    }
    return false;
}

bool CInventory::IsExistStackable(int itemIdx, Inven_Item::ITEM_TYPE type) const
{
    int start = 0;
    int end = 0;
    StartEndPos(type, start, end);
    for (int i = start; i <= end; i++)
    {
        if (m_pEquipSlot[i].m_addInfo == itemIdx)
        {
            return true;
        }
    }
    return false;
}

bool CInventory::IsOverStackableItem(int itemIdx, unsigned char count)
{
    return true;
}

bool CInventory::isAnyExpiredAvatarItem() const
{
    int now = OS_API::GetDateTimeTick();
    for (int i = 0; i <= 9; i++)
    {
        if (m_invenItem[i].m_addInfo != 0 && m_invenItem[i].m_addInfo2 != 0 &&
            m_invenItem[i].m_addInfo2 < now)
        {
            return true;
        }
    }
    for (int i = 0; i <= 0x68; i++)
    {
        if (m_pAvatarSlot[i].m_addInfo != 0 && m_pAvatarSlot[i].m_addInfo2 != 0 &&
            m_pAvatarSlot[i].m_addInfo2 < now)
        {
            return true;
        }
    }
    return false;
}

bool CInventory::isEquipmentPlaceableAtInventory(int slot)
{
    if (slot > 2 && slot <= 8)
        return true;
    if (slot > 8 && slot <= 0x38)
        return true;
    return false;
}

// ===================== 背包插入 / 堆叠（第九批 I2 重点） =====================

int CInventory::insertInvenItemIntoQuickSlot(Inven_Item item, eItemAddReason reason, bool bLog)
{
    for (int i = 3; i <= 8; i++)
    {
        if (m_pEquipSlot[i].m_addInfo == 0)
        {
            if (item.m_field1 != 1 && !checkStackableLimit((unsigned long)item.m_addInfo, item.m_addInfo2))
            {
                return -10;
            }
            m_pEquipSlot[i].setCopy(item);
            if (bLog)
            {
                if (m_pParent == 0)
                {
                    LogManager::logFormat(
                        1, "inventory.cpp",
                        __PRETTY_FUNCTION__,
                        0x1528, "History ERROR, m_pParent NULL, ItemAdd ");
                }
                else if (item.m_field1 == 1)
                {
                    m_pParent->m_historyLog.ItemAdd(
                        (INVEN_TYPE)1, 1, 1, item, reason);
                }
                else
                {
                    m_pParent->m_historyLog.ItemAdd(
                        (INVEN_TYPE)1, m_pEquipSlot[i].get_add_info(), item.get_add_info(),
                        item, reason);
                }
            }
            return i;
        }
    }
    return -2;
}

int CInventory::insertInvenItemIntoCoresspondingSpace(Inven_Item item, eItemAddReason reason,
                                                      bool bLog)
{
    int start = 0;
    int end = 0;
    getStartEndPos((Inven_Item::ITEM_TYPE)item.m_field1, start, end);
    for (int i = start; i <= end; i++)
    {
        if (m_pEquipSlot[i].m_addInfo == 0)
        {
            if (item.m_field1 != 1 && !checkStackableLimit((unsigned long)item.m_addInfo, item.m_addInfo2))
            {
                return -10;
            }
            m_pEquipSlot[i].setCopy(item);
            if (bLog)
            {
                if (m_pParent == 0)
                {
                    LogManager::logFormat(
                        1, "inventory.cpp",
                        __PRETTY_FUNCTION__,
                        0x155c, "History ERROR, m_pParent NULL, ItemAdd ");
                }
                else if (item.m_field1 == 1)
                {
                    m_pParent->m_historyLog.ItemAdd(
                        (INVEN_TYPE)1, 1, 1, item, reason);
                }
                else
                {
                    m_pParent->m_historyLog.ItemAdd(
                        (INVEN_TYPE)1, m_pEquipSlot[i].get_add_info(), item.get_add_info(),
                        item, reason);
                }
            }
            return i;
        }
    }
    return -2;
}

int CInventory::stackInvenItemIntoQuickSlot(Inven_Item item, eItemAddReason reason, bool bLog)
{
    if (item.m_field1 == 1)
    {
        return -1;
    }
    for (int i = 3; i <= 8; i++)
    {
        if (m_pEquipSlot[i].m_addInfo != item.m_addInfo)
        {
            continue;
        }
        if (0x7fffffff - item.get_add_info() <= m_pEquipSlot[i].get_add_info())
        {
            if (bLog && m_pParent != 0)
            {
                int itemAddInfo = item.m_addInfo;
                int itemInfo = item.get_add_info();
                int slotInfo = m_pEquipSlot[i].get_add_info();
                cMyTrace trace(__PRETTY_FUNCTION__,
                               0x157c, 4);
                trace("over stack limit:m_id(%s),charac_no(%d) Stack Item [%d]+[%d] item[%d]",
                      NumberToString(m_pParent->get_acc_id(), 0), m_pParent->get_charac_no(-1),
                      slotInfo, itemInfo, itemAddInfo);
            }
            m_pEquipSlot[i].set_add_info(0);
        }
        else
        {
            if (!checkStackableLimit((unsigned long)m_pEquipSlot[i].m_addInfo,
                                     m_pEquipSlot[i].get_add_info() + item.m_addInfo2))
            {
                return -10;
            }
            m_pEquipSlot[i].set_add_info(m_pEquipSlot[i].get_add_info() + item.m_addInfo2);
            if (bLog)
            {
                if (m_pParent == 0)
                {
                    LogManager::logFormat(
                        1, "inventory.cpp",
                        __PRETTY_FUNCTION__,
                        0x1597, "History ERROR, m_pParent NULL, ItemAdd ");
                }
                else
                {
                    m_pParent->m_historyLog.ItemAdd(
                        (INVEN_TYPE)1, m_pEquipSlot[i].get_add_info(), item.get_add_info(),
                        item, reason);
                }
            }
        }
        return i;
    }
    return -2;
}

int CInventory::stackInvenItemIntoCorrespondingSpace(Inven_Item item, eItemAddReason reason,
                                                     bool bLog)
{
    if (item.m_field1 == 1)
    {
        return -1;
    }
    int start = 0;
    int end = 0;
    getStartEndPos((Inven_Item::ITEM_TYPE)item.m_field1, start, end);
    for (int i = start; i <= end; i++)
    {
        if (m_pEquipSlot[i].m_addInfo != item.m_addInfo)
        {
            continue;
        }
        if (0x7fffffff - item.get_add_info() <= m_pEquipSlot[i].get_add_info())
        {
            if (bLog && m_pParent != 0)
            {
                int itemAddInfo = item.m_addInfo;
                int itemInfo = item.get_add_info();
                int slotInfo = m_pEquipSlot[i].get_add_info();
                cMyTrace trace(__PRETTY_FUNCTION__,
                               0x15b7, 4);
                trace("over stack limit:m_id(%s),charac_no(%d) Stack Item [%d]+[%d] item[%d]",
                      NumberToString(m_pParent->get_acc_id(), 0), m_pParent->get_charac_no(-1),
                      slotInfo, itemInfo, itemAddInfo);
            }
            m_pEquipSlot[i].set_add_info(0);
        }
        else
        {
            if (!checkStackableLimit((unsigned long)m_pEquipSlot[i].m_addInfo,
                                     m_pEquipSlot[i].get_add_info() + item.m_addInfo2))
            {
                return -10;
            }
            m_pEquipSlot[i].set_add_info(m_pEquipSlot[i].get_add_info() + item.m_addInfo2);
            if (bLog)
            {
                if (m_pParent == 0)
                {
                    LogManager::logFormat(
                        1, "inventory.cpp",
                        __PRETTY_FUNCTION__,
                        0x15d2, "History ERROR, m_pParent NULL, ItemAdd ");
                }
                else
                {
                    m_pParent->m_historyLog.ItemAdd(
                        (INVEN_TYPE)1, m_pEquipSlot[i].get_add_info(), item.get_add_info(),
                        item, reason);
                }
            }
        }
        return i;
    }
    return -2;
}

int CInventory::tryInsertInvenItemIntoQuickSlot(Inven_Item item) const
{
    if (!item.isEquipableItemType() &&
        !checkStackableLimit((unsigned long)item.m_addInfo, item.m_addInfo2))
    {
        return -10;
    }
    for (int i = 3; i <= 8; i++)
    {
        if (m_pEquipSlot[i].m_addInfo == 0)
        {
            return i;
        }
    }
    return -2;
}

int CInventory::tryInsertInvenItemIntoCoresspondingSpace(Inven_Item item) const
{
    if (!item.isEquipableItemType() &&
        !checkStackableLimit((unsigned long)item.m_addInfo, item.m_addInfo2))
    {
        return -10;
    }
    int start = 0;
    int end = 0;
    getStartEndPos((Inven_Item::ITEM_TYPE)item.m_field1, start, end);
    for (int i = start; i <= end; i++)
    {
        if (m_pEquipSlot[i].m_addInfo == 0)
        {
            return i;
        }
    }
    return -2;
}

int CInventory::tryStackInvenItemIntoQuickSlot(Inven_Item item) const
{
    if (item.m_field1 == 1)
    {
        return -1;
    }
    for (int i = 3; i <= 8; i++)
    {
        if (m_pEquipSlot[i].m_addInfo != item.m_addInfo)
        {
            continue;
        }
        if (0x7fffffff - item.get_add_info() <= m_pEquipSlot[i].get_add_info())
        {
            return -2;
        }
        if (!checkStackableLimit((unsigned long)item.m_addInfo,
                                 m_pEquipSlot[i].get_add_info() + item.get_add_info()))
        {
            return -10;
        }
        return i;
    }
    return -4;
}

int CInventory::tryStackInvenItemIntoCorrespondingSpace(Inven_Item item) const
{
    if (item.m_field1 == 1)
    {
        return -1;
    }
    int start = 0;
    int end = 0;
    getStartEndPos((Inven_Item::ITEM_TYPE)item.m_field1, start, end);
    for (int i = start; i <= end; i++)
    {
        if (m_pEquipSlot[i].m_addInfo != item.m_addInfo)
        {
            continue;
        }
        if (0x7fffffff - item.get_add_info() <= m_pEquipSlot[i].get_add_info())
        {
            return -2;
        }
        if (!checkStackableLimit((unsigned long)item.m_addInfo,
                                 m_pEquipSlot[i].get_add_info() + item.get_add_info()))
        {
            return -10;
        }
        return i;
    }
    return -4;
}

int CInventory::insertItemIntoInventory(Inven_Item item, eItemAddReason reason, bool bLog, bool b)
{
    if (item.m_addInfo == 0)
    {
        if (item.get_add_info() < 0)
        {
            return -1;
        }
        if (item.get_add_info() == 0)
        {
            return 0;
        }
        gain_money(item.get_add_info(), (eMoneyAddReason)7, bLog, 0);
        return 0;
    }
    if (reason != 7 && reason != 2)
    {
        int dbLogType = G_CDataManager()->IsDBLogItem(item.m_addInfo);
        if (dbLogType != -1)
        {
            if (item.m_field1 == 1)
            {
                m_pParent->AddDBLogItem(item.m_addInfo, 1u,
                                        (ENUM_DBLOG_ITEM_TYPE)dbLogType);
            }
            else if (item.m_field1 == 2)
            {
                m_pParent->AddDBLogItem(item.m_addInfo, (unsigned int)item.get_add_info(),
                                        (ENUM_DBLOG_ITEM_TYPE)dbLogType);
            }
        }
    }
    if (item.IsCreatureItemType())
    {
        return insertItemIntoCreature(item, reason, bLog, b);
    }
    if (item.m_field1 == 8)
    {
        char ipgNo[24];
        WongWork::CGenUniqueNo::genIPGNo(
            WongWork::CGenUniqueNo::ENUM_IPGNO_TYPE_6,
            ((CUserCharacInfo*)m_pParent)->getCurCharacNo(), ipgNo);
        int slot = m_pParent->addAvatarItem(item.m_addInfo, (char)0, (char)item.m_fieldb,
                                            ipgNo, (eAvatarItemAddReason)0x14);
        m_pParent->SendUpdateItemList((CUser::eSendTarget)1, (ENUM_ITEMSPACE)1, slot);
        return slot;
    }
    if (item.m_field1 != 1)
    {
        int result = stackInvenItemIntoQuickSlot(item, reason, bLog);
        if (result >= 0)
        {
            return result;
        }
        if (result == -10)
        {
            return -1;
        }
        result = stackInvenItemIntoCorrespondingSpace(item, reason, bLog);
        if (result >= 0)
        {
            return result;
        }
        if (result == -10)
        {
            return -1;
        }
    }
    if (item.m_field1 == 2)
    {
        int result = insertInvenItemIntoQuickSlot(item, reason, bLog);
        if (result >= 0)
        {
            return result;
        }
        if (result == -10)
        {
            return -1;
        }
        result = insertInvenItemIntoCoresspondingSpace(item, reason, bLog);
        if (result >= 0)
        {
            return result;
        }
        if (result == -10)
        {
            return -1;
        }
    }
    else
    {
        int result = insertInvenItemIntoCoresspondingSpace(item, reason, bLog);
        if (result >= 0)
        {
            return result;
        }
        result = insertInvenItemIntoQuickSlot(item, reason, bLog);
        if (result >= 0)
        {
            return result;
        }
    }
    return -1;
}

int CInventory::tryInsertItemIntoInventory(Inven_Item item) const
{
    if (item.m_addInfo == 0)
    {
        if (item.m_addInfo2 < 0)
        {
            return -1;
        }
        return 0;
    }
    if (item.m_field1 == 8)
    {
        int result = tryInsertAvatarIntoInventory(item);
        if (result >= 0)
        {
            return result;
        }
        return -1;
    }
    if (item.m_field1 != 1)
    {
        int result = tryStackInvenItemIntoQuickSlot(item);
        if (result >= 0)
        {
            return result;
        }
        if (result == -10)
        {
            return -1;
        }
        result = tryStackInvenItemIntoCorrespondingSpace(item);
        if (result >= 0)
        {
            return result;
        }
        if (result == -10)
        {
            return -1;
        }
    }
    if (item.m_field1 == 2)
    {
        int result = tryInsertInvenItemIntoQuickSlot(item);
        if (result >= 0)
        {
            return result;
        }
        if (result == -10)
        {
            return -1;
        }
        result = tryInsertInvenItemIntoCoresspondingSpace(item);
        if (result >= 0)
        {
            return result;
        }
        if (result == -10)
        {
            return -1;
        }
    }
    else
    {
        int result = tryInsertInvenItemIntoCoresspondingSpace(item);
        if (result >= 0)
        {
            return result;
        }
        result = tryInsertInvenItemIntoQuickSlot(item);
        if (result >= 0)
        {
            return result;
        }
    }
    return -1;
}

int CInventory::insertItemIntoEquipment(Inven_Item item, eItemAddReason reason, bool bLog, bool b)
{
    CItem* pItem = G_CDataManager()->find_item(item.m_addInfo);
    if (pItem == 0)
    {
        return -1;
    }
    if (m_invenItem[pItem->GetItemType()].m_addInfo != 0)
    {
        return -2;
    }
    m_invenItem[pItem->GetItemType()].setCopy(item);
    if (bLog)
    {
        if (m_pParent == 0)
        {
            LogManager::logFormat(
                1, "inventory.cpp",
                __PRETTY_FUNCTION__,
                0x14f9, "History ERROR, m_pParent NULL, ItemAdd ");
        }
        else
        {
            m_pParent->m_historyLog.ItemAdd(
                (INVEN_TYPE)1, 1, 1, item, reason);
        }
    }
    return pItem->GetItemType();
}

int CInventory::tryInsertItemIntoEquipment(Inven_Item item) const
{
    CItem* pItem = G_CDataManager()->find_item(item.m_addInfo);
    if (pItem == 0)
    {
        return -1;
    }
    if (m_invenItem[pItem->GetItemType()].m_addInfo != 0)
    {
        return -2;
    }
    return pItem->GetItemType();
}

int CInventory::insertItemIntoCreatureEquipment(Inven_Item item, eItemAddReason reason, bool bLog)
{
    if (item.m_addInfo == 0)
    {
        return -1;
    }
    int slot = GetEmptyCreatureSlot(item);
    if (slot < 0)
    {
        return -1;
    }
    m_pCreatureSlot[slot].setCopy(item);
    return slot;
}

int CInventory::insertItemIntoArtifactEquipment(Inven_Item item, eItemAddReason reason, bool bLog)
{
    if (item.m_addInfo == 0)
    {
        return -1;
    }
    int slot = GetEmptyCreatureSlot(item);
    if (slot < 0)
    {
        return -1;
    }
    m_pCreatureSlot[slot].setCopy(item);
    if (bLog)
    {
        WriteCreatureLog((INVEN_TYPE)3, item.m_addInfo, 1, 1, item.m_field1, reason);
    }
    return slot;
}

int CInventory::insertItemIntoCreatureStackable(Inven_Item item, eItemAddReason reason, bool bLog)
{
    if (item.m_addInfo == 0)
    {
        return -1;
    }
    int slot = GetExistCreatureSlot(item);
    if (slot >= 0)
    {
        if (0x7fffffff - item.get_add_info() > m_pCreatureSlot[slot].get_add_info())
        {
            if (!checkStackableLimit((unsigned long)m_pCreatureSlot[slot].m_addInfo,
                                     m_pCreatureSlot[slot].get_add_info() + item.get_add_info()))
            {
                return -10;
            }
            m_pCreatureSlot[slot].set_add_info(
                m_pCreatureSlot[slot].get_add_info() + item.get_add_info());
            if (bLog)
            {
                WriteCreatureLog((INVEN_TYPE)1, item.m_addInfo, m_pCreatureSlot[slot].get_add_info(),
                                 item.get_add_info(), item.m_field1, reason);
            }
            return slot;
        }
    }
    slot = GetEmptyCreatureSlot(item);
    if (slot < 0)
    {
        return -1;
    }
    m_pCreatureSlot[slot].setCopy(item);
    if (bLog)
    {
        WriteCreatureLog((INVEN_TYPE)3, item.m_addInfo, m_pCreatureSlot[slot].get_add_info(),
                         item.get_add_info(), item.m_field1, reason);
    }
    return slot;
}

int CInventory::insertItemIntoCreature(Inven_Item& item, eItemAddReason reason, bool bLog, bool b)
{
    if (item.m_field1 == 7)
    {
        return insertItemIntoCreatureStackable(item, reason, bLog);
    }
    if (item.m_field1 == 5)
    {
        return insertItemIntoCreatureEquipment(item, reason, bLog);
    }
    if (item.m_field1 == 6)
    {
        return insertItemIntoArtifactEquipment(item, reason, bLog);
    }
    LogManager::logFormat(
        1, "inventory.cpp",
        __PRETTY_FUNCTION__,
        0x2549, "History ERROR, unknown inven_Item type, insertItemIntoCreature ");
    return -1;
}

int CInventory::tryInsertItemIntoCreature(Inven_Item& item) const
{
    int result;
    if (item.isEquipableItemType())
    {
        result = GetEmptyCreatureSlot(item);
    }
    else
    {
        result = GetExistCreatureSlot(item);
        if (result < 0)
        {
            result = GetEmptyCreatureSlot(item);
        }
    }
    return result;
}

// ===================== 日志 / 检查类（第九批 I2 补全） =====================

void CInventory::WriteCreatureLog(INVEN_TYPE invenType, int a, int b, int c, int d,
                                  eItemAddReason reason) const
{
    if (m_pParent == 0)
    {
        LogManager::logFormat(
            1, "inventory.cpp",
            __PRETTY_FUNCTION__,
            0x24ce, "History ERROR, m_pParent NULL, ItemAdd ");
    }
    else
    {
        m_pParent->m_historyLog.CreatureItemAdd(
            invenType, a, b, c, d, reason);
    }
}

int CInventory::CheckExistItemInInventoryOrEquipment(int itemIdx, int& slot, bool& bFound) const
{
    bFound = false;
    for (int i = 3; i <= 8; i++)
    {
        if (m_pEquipSlot[i].m_addInfo == itemIdx)
        {
            slot = i;
            return 1;
        }
    }
    int type = GetItemType(itemIdx);
    if (type == 0)
    {
        LogManager::logFormat(
            1, "inventory.cpp",
            __PRETTY_FUNCTION__,
            0x1ace, "Unknown Type %d", 0);
        return 0;
    }
    for (int i = 0; i <= 0x19; i++)
    {
        if (m_invenItem[i].m_addInfo == itemIdx)
        {
            bFound = true;
            slot = i;
            return 1;
        }
    }
    int start = 0;
    int end = 0;
    StartEndPos((Inven_Item::ITEM_TYPE)type, start, end);
    for (int i = start; i <= end; i++)
    {
        if (m_pEquipSlot[i].m_addInfo == itemIdx)
        {
            slot = i;
            return 1;
        }
    }
    return 0;
}

int CInventory::check_vilmark_dungeon_item_exist(int itemIdx) const
{
    for (int i = 3; i <= 8; i++)
    {
        if (m_pEquipSlot[i].m_addInfo == itemIdx)
        {
            return i;
        }
    }
    int start = 0;
    int end = 0;
    StartEndPos((Inven_Item::ITEM_TYPE)3, start, end);
    for (int i = start; i <= end; i++)
    {
        if (m_pEquipSlot[i].m_addInfo == itemIdx)
        {
            return i;
        }
    }
    StartEndPos((Inven_Item::ITEM_TYPE)4, start, end);
    for (int i = start; i <= end; i++)
    {
        if (m_pEquipSlot[i].m_addInfo == itemIdx)
        {
            return i;
        }
    }
    return -1;
}

int CInventory::possible_to_insert(int itemIdx) const
{
    int type = GetItemType(itemIdx);
    if (type == 0)
    {
        return 0;
    }
    if (check_empty_count((Inven_Item::ITEM_TYPE)type, 1))
    {
        return 1;
    }
    CItem* pItem = G_CDataManager()->find_item(itemIdx);
    if (pItem == 0)
    {
        return 0;
    }
    if (!pItem->is_stackable())
    {
        return 0;
    }
    return (unsigned int)(~(unsigned int)check_item_exist(itemIdx)) >> 31;
}

int CInventory::removeDupliacteEquipItem()
{
    for (int i = 3; i <= 0xc8; i++)
    {
        if (m_pEquipSlot[i].m_field1 == 1 &&
            getExistEquipItem(m_pEquipSlot[i].m_addInfo, m_pEquipSlot[i].m_addInfo2, i) != -1)
        {
            int characNo = m_pParent != 0 ? ((CUserCharacInfo*)m_pParent)->getCurCharacNo() : 0;
            int itemIdx = m_pEquipSlot[i].m_addInfo;
            cMyTrace trace(__PRETTY_FUNCTION__, 0x16d2, 5);
            trace("DUPLICATE ITEM REMOVED %d at %d of %d", itemIdx, i, characNo);
            m_pEquipSlot[i].reset();
            return 1;
        }
    }
    return 0;
}

void CInventory::ShowAllExistItemsForErrorCheck() const
{
    for (int i = 3; i < 0x138; i++)
    {
        if (m_pEquipSlot[i].m_addInfo != 0)
        {
            LogManager::logFormat(
                1, "inventory.cpp",
                __PRETTY_FUNCTION__, 0x1af0,
                "Inventory(%d) : %d\n", i, m_pEquipSlot[i].m_addInfo);
        }
    }
    for (int i = 0; i < 0x1a; i++)
    {
        if (m_invenItem[i].m_addInfo != 0)
        {
            LogManager::logFormat(
                1, "inventory.cpp",
                __PRETTY_FUNCTION__, 0x1af6,
                "Equip(%d) : %d\n", i, m_invenItem[i].m_addInfo);
        }
    }
}

int CInventory::checkToFix() const
{
    for (int i = 3; i < 9; i++)
    {
        if (m_pEquipSlot[i].m_addInfo != 0 && m_pEquipSlot[i].m_field1 != 1 &&
            !checkStackableLimit((unsigned long)m_pEquipSlot[i].m_addInfo,
                                 m_pEquipSlot[i].get_add_info()))
        {
            return 1;
        }
    }
    for (int i = 0x39; i <= 0xc8; i++)
    {
        if (m_pEquipSlot[i].m_addInfo != 0 &&
            !checkStackableLimit((unsigned long)m_pEquipSlot[i].m_addInfo,
                                 m_pEquipSlot[i].get_add_info()))
        {
            return 1;
        }
    }
    return 0;
}

void CInventory::CheckNFix()
{
    for (int i = 3; i < 9; i++)
    {
        if (m_pEquipSlot[i].m_addInfo != 0 && m_pEquipSlot[i].m_field1 != 1 &&
            !checkStackableLimit((unsigned long)m_pEquipSlot[i].m_addInfo,
                                 m_pEquipSlot[i].get_add_info()))
        {
            m_pEquipSlot[i].m_addInfo2 = 1;
        }
    }
    for (int i = 0x39; i < 0xc9; i++)
    {
        if (m_pEquipSlot[i].m_addInfo != 0 &&
            !checkStackableLimit((unsigned long)m_pEquipSlot[i].m_addInfo,
                                 m_pEquipSlot[i].get_add_info()))
        {
            m_pEquipSlot[i].m_addInfo2 = 1;
        }
    }
}

void CInventory::ChecknFixWineAndMoru()
{
    for (int i = 3; i < 9; i++)
    {
        if (m_pEquipSlot[i].m_addInfo != 0 && m_pEquipSlot[i].m_field1 != 1 &&
            !IsOverStackableItem(m_pEquipSlot[i].m_addInfo,
                                 (unsigned char)m_pEquipSlot[i].m_field1) &&
            m_pEquipSlot[i].m_addInfo2 > 799)
        {
            m_pEquipSlot[i].m_addInfo2 = 1;
        }
    }
    for (int i = 0x39; i < 0xc9; i++)
    {
        if (m_pEquipSlot[i].m_addInfo != 0 &&
            !IsOverStackableItem(m_pEquipSlot[i].m_addInfo,
                                 (unsigned char)m_pEquipSlot[i].m_field1) &&
            m_pEquipSlot[i].m_addInfo2 > 799)
        {
            m_pEquipSlot[i].m_addInfo2 = 1;
        }
    }
}

// ===================== 专家 Job（第九批 I2 补全） =====================

void CInventory::GetExpertJobAddtionalExp(
    ENUM_EXPERT_JOB_TYPE type, int param,
    STItemScript::SEXPERTJOB::stExpertJobAdditionalExp& out) const
{
    if (type == 0)
    {
        return;
    }
    for (int i = 10; i < 0x16; i++)
    {
        if (m_invenItem[i].m_addInfo != 0)
        {
            CItem* pItem = G_CDataManager()->find_item(m_invenItem[i].m_addInfo);
            if (pItem->isExpertJobUsable(type, param))
            {
                STItemScript::SEXPERTJOB::stExpertJobAdditionalExp exp;
                pItem->GetExpertJobAdditionalExp(exp);
                out.m_field0 = exp.m_field0 + out.m_field0;
                out.m_field4 = out.m_field4 + exp.m_field4;
            }
        }
    }
}

float CInventory::GetExpertJobCompoundMaterialVariation(ENUM_EXPERT_JOB_TYPE type, int param) const
{
    float total = 0.0f;
    if (type == 0)
    {
        return 0.0f;
    }
    for (int i = 10; i < 0x16; i++)
    {
        if (m_invenItem[i].m_addInfo != 0)
        {
            CItem* pItem = G_CDataManager()->find_item(m_invenItem[i].m_addInfo);
            if (pItem->isExpertJobUsable(type, param))
            {
                total = total + pItem->GetExpertJobCompoundMaterialVariation();
            }
        }
    }
    return total;
}

float CInventory::GetExpertJobCompoundRateVariation(ENUM_EXPERT_JOB_TYPE type, int param) const
{
    float total = 0.0f;
    if (type == 0)
    {
        return 0.0f;
    }
    for (int i = 10; i < 0x16; i++)
    {
        if (m_invenItem[i].m_addInfo != 0)
        {
            CItem* pItem = G_CDataManager()->find_item(m_invenItem[i].m_addInfo);
            if (pItem->isExpertJobUsable(type, param))
            {
                total = total + pItem->GetExpertJobCompoundRateVariation();
            }
        }
    }
    return total;
}

void CInventory::GetExpertJobCompoundResultVariation(
    ENUM_EXPERT_JOB_TYPE type, int param,
    STItemScript::SEXPERTJOB::stExpertCompoundResultVariation& out) const
{
    if (type == 0)
    {
        return;
    }
    for (int i = 10; i < 0x16; i++)
    {
        if (m_invenItem[i].m_addInfo != 0)
        {
            CItem* pItem = G_CDataManager()->find_item(m_invenItem[i].m_addInfo);
            if (pItem->isExpertJobUsable(type, param))
            {
                STItemScript::SEXPERTJOB::stExpertCompoundResultVariation var;
                pItem->GetExpertJobCompoundResultVariation(var);
                out.m_field0 = var.m_field0 + out.m_field0;
                out.m_field4 = var.m_field4 + out.m_field4;
            }
        }
    }
}

float CInventory::GetExpertJobSelfDisjointBigWinRate(ENUM_EXPERT_JOB_TYPE type, int param) const
{
    float total = 0.0f;
    if (type == 0)
    {
        return 0.0f;
    }
    for (int i = 10; i < 0x16; i++)
    {
        if (m_invenItem[i].m_addInfo != 0)
        {
            CItem* pItem = G_CDataManager()->find_item(m_invenItem[i].m_addInfo);
            if (pItem->isExpertJobUsable(type, param))
            {
                total = total + pItem->GetExpertJobSelfDisjointBigWinRate();
            }
        }
    }
    return total;
}

float CInventory::GetExpertJobSelfDisjointResultVariation(ENUM_EXPERT_JOB_TYPE type,
                                                          int param) const
{
    float total = 0.0f;
    if (type == 0)
    {
        return 0.0f;
    }
    for (int i = 10; i < 0x16; i++)
    {
        if (m_invenItem[i].m_addInfo != 0)
        {
            CItem* pItem = G_CDataManager()->find_item(m_invenItem[i].m_addInfo);
            if (pItem->isExpertJobUsable(type, param))
            {
                total = total + pItem->GetExpertJobSelfDisjointResultVariation();
            }
        }
    }
    return total;
}

// ===================== 物品操作 / 槽处理（第九批 I2 补全） =====================

void CInventory::update_item(INVEN_TYPE invenType, int slot, Inven_Item item)
{
    if (CheckValidSlot(invenType, slot) != 1)
    {
        return;
    }
    switch (invenType)
    {
    case 0:
        m_invenItem[slot] = item;
        break;
    case 1:
        m_pEquipSlot[slot] = item;
        break;
    case 2:
        m_pAvatarSlot[slot] = item;
        break;
    case 3:
        m_pCreatureSlot[slot] = item;
        break;
    }
}

// ============================================================================
// GetIntegratedPvPItemAttr（3 参重载，ORIG T 0x084fc651）。MakeItemPacket 直接依赖。
// ORIG 反汇编（84fc651）：G_GameWorld()->IsIntegratedPvPBaseChannel() 且
// G_CDataManager()->GetUpgradeRevisionPvPChannel()==0 时，若 invenType 非 0/非 1
// 且 slot 在 (2, 0x38] 区间 → _getIntegratedPvPItemAttr(item)；否则 → item.GetItemAttr()。
// 返回 unsigned char（ORIG 无 movzbl，值在 %al）。依赖 _getIntegratedPvPItemAttr。
// ============================================================================
unsigned char GetIntegratedPvPItemAttr(const Inven_Item& item, INVEN_TYPE invenType,
                                       int slot)
{
    // ORIG 0x84fc651：外层 && 控制流；内层 switch(invenType)，case 0 空 break
    // （产生 je→nop→GetItemAttr 的 pad 形态），case 1 内做 slot 区间 (2,0x38] 判定。
    if (G_GameWorld()->IsIntegratedPvPBaseChannel() &&
        G_CDataManager()->GetUpgradeRevisionPvPChannel() == 0)
    {
        switch (invenType)
        {
        case INVEN_TYPE_INVEN:
            break;
        case INVEN_TYPE_EQUIP:
            if (slot > 2 && slot <= 0x38)
            {
                return (unsigned char)_getIntegratedPvPItemAttr(item);
            }
            break;
        }
    }
    return item.GetItemAttr();
}

// ============================================================================
// CInventory::MakeItemPacket（ORIG 0x084fc6bc，0x1009 字节）。
// 按 INVEN_TYPE 分 4 组槽位发包：EQUIP(1) 含 0/1/2 三个特殊槽（0=money、
// 1=coin+payCoin+eventCoin、2=GetWinPoint）与普通装备槽；AVATAR(2) 走
// m_pAvatarSlot（含 jewel socket / expansion info）；CREATURE(3) 走
// m_pCreatureSlot（ITEM_TYPE 5 宠时补 GetRemainDate）；INVEN(0) 走
// m_invenItem[slot]（含 PvP 综合通道/封印数分支、avatar 扩展、宠物日期、
// make3rdChroniclePacket）。空槽 / 非期望类型返回 false。
// 依赖：CCreatureMgr::GetRemainDate（头已声明，BLOCKED——producer 提供定义）。
// ============================================================================
bool CInventory::MakeItemPacket(INVEN_TYPE invenType, int slot, PacketGuard& packet) const
{
    InterfacePacketBuf& buf = reinterpret_cast<InterfacePacketBuf&>(packet);
    const Inven_Item* item;
    GameWorld* world;
    bool integratedPvP;
    int nowTick;
    char* expansion;
    int attr;
    cMyTrace trace(__PRETTY_FUNCTION__, 0x758, 5);

    switch (invenType)
    {
    case INVEN_TYPE_EQUIP:
        buf.put_short(slot);
        if (slot == 0)
        {
            buf.put_int(0);
            buf.put_int(m_money);
            buf.put_byte(0);
            buf.put_short(0);
            buf.put_byte(0);
            buf.put_int(0);
            buf.put_byte(0);
            buf.put_short(0);
            buf.put_byte(0);
            buf.put_packet(g_emptySlot);
            return true;
        }
        if (slot == 1)
        {
            buf.put_int(1);
            buf.put_int(m_coin + m_payCoin + m_eventCoin);
            buf.put_byte(0);
            buf.put_short(0);
            buf.put_byte(0);
            buf.put_int(0);
            buf.put_byte(0);
            buf.put_short(0);
            buf.put_byte(0);
            buf.put_packet(g_emptySlot);
            return true;
        }
        if (slot == 2)
        {
            buf.put_int(2);
            buf.put_int(m_pParent->GetWinPoint());
            buf.put_byte(0);
            buf.put_short(0);
            buf.put_byte(0);
            buf.put_int(0);
            buf.put_byte(0);
            buf.put_short(0);
            buf.put_byte(0);
            buf.put_packet(g_emptySlot);
            return true;
        }
        item = &m_pEquipSlot[slot];
        if (item->m_addInfo == 0)
        {
            // 空装备槽（≥3）：item_idx 置 -1，无 3rd chronicle
            buf.put_item_idx(0xffffffff);
            buf.put_int(item->m_addInfo2);
            buf.put_byte(GetIntegratedPvPItemAttr(*item, INVEN_TYPE_EQUIP, slot));
            buf.put_short(item->m_fieldb);
            buf.put_byte(item->m_field0);
            world = G_GameWorld();
            if (!world->IsEnchantRevisionChannel())
                buf.put_int(item->m_fieldd);
            else
                buf.put_int(0);
            buf.put_byte(item->m_amp.getAbilityType());
            buf.put_short(item->m_amp.getAbilityValue());
            buf.put_byte(0);
            buf.put_packet(*item);
            return true;
        }
        buf.put_int(item->m_addInfo);
        buf.put_int(item->m_addInfo2);
        buf.put_byte(GetIntegratedPvPItemAttr(*item, INVEN_TYPE_EQUIP, slot));
        buf.put_short(item->m_fieldb);
        buf.put_byte(item->m_field0);
        world = G_GameWorld();
        if (!world->IsEnchantRevisionChannel())
            buf.put_int(item->m_fieldd);
        else
            buf.put_int(0);
        buf.put_byte(item->m_amp.getAbilityType());
        buf.put_short(item->m_amp.getAbilityValue());
        G_CDataManager()->m_itemUpgrade->make3rdChroniclePacket(m_pParent, *item, &packet);
        buf.put_packet(*item);
        return true;

    case INVEN_TYPE_AVATAR:
        item = &m_pAvatarSlot[slot];
        if (item->m_addInfo == 0)
        {
            trace(__PRETTY_FUNCTION__, 0x7a0);
            return false;
        }
        buf.put_short(slot);
        buf.put_int(item->m_addInfo);
        nowTick = OS_API::GetDateTimeTick();
        buf.put_int(m_avatarItemMgr.GetRemainDate(item->m_addInfo2, nowTick));
        buf.put_byte(item->GetItemAttr());
        buf.put_short(item->m_fieldb);
        buf.put_byte(item->m_field0);
        buf.put_int(item->m_fieldd);
        buf.put_byte(0);
        buf.put_short(0);
        buf.put_int(0x1e);
        buf.put_binary(m_avatarItemMgr.getJewelSocketData(item->m_addInfo2), 0x1e);
        expansion = (char*)m_avatarItemMgr.GetExpansionInfo(item->m_addInfo2);
        buf.put_int(4);
        if (expansion == 0)
        {
            stAvatarExpansionInfo_t expInfo;
            expInfo.init();
            buf.put_binary((char*)&expInfo, 4);
        }
        else
        {
            buf.put_binary(expansion, 4);
        }
        buf.put_byte(0);
        buf.put_packet(*item);
        return true;

    case INVEN_TYPE_CREATURE:
        item = &m_pCreatureSlot[slot];
        if (item->m_addInfo == 0)
        {
            trace(__PRETTY_FUNCTION__, 0x801);
            return false;
        }
        buf.put_short(slot);
        buf.put_int(item->m_addInfo);
        buf.put_int(item->m_addInfo2);
        buf.put_byte(item->GetItemAttr());
        buf.put_short(item->m_fieldb);
        buf.put_byte(item->m_field0);
        buf.put_int(item->m_fieldd);
        buf.put_byte(0);
        buf.put_short(0);
        if (item->m_field1 == (unsigned char)Inven_Item::ITEM_TYPE_CREATURE_EQUIP)
        {
            nowTick = OS_API::GetDateTimeTick();
            buf.put_int(m_creatureMgr.GetRemainDate(item->m_addInfo2, nowTick));
        }
        buf.put_byte(0);
        buf.put_packet(*item);
        return true;

    case INVEN_TYPE_INVEN:
        item = &m_invenItem[slot];
        if (item->m_addInfo == 0)
        {
            trace(__PRETTY_FUNCTION__, 0x884);
            return false;
        }
        buf.put_short(slot);
        buf.put_int(item->m_addInfo);
        buf.put_int(item->m_addInfo2);
        world = G_GameWorld();
        integratedPvP = false;
        if (world->IsIntegratedPvPBaseChannel())
        {
            if (G_CDataManager()->GetUpgradeRevisionPvPChannel() == 0)
                integratedPvP = true;
        }
        if (integratedPvP)
        {
            if (slot < 0x16 && ((1 << (slot & 0x1f)) & 0x3ff400) != 0)
                attr = (int)item->GetReSealCount() << 5;
            else
                attr = (int)item->GetItemAttr();
            buf.put_byte(attr);
        }
        else
        {
            buf.put_byte(item->GetItemAttr());
        }
        buf.put_short(item->m_fieldb);
        buf.put_byte(item->m_field0);
        world = G_GameWorld();
        if (!world->IsEnchantRevisionChannel())
            buf.put_int(item->m_fieldd);
        else
            buf.put_int(0);
        buf.put_byte(item->m_amp.getAbilityType());
        buf.put_short(item->m_amp.getAbilityValue());
        if (item->isAvatarItemType())
        {
            buf.put_int(0x1e);
            buf.put_binary(m_avatarItemMgr.getJewelSocketData(item->m_addInfo2), 0x1e);
            expansion = (char*)m_avatarItemMgr.GetExpansionInfo(item->m_addInfo2);
            buf.put_int(4);
            if (expansion == 0)
            {
                stAvatarExpansionInfo_t expInfo;
                expInfo.init();
                buf.put_binary((char*)&expInfo, 4);
            }
            else
            {
                buf.put_binary(expansion, 4);
            }
        }
        if (item->m_field1 == (unsigned char)Inven_Item::ITEM_TYPE_CREATURE_EQUIP)
        {
            nowTick = OS_API::GetDateTimeTick();
            buf.put_int(m_creatureMgr.GetRemainDate(item->m_addInfo2, nowTick));
        }
        G_CDataManager()->m_itemUpgrade->make3rdChroniclePacket(m_pParent, *item, &packet);
        buf.put_packet(*item);
        return true;

    default:
        return false;
    }
}

// ============================================================================
// CInventory::MakeSecuRewarditemPacket（ORIG 0x084fd6d2）。仅 EQUIP(1) 生效：
// 空装备槽返回 0；否则写 put_short(slot)，item_idx 依 m_addInfo2 为 0 时置 -1，
// 再 put_int(m_addInfo2) 并返回 1。
// ============================================================================
int CInventory::MakeSecuRewarditemPacket(INVEN_TYPE invenType, int slot,
                                         PacketGuard& packet) const
{
    InterfacePacketBuf& buf = reinterpret_cast<InterfacePacketBuf&>(packet);
    if (invenType != INVEN_TYPE_EQUIP)
        return 0;
    if (m_pEquipSlot[slot].m_addInfo == 0)
        return 0;
    buf.put_short(slot);
    if (m_pEquipSlot[slot].m_addInfo == 0 || m_pEquipSlot[slot].m_addInfo2 == 0)
        buf.put_item_idx(0xffffffff);
    else
        buf.put_item_idx(m_pEquipSlot[slot].m_addInfo);
    buf.put_int(m_pEquipSlot[slot].m_addInfo2);
    return 1;
}

int CInventory::RemoveFPItem()
{
    return RemoveFPItem(m_invenItem, m_pEquipSlot);
}

int CInventory::RemoveFPItem(Inven_Item* inven, Inven_Item* equip)
{
    for (int i = 0; i < 0x1a; i++)
    {
        if (inven[i].m_addInfo != 0 && (unsigned int)inven[i].m_addInfo > 0x1963 &&
            (unsigned int)inven[i].m_addInfo < 7000)
        {
            inven[i].reset();
        }
    }
    for (int i = 3; i < 0x138; i++)
    {
        if (equip[i].m_addInfo != 0 && (unsigned int)equip[i].m_addInfo > 0x1963 &&
            (unsigned int)equip[i].m_addInfo < 7000)
        {
            equip[i].reset();
        }
    }
}

int CInventory::RemoveKCItem()
{
    return RemoveKCItem(m_invenItem, m_pEquipSlot);
}

int CInventory::RemoveKCItem(Inven_Item* inven, Inven_Item* equip)
{
    for (int i = 0; i < 0x1a; i++)
    {
        if (inven[i].m_addInfo != 0 && (unsigned int)inven[i].m_addInfo > 0x1963 &&
            (unsigned int)inven[i].m_addInfo < 7000)
        {
            CItem* pItem = G_CDataManager()->find_item(inven[i].m_addInfo);
            if (pItem == 0)
            {
                LogManager::logFormat(
                    1, "inventory.cpp",
                    __PRETTY_FUNCTION__, 0x23fe,
                    "CInventory::RemoveKCItem No EquipItem(%d) Found loopcount(%d)",
                    inven[i].m_addInfo, i);
            }
            else if (pItem->GetFinishPointPrice() != 0)
            {
                inven[i].reset();
            }
        }
    }
    for (int i = 3; i < 0x138; i++)
    {
        if (equip[i].m_addInfo != 0 && (unsigned int)equip[i].m_addInfo > 0x1963 &&
            (unsigned int)equip[i].m_addInfo < 7000)
        {
            CItem* pItem = G_CDataManager()->find_item(equip[i].m_addInfo);
            if (pItem == 0)
            {
                LogManager::logFormat(
                    1, "inventory.cpp",
                    __PRETTY_FUNCTION__, 0x2416,
                    "CInventory::RemoveKCItem No InvenItem(%d) Found loopcount(%d)",
                    equip[i].m_addInfo, i);
            }
            else if (pItem->GetFinishPointPrice() != 0)
            {
                equip[i].reset();
            }
        }
    }
    return 0;
}

int CInventory::sort(INVEN_TYPE invenType)
{
    if (invenType == 1)
    {
        for (int i = 1; i < 5; i++)
        {
            int start = 0;
            int end = 0;
            StartEndPos((Inven_Item::ITEM_TYPE)i, start, end);
            qsort(m_pEquipSlot + start, end - start + 1, 0x3d, _CompareSlot);
        }
        int start = 0;
        int end = 0;
        StartEndPos((Inven_Item::ITEM_TYPE)10, start, end);
        qsort(m_pEquipSlot + start, end - start + 1, 0x3d, _CompareSlot);
        return 0;
    }
    if (invenType == 3)
    {
        for (int i = 6; i < 8; i++)
        {
            int start = 0;
            int end = 0;
            StartEndPos((Inven_Item::ITEM_TYPE)i, start, end);
            qsort(m_pCreatureSlot + start, end - start + 1, 0x3d, _CompareSlot);
        }
        return 0;
    }
    return 1;
}

int CInventory::FindItemLock(unsigned char b, ENUM_ITEMSPACE& space, int& slot) const
{
    space = (ENUM_ITEMSPACE)3;
    slot = 0;
    while (slot < 0x16)
    {
        if (((stAmplifyOption_t*)((char*)&m_invenItem[slot] + 0x11))->GetLock() == b)
        {
            return 1;
        }
        slot++;
    }
    space = (ENUM_ITEMSPACE)0;
    slot = 0;
    while (slot < 0x138)
    {
        if (((stAmplifyOption_t*)((char*)&m_pEquipSlot[slot] + 0x11))->GetLock() == b)
        {
            return 1;
        }
        slot++;
    }
    space = (ENUM_ITEMSPACE)7;
    slot = 0;
    while (slot < 0xf2)
    {
        if (((stAmplifyOption_t*)((char*)&m_pCreatureSlot[slot] + 0x11))->GetLock() == b)
        {
            return 1;
        }
        slot++;
    }
    space = (ENUM_ITEMSPACE)1;
    slot = 0;
    while (true)
    {
        if (slot > 0x68)
        {
            return 0;
        }
        if (((stAmplifyOption_t*)((char*)&m_pAvatarSlot[slot] + 0x11))->GetLock() == b)
        {
            break;
        }
        slot++;
    }
    return 1;
}

int CInventory::tryInsertAvatarIntoInventory(Inven_Item item) const
{
    for (int i = 0; i <= 0x68; i++)
    {
        if (m_pAvatarSlot[i].m_addInfo == 0)
        {
            return i;
        }
    }
    return -1;
}

int CInventory::insertAvatarIntoInventory(Inven_Item item, eAvatarItemAddReason reason, bool bLog)
{
    for (int i = 0; i <= 0x68; i++)
    {
        if (m_pAvatarSlot[i].m_addInfo == 0)
        {
            m_pAvatarSlot[i] = item;
            if (bLog)
            {
                const char* agency = m_avatarItemMgr.GetIPGAgencyNo(
                    m_pAvatarSlot[i].get_add_info());
                if (agency == 0)
                {
                    agency = "";
                }
                m_pParent->m_historyLog.AvatarItemAdd(
                    item.m_addInfo, m_pAvatarSlot[i].get_add_info(), agency, reason);
            }
            return i;
        }
    }
    return -1;
}

int CInventory::insertAvatarIntoSpecificSlot(Inven_Item item, int slot,
                                              eAvatarItemAddReason reason, bool bLog)
{
    int result = -1;
    if (slot < 0x69)
    {
        if (m_pAvatarSlot[slot].m_addInfo == 0)
        {
            m_pAvatarSlot[slot] = item;
            result = slot;
        }
        else
        {
            for (int i = 0; i < 0x69; i++)
            {
                if (m_pAvatarSlot[i].m_addInfo == 0)
                {
                    m_pAvatarSlot[i] = item;
                    result = i;
                    break;
                }
            }
        }
        if (result >= 0 && bLog)
        {
            const char* agency = m_avatarItemMgr.GetIPGAgencyNo(
                m_pAvatarSlot[result].get_add_info());
            if (agency == 0)
            {
                agency = "";
            }
            m_pParent->m_historyLog.AvatarItemAdd(
                item.m_addInfo, m_pAvatarSlot[result].get_add_info(), agency, reason);
        }
    }
    return result;
}

int CInventory::use_equip(int slot)
{
    if (slot < 0 || slot > 0x19)
    {
        return 1;
    }
    if (m_invenItem[slot].m_field1 != 1)
    {
        return 0x13;
    }
    CEquipItem* pItem = (CEquipItem*)G_CDataManager()->find_item(m_invenItem[slot].m_addInfo);
    if (pItem == 0)
    {
        return 0x11;
    }
    if (m_invenItem[slot].m_fieldb == 0 || pItem->get_endurance() == 0)
    {
        return 0xb;
    }
    if (pItem->m_itemType < 10)   // ORIG 0x84ff81c：CEquipItem+0x234（m_itemType）< 10 → 不可用
    {
        return 0xb;
    }
    if (m_invenItem[slot].m_addInfo == 0)
    {
        return 0x11;
    }
    if (m_invenItem[slot].m_fieldb == 0)
    {
        return 0x16;
    }
    if (m_invenItem[slot].m_fieldb != 0)
    {
        m_invenItem[slot].m_fieldb--;
    }
    return 0;
}

int CInventory::try_use_item(int slot, int count, int invenType) const
{
    if (count < 0)
    {
        return 1;
    }
    if (invenType == 7)
    {
        return 1;
    }
    if ((slot < 3 || slot > 8) && (slot < 0x39 || slot > 0x68))
    {
        return 4;
    }
    if (m_pEquipSlot[slot].m_addInfo == 0)
    {
        return 0x17;
    }
    CItem* pItem = G_CDataManager()->find_item(m_pEquipSlot[slot].m_addInfo);
    if (pItem == 0)
    {
        return 1;
    }
    if (pItem->GetItemType() == 0x12)
    {
        return 0;
    }
    if (m_pEquipSlot[slot].m_addInfo2 < count)
    {
        return 1;
    }
    return 0;
}

void CInventory::checkEquipmentState(int a, int b, int& c, int& d, int& e, int& f, int& g,
                                     int& h)
{
    if (a < 9 && b < 5)
    {
        if (m_invenItem[10].m_addInfo == 0)
        {
            c++;
        }
        else if (m_invenItem[10].m_fieldb == 0)
        {
            f++;
        }
        for (int i = 0xc; i < 0x16; i++)
        {
            if (i <= 0x13)
            {
                if (m_invenItem[i].m_addInfo == 0)
                {
                    c++;
                }
                if (i < 0x11)
                {
                    CEquipItem* pItem = (CEquipItem*)G_CDataManager()->find_item(
                        m_invenItem[i].m_addInfo);
                    if (pItem != 0)
                    {
                        if (m_invenItem[i].m_fieldb == 0)
                        {
                            f++;
                        }
                        int subType = pItem->get_sub_type();
                        if (a == 4 && b == 3)
                        {
                            if (subType == 0 || subType == 3 || subType == 4)
                            {
                                h++;
                            }
                            else
                            {
                                d += 2;
                                e = 2;
                            }
                        }
                        else if (*(int*)((char*)_S_CHARAC_JOB_MASTARY + (a * 5 + b) * 4) == subType)
                        {
                            h++;
                        }
                        else if (*(int*)((char*)_S_CHARAC_JOB_MASTARY + a * 0x14) == subType)
                        {
                            d++;
                            e = e == 2 ? 2 : 1;
                        }
                        else
                        {
                            d += 2;
                            e = 2;
                        }
                    }
                }
            }
            else
            {
                if (m_invenItem[i].m_addInfo != 0)
                {
                    g++;
                }
            }
        }
    }
}

int CInventory::erase_repeated_item(const std::vector<std::pair<int, int> >& items,
                                    std::vector<std::pair<int, int> >& out)
{
    for (std::vector<std::pair<int, int> >::const_iterator it = items.begin(); it != items.end();
         ++it)
    {
        int itemIdx = it->first;
        int type = GetItemType(itemIdx);
        if (type == 0)
        {
            continue;
        }
        int start = 0;
        int end = 0;
        StartEndPos((Inven_Item::ITEM_TYPE)type, start, end);
        int need = it->second;
        for (int i = start; i <= end; i++)
        {
            if (IsCreatureItemType(type))
            {
                if (m_pCreatureSlot[i].m_addInfo == itemIdx)
                {
                    if (isEquipableItemType(type))
                    {
                        need--;
                    }
                    else
                    {
                        need -= m_pCreatureSlot[i].m_addInfo2;
                    }
                }
            }
            else if (m_pEquipSlot[i].m_addInfo == itemIdx)
            {
                if (m_pEquipSlot[i].m_field1 == 1)
                {
                    need--;
                }
                else
                {
                    need -= m_pEquipSlot[i].m_addInfo2;
                }
            }
        }
        if (it->second > 0)
        {
            out.push_back(std::make_pair(itemIdx, need));
        }
    }
    return out.size();
}

// ---- 事件物品批处理（ORIG 0x85058da / 0x85067b2 / 0x8505db4 / 0x8506bb2 / 0x8508fd0 / 0x8509466）----

bool CInventory::tryDeleteEventItems(const std::vector<std::pair<int, int> >& items)
{
    if (m_pParent->getGmQuestFlag())
    {
        return true;
    }
    if (items.size() == 0)
    {
        return false;
    }
    int okCount = 0;
    for (std::vector<std::pair<int, int> >::const_iterator it = items.begin(); it != items.end();
         ++it)
    {
        int itemIdx = it->first;
        int amount = it->second;
        if (itemIdx == 0)
        {
            if (use_money(amount, (eMoneySubReason)4, false))
            {
                okCount++;
            }
        }
        else if (itemIdx == 2)
        {
            if ((unsigned int)amount <= (unsigned int)((CUserCharacInfo*)m_pParent)->GetWinPoint())
            {
                okCount++;
            }
        }
        else
        {
            int type = GetItemType(itemIdx);
            if (type == 0)
            {
                continue;
            }
            int start = 0;
            int end = 0;
            StartEndPos((Inven_Item::ITEM_TYPE)type, start, end);
            if (type == 1)
            {
                int need = amount;
                for (int i = start; i <= end; i++)
                {
                    if (m_pEquipSlot[i].m_addInfo == itemIdx)
                    {
                        okCount++;
                        m_pEquipSlot[i].reset();
                        need--;
                        if (need == 0)
                        {
                            break;
                        }
                    }
                }
                if (need > 0)
                {
                    for (int i = 3; i < 9; i++)
                    {
                        if (m_pEquipSlot[i].m_addInfo == itemIdx)
                        {
                            okCount++;
                            m_pEquipSlot[i].reset();
                            need--;
                            if (need == 0)
                            {
                                break;
                            }
                        }
                    }
                }
                if (need > 0)
                {
                    for (int i = 0; i < 0x1a; i++)
                    {
                        if (m_invenItem[i].m_addInfo == itemIdx)
                        {
                            okCount++;
                            m_invenItem[i].reset();
                            need--;
                            if (need == 0)
                            {
                                break;
                            }
                        }
                    }
                }
            }
            else
            {
                bool found = false;
                for (int i = 3; i < 9; i++)
                {
                    if (m_pEquipSlot[i].m_addInfo == itemIdx &&
                        amount <= m_pEquipSlot[i].m_addInfo2)
                    {
                        if (m_pEquipSlot[i].m_addInfo2 == amount)
                        {
                            m_pEquipSlot[i].reset();
                        }
                        else
                        {
                            m_pEquipSlot[i].m_addInfo2 -= amount;
                        }
                        found = true;
                        okCount++;
                        break;
                    }
                }
                if (!found)
                {
                    for (int i = start; i <= end; i++)
                    {
                        if (m_pEquipSlot[i].m_addInfo == itemIdx &&
                            amount <= m_pEquipSlot[i].m_addInfo2)
                        {
                            if (m_pEquipSlot[i].m_addInfo2 == amount)
                            {
                                m_pEquipSlot[i].reset();
                            }
                            else
                            {
                                m_pEquipSlot[i].m_addInfo2 -= amount;
                            }
                            found = true;
                            okCount++;
                            break;
                        }
                    }
                }
            }
        }
    }
    return okCount == (int)items.size();
}

int CInventory::tryInsertEventItems(const std::vector<std::pair<int, int> >& items)
{
    int insertCount = 0;
    if (items.size() == 0)
    {
        return 0;
    }
    for (std::vector<std::pair<int, int> >::const_iterator it = items.begin(); it != items.end();
         ++it)
    {
        int itemIdx = it->first;
        int count = it->second;
        if (itemIdx == 0 || itemIdx == 1)
        {
            insertCount++;
            continue;
        }
        CItem* pItem = G_CDataManager()->find_item(itemIdx);
        if (pItem == 0)
        {
            return insertCount;
        }
        Inven_Item inven;
        pItem->make_item(inven);
        if (!pItem->is_stackable())
        {
            if (pItem->IsAvatarItem())
            {
                for (int i = 0; i < count; i++)
                {
                    if (insertAvatarIntoInventory(inven, eAvatarItemAddReason_0, false) == -1)
                    {
                        return insertCount;
                    }
                    insertCount++;
                }
            }
            else
            {
                for (int i = 0; i < count; i++)
                {
                    if (insertItemIntoInventory(inven, (eItemAddReason)8, false, true) == -1)
                    {
                        return insertCount;
                    }
                    insertCount++;
                }
            }
        }
        else
        {
            inven.m_addInfo2 = count;
            if (insertItemIntoInventory(inven, (eItemAddReason)8, false, true) == -1)
            {
                return insertCount;
            }
            insertCount++;
        }
    }
    return insertCount;
}

int CInventory::delete_event_items(const std::vector<std::pair<int, int> >& items,
                                   std::vector<int>& outSlots, bool& bFound,
                                   eMoneySubReason moneyReason, eItemDelReason itemReason,
                                   bool bLog)
{
    if (m_pParent->getGmQuestFlag())
    {
        return 1;
    }
    int deleteCount = 0;
    if (items.size() == 0)
    {
        return 0;
    }
    for (std::vector<std::pair<int, int> >::const_iterator it = items.begin(); it != items.end();
         ++it)
    {
        int itemIdx = it->first;
        int amount = it->second;
        bool found = false;
        if (itemIdx == 0)
        {
            if (use_money(amount, moneyReason, true))
            {
                deleteCount++;
                outSlots.push_back(0);
                outSlots.push_back(0);
                outSlots.push_back(amount);
            }
            if (moneyReason == (eMoneySubReason)4 && m_pParent != 0)
            {
                GetInstanceValueStatistic()->AddValueStatistic(
                    (VALUE_STATISTIC_FIELD)0x11, m_pParent, (unsigned int)amount);
            }
        }
        else if (itemIdx == 2)
        {
            if ((unsigned int)amount <=
                (unsigned int)((CUserCharacInfo*)m_pParent)->GetWinPoint())
            {
                m_pParent->UseWinPoint(amount, (eWPSubReason)3);
                deleteCount++;
                outSlots.push_back(0);
                outSlots.push_back(2);
                outSlots.push_back(amount);
            }
        }
        else
        {
            int type = GetItemType(itemIdx);
            if (type == 0)
            {
                continue;
            }
            int start = 0;
            int end = 0;
            StartEndPos((Inven_Item::ITEM_TYPE)type, start, end);
            if (type == 1)
            {
                int need = amount;
                for (int i = start; i <= end; i++)
                {
                    if (m_pEquipSlot[i].m_addInfo == itemIdx)
                    {
                        deleteCount++;
                        outSlots.push_back(0);
                        outSlots.push_back(i);
                        outSlots.push_back(m_pEquipSlot[i].m_addInfo2);
                        delete_item(INVEN_TYPE_EQUIP, i, 1, itemReason, true);
                        need--;
                        if (need == 0)
                        {
                            break;
                        }
                    }
                }
                if (need > 0)
                {
                    for (int i = 3; i < 9; i++)
                    {
                        if (m_pEquipSlot[i].m_addInfo == itemIdx)
                        {
                            deleteCount++;
                            outSlots.push_back(0);
                            outSlots.push_back(i);
                            outSlots.push_back(m_pEquipSlot[i].m_addInfo2);
                            delete_item(INVEN_TYPE_EQUIP, i, 1, itemReason, true);
                            need--;
                            if (need == 0)
                            {
                                break;
                            }
                        }
                    }
                }
                if (need > 0)
                {
                    for (int i = 0; i < 0x1a; i++)
                    {
                        if (m_invenItem[i].m_addInfo == itemIdx)
                        {
                            if (i < 0xc)
                            {
                                bFound = true;
                            }
                            deleteCount++;
                            outSlots.push_back(3);
                            outSlots.push_back(i);
                            outSlots.push_back(m_invenItem[i].m_addInfo2);
                            delete_item(INVEN_TYPE_INVEN, i, 1, itemReason, true);
                            need--;
                            if (need == 0)
                            {
                                break;
                            }
                        }
                    }
                }
                if (need == 0)
                {
                    found = true;
                }
            }
            else
            {
                for (int i = 3; i < 9; i++)
                {
                    if (m_pEquipSlot[i].m_addInfo == itemIdx &&
                        amount <= m_pEquipSlot[i].m_addInfo2)
                    {
                        delete_item(INVEN_TYPE_EQUIP, i, amount, itemReason, true);
                        found = true;
                        deleteCount++;
                        outSlots.push_back(0);
                        outSlots.push_back(i);
                        outSlots.push_back(amount);
                        if (m_pParent != 0 && (int)itemReason == 8 && bLog)
                        {
                            GetInstanceCubeStatistic()->collectCubeStatistics(
                                itemIdx, amount, m_pParent,
                                (CUBE_STATISTIC_FIELD)0x65);
                        }
                        else if (m_pParent != 0 && (int)itemReason == 8 && !bLog)
                        {
                            GetInstanceCubeStatistic()->collectCubeStatistics(
                                itemIdx, amount, m_pParent,
                                (CUBE_STATISTIC_FIELD)0x64);
                        }
                        else if (m_pParent != 0 && (int)itemReason == 4)
                        {
                            GetInstanceCubeStatistic()->collectCubeStatistics(
                                itemIdx, amount, m_pParent,
                                (CUBE_STATISTIC_FIELD)0x66);
                        }
                        break;
                    }
                }
                if (!found)
                {
                    for (int i = start; i <= end; i++)
                    {
                        if (m_pEquipSlot[i].m_addInfo == itemIdx &&
                            amount <= m_pEquipSlot[i].m_addInfo2)
                        {
                            delete_item(INVEN_TYPE_EQUIP, i, amount, itemReason, true);
                            found = true;
                            deleteCount++;
                            outSlots.push_back(0);
                            outSlots.push_back(i);
                            outSlots.push_back(amount);
                            if (m_pParent != 0 && (int)itemReason == 8 && bLog)
                            {
                                GetInstanceCubeStatistic()->collectCubeStatistics(
                                    itemIdx, amount, m_pParent,
                                    (CUBE_STATISTIC_FIELD)0x65);
                            }
                            else if (m_pParent != 0 && (int)itemReason == 8 && !bLog)
                            {
                                GetInstanceCubeStatistic()->collectCubeStatistics(
                                    itemIdx, amount, m_pParent,
                                    (CUBE_STATISTIC_FIELD)0x64);
                            }
                            else if (m_pParent != 0 && (int)itemReason == 4)
                            {
                                GetInstanceCubeStatistic()->collectCubeStatistics(
                                    itemIdx, amount, m_pParent,
                                    (CUBE_STATISTIC_FIELD)0x66);
                            }
                            break;
                        }
                    }
                }
            }
            if (!found)
            {
                cMyTrace trace(__PRETTY_FUNCTION__, 0x1d3a, 5);
                trace("NOT CRITICAL BUT FOR OBSERVING, No Event Item %d FOUND!", itemIdx);
                return 0;
            }
        }
    }
    if (deleteCount != (int)items.size())
    {
        deleteCount = 0;
    }
    return deleteCount;
}

int CInventory::insert_item_special_slot(Inven_Item item, int a, int b,
                                         eMoneyAddReason moneyReason, eItemAddReason itemReason)
{
    if (item.m_addInfo == 0)
    {
        if (a < 0)
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x1742, 5);
            trace("[%s][%d]", __PRETTY_FUNCTION__, 0x1742);
            return -2;
        }
        gain_money(a, moneyReason, true, 0);
        return 0;
    }
    if (item.m_field1 == 1)
    {
        if (m_pEquipSlot[b].m_addInfo == 0)
        {
            m_pEquipSlot[b].setCopy(item);
            if (m_pParent != 0)
            {
                m_pParent->m_historyLog.ItemAdd(
                    INVEN_TYPE_EQUIP, 1, 1, item, itemReason);
            }
            return b;
        }
    }
    else
    {
        for (int i = 3; i < 9; i++)
        {
            if (m_pEquipSlot[i].m_addInfo == item.m_addInfo)
            {
                if (0x7fffffff - a < m_pEquipSlot[i].m_addInfo2)
                {
                    if (m_pParent != 0)
                    {
                        cMyTrace trace(__PRETTY_FUNCTION__, 0x177c, 4);
                        trace("INVEN INT_MAX setting 3 : mid(%d),characNo(%d),inven(%d)+add(%d) item_id(%d)",
                              NumberToString(m_pParent->get_acc_id(), 0),
                              m_pParent->get_charac_no(-1), m_pEquipSlot[i].m_addInfo2, a,
                              item.m_addInfo);
                    }
                    m_pEquipSlot[i].m_addInfo2 = 0x7fffffff;
                }
                else
                {
                    m_pEquipSlot[i].m_addInfo2 += a;
                }
                if (m_pParent != 0)
                {
                    m_pParent->m_historyLog.ItemAdd(
                        INVEN_TYPE_EQUIP, m_pEquipSlot[i].get_add_info(), item.get_add_info(),
                        item, itemReason);
                }
                return i;
            }
        }
        int start = 0;
        int end = 0;
        StartEndPos((Inven_Item::ITEM_TYPE)item.m_field1, start, end);
        for (int i = start; i <= end; i++)
        {
            if (m_pEquipSlot[i].m_addInfo == item.m_addInfo)
            {
                if (0x7fffffff - a < m_pEquipSlot[i].m_addInfo2)
                {
                    m_pEquipSlot[i].m_addInfo2 = 0x7fffffff;
                    if (m_pParent != 0)
                    {
                        cMyTrace trace(__PRETTY_FUNCTION__, 0x179f, 4);
                        trace("INT_MAX setting 3 : mid(%d),characNo(%d), inven(%d)+add(%d)",
                              NumberToString(m_pParent->get_acc_id(), 0),
                              m_pParent->get_charac_no(-1), m_pEquipSlot[i].m_addInfo2, a);
                    }
                }
                else
                {
                    m_pEquipSlot[i].m_addInfo2 += a;
                }
                if (m_pParent != 0)
                {
                    m_pParent->m_historyLog.ItemAdd(
                        INVEN_TYPE_EQUIP, m_pEquipSlot[i].get_add_info(), item.get_add_info(),
                        item, itemReason);
                }
                return i;
            }
        }
        if (m_pEquipSlot[b].m_addInfo == 0)
        {
            m_pEquipSlot[b].setCopy(item);
            if (m_pParent != 0)
            {
                m_pParent->m_historyLog.ItemAdd(
                    INVEN_TYPE_EQUIP, m_pEquipSlot[b].get_add_info(), item.get_add_info(), item,
                    itemReason);
            }
            return b;
        }
    }
    return -1;
}

void CInventory::MoveClearAvatar(int a, int b, int c, int d, int e, int f)
{
    CItem* pClearItem = 0;
    CItem* pTargetItem = 0;
    if (c != 0)
    {
        pClearItem = G_CDataManager()->find_item(c);
        if (pClearItem != 0 && pClearItem->IsClearAvatar() && f != 0)
        {
            pTargetItem = G_CDataManager()->find_item(f);
            if (pTargetItem != 0 && !pTargetItem->IsClearAvatar())
            {
                SetClearAvatar(d, f);
                if (d < 0 || d > 0x19)
                {
                    return;
                }
                stAvatarExpansionInfo_t* expInfo =
                    (stAvatarExpansionInfo_t*)GetAvatarItemMgrW()->GetExpansionInfo(
                        m_invenItem[d].m_addInfo2);
                if (expInfo == 0)
                {
                    return;
                }
                if (a - 10 < 0 || a - 10 > 0x68)
                {
                    return;
                }
                Inven_Item* slotItem = &m_pAvatarSlot[a - 10];
                std::map<int, AvatarVariation::colorRGB>& colorMap =
                    G_CDataManager()->m_avatarColor.m_colorMap;
                if (colorMap.find((int)expInfo->m_color[0]) == colorMap.end())
                {
                    LogManager::logFormat(
                        1, "inventory.cpp", __PRETTY_FUNCTION__, 0x21db,
                        "Avatar color error. ItemID(%d), colorId(%d)",
                        m_invenItem[d].m_addInfo2, (int)expInfo->m_color[0]);
                    return;
                }
                if (colorMap.find((int)expInfo->m_color[1]) == colorMap.end())
                {
                    LogManager::logFormat(
                        1, "inventory.cpp", __PRETTY_FUNCTION__, 0x21e2,
                        "Avatar color error. ItemID(%d), colorId(%d)",
                        m_invenItem[d].m_addInfo2, (int)expInfo->m_color[1]);
                    return;
                }
                if (!GetAvatarItemMgrW()->SetAvatarColor(
                        slotItem->m_addInfo2, expInfo->m_color[0], expInfo->m_color[1]))
                {
                    return;
                }
                SendAvatarEvent(0x31a, slotItem->m_addInfo2, expInfo->m_color[0],
                                expInfo->m_color[1], 0);
            }
        }
    }
    if (f != 0)
    {
        if (pTargetItem == 0)
        {
            pTargetItem = G_CDataManager()->find_item(f);
        }
        if (pTargetItem != 0 && pTargetItem->IsClearAvatar())
        {
            SetClearAvatar(d, 0);
        }
    }
}

void CInventory::SendAvatarEvent(int a, int b, int c, int d, int e) const
{
    if (m_pParent == 0)
    {
        LogManager::logFormat(
            1, "inventory.cpp", __PRETTY_FUNCTION__,
            0x212a, "CInventory::SendAvatarEven ERROR, m_pParent NULL ");
        return;
    }
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("inventory.cpp", 0x212f), true);
    int characNo = m_pParent->get_charac_no(-1);
    **guard << a;
    **guard << (int)m_pParent->GetUID();
    unsigned char b0 = (unsigned char)b;
    if (a == 0x34)
    {
        char buf[7];
        memset(buf, 0, sizeof(buf));
        *(int*)(buf + 0) = characNo;
        buf[4] = (char)b0;
        *(short*)(buf + 5) = (short)c;
        guard->put_binary(buf, 7);
    }
    else if (a < 0x35)
    {
        if (a == 0x32)
        {
            char buf[0x16];
            memset(buf, 0, sizeof(buf));
            *(int*)(buf + 0) = characNo;
            *(int*)(buf + 4) = d;
            *(int*)(buf + 8) = e;
            buf[0xc] = (char)b0;
            buf[0xd] = (char)c;
            *(int*)(buf + 0xe) = GetClearAvatar(b & 0xff) != 0 ? d : 0;
            *(int*)(buf + 0x12) = GetClearAvatar((unsigned char)c) != 0 ? e : 0;
            guard->put_binary(buf, 0x16);
        }
        else if (a == 0x31)
        {
            char buf[0x53];
            memset(buf, 0, sizeof(buf));
            *(int*)(buf + 0) = characNo;
            *(int*)(buf + 4) = b;
            *(int*)(buf + 8) = d;
            buf[0xc] = (char)e;
            buf[0xd] = (char)c;
            guard->put_binary(buf, 0x53);
        }
        else
        {
            char buf[0x70c];
            memset(buf, 0, sizeof(buf));
            *(int*)(buf + 0) = 1;
            *(int*)(buf + 4) = characNo;
            buf[8] = (char)b0;
            guard->put_binary(buf, 0x70c);
        }
    }
    else if (a == 0x2c3)
    {
        // ORIG GetInBuffer<SIG_AVATAR_OPTION_CHANGE>（sizeof 0xc）：[0]=b、
        // [4]=(char)c、[8]=characNo。
        char buf[0xc];
        memset(buf, 0, sizeof(buf));
        *(int*)(buf + 0) = b;
        buf[4] = (char)c;
        *(int*)(buf + 8) = characNo;
        guard->put_binary(buf, sizeof(buf));
    }
    else if (a == 0x31a)
    {
        char buf[0xc];
        memset(buf, 0, sizeof(buf));
        *(int*)(buf + 0) = characNo;
        *(int*)(buf + 4) = b;
        *(short*)(buf + 8) = (short)c;
        *(short*)(buf + 10) = (short)d;
        guard->put_binary(buf, 0xc);
    }
    else if (a == 0x36)
    {
        // ORIG GetInBuffer<SIG_AVATAR_CHANGE_STATE>（sizeof 0x51）：
        // +0x00=b、+0x04=c、+0x08=(char)d、+0x09=(char)e。
        SIG_AVATAR_CHANGE_STATE* s =
            guard->GetInBuffer<SIG_AVATAR_CHANGE_STATE>();
        *(int*)((char*)s + 0) = b;
        *(int*)((char*)s + 4) = c;
        *(char*)((char*)s + 8) = (char)d;
        *(char*)((char*)s + 9) = (char)e;
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

void CInventory::ExpiredAvatarItem(PacketGuard* packet)
{
    int now = OS_API::GetDateTimeTick();
    for (int i = 0; i <= 9; i++)
    {
        if (m_invenItem[i].m_addInfo != 0 && m_invenItem[i].m_addInfo2 != 0 &&
            m_invenItem[i].m_addInfo2 < now)
        {
            SendAvatarEvent(0x33, m_pParent->get_charac_no(-1), i, 0, 0);
            m_invenItem[i].reset();
        }
    }
    for (int i = 0; i < 0x69; i++)
    {
        if (m_pAvatarSlot[i].m_addInfo != 0 && m_pAvatarSlot[i].m_addInfo2 != 0 &&
            m_pAvatarSlot[i].m_addInfo2 < now)
        {
            SendAvatarEvent(0x33, m_pParent->get_charac_no(-1), i + 10, 0, 0);
            m_pAvatarSlot[i].reset();
        }
    }
}

int CInventory::CalBlankSlot(int* out,
                             const std::vector<std::pair<int, int> >& items) const
{
    if (items.size() == 0)
    {
        return 0;
    }
    for (std::vector<std::pair<int, int> >::const_iterator it = items.begin(); it != items.end();
         ++it)
    {
        int itemIdx = it->first;
        int count = it->second;
        if (itemIdx == 0 || itemIdx == 2)
        {
            continue;
        }
        int type = GetItemType(itemIdx);
        if (type == 0)
        {
            continue;
        }
        int start = 0;
        int end = 0;
        StartEndPos((Inven_Item::ITEM_TYPE)type, start, end);
        if (type == 1)
        {
            int found = 0;
            for (int i = start; i <= end; i++)
            {
                if (m_pEquipSlot[i].m_addInfo == itemIdx)
                {
                    found++;
                    out[1]++;
                    if (found == count)
                    {
                        break;
                    }
                }
            }
            if (found < count)
            {
                for (int i = 3; i < 9; i++)
                {
                    if (m_pEquipSlot[i].m_addInfo == itemIdx)
                    {
                        found++;
                        out[0]++;
                        if (found == count)
                        {
                            break;
                        }
                    }
                }
            }
        }
        else
        {
            if (IsCreatureItemType(type))
            {
                if (isEquipableItemType(type))
                {
                    int found = 0;
                    for (int i = start; i <= end; i++)
                    {
                        if (m_pEquipSlot[i].m_addInfo == itemIdx)
                        {
                            found++;
                            out[type]++;
                            if (found == count)
                            {
                                break;
                            }
                        }
                    }
                }
                else
                {
                    bool found = false;
                    for (int i = start; i <= end; i++)
                    {
                        if (m_pEquipSlot[i].m_addInfo == itemIdx &&
                            (unsigned int)m_pEquipSlot[i].m_addInfo2 == (unsigned int)count)
                        {
                            found = true;
                            out[type]++;
                            break;
                        }
                    }
                    if (!found)
                    {
                        for (int i = 3; i < 9; i++)
                        {
                            if (m_pEquipSlot[i].m_addInfo == itemIdx &&
                                (unsigned int)m_pEquipSlot[i].m_addInfo2 == (unsigned int)count)
                            {
                                out[0]++;
                                break;
                            }
                        }
                    }
                }
            }
            else
            {
                bool found = false;
                for (int i = start; i <= end; i++)
                {
                    if (m_pEquipSlot[i].m_addInfo == itemIdx &&
                        (unsigned int)m_pEquipSlot[i].m_addInfo2 == (unsigned int)count)
                    {
                        found = true;
                        out[type]++;
                        break;
                    }
                }
                if (!found)
                {
                    for (int i = 3; i < 9; i++)
                    {
                        if (m_pEquipSlot[i].m_addInfo == itemIdx &&
                            (unsigned int)m_pEquipSlot[i].m_addInfo2 == (unsigned int)count)
                        {
                            out[0]++;
                            break;
                        }
                    }
                }
            }
        }
    }
    return 0;
}

int CInventory::CalNeedSlot(int* out,
                            const std::vector<std::pair<int, int> >& items) const
{
    if (items.size() == 0)
    {
        return 0;
    }
    for (std::vector<std::pair<int, int> >::const_iterator it = items.begin(); it != items.end();
         ++it)
    {
        int itemIdx = it->first;
        int count = it->second;
        if (itemIdx == 0)
        {
            continue;
        }
        int type = GetItemType(itemIdx);
        if (type == 0)
        {
            continue;
        }
        if (type == 1)
        {
            out[1] += count;
        }
        else if (IsCreatureItemType(type))
        {
            if (isEquipableItemType(type))
            {
                out[type] += count;
            }
            else
            {
                int start = 0;
                int end = 0;
                StartEndPos((Inven_Item::ITEM_TYPE)type, start, end);
                bool found = false;
                for (int i = start; i <= end; i++)
                {
                    if (m_pCreatureSlot[i].m_addInfo == itemIdx)
                    {
                        found = true;
                        break;
                    }
                }
                if (!found)
                {
                    out[type]++;
                }
            }
        }
        else
        {
            if (type == 8)
            {
                if (getAvatarEmptySlot(0, 0x68) != -1)
                {
                    out[type]++;
                }
            }
            else
            {
                int start = 0;
                int end = 0;
                StartEndPos((Inven_Item::ITEM_TYPE)type, start, end);
                bool found = false;
                for (int i = start; i <= end; i++)
                {
                    if (m_pEquipSlot[i].m_addInfo == itemIdx)
                    {
                        found = true;
                        break;
                    }
                }
                if (!found)
                {
                    for (int i = 3; i < 9; i++)
                    {
                        if (m_pEquipSlot[i].m_addInfo == itemIdx)
                        {
                            found = true;
                            break;
                        }
                    }
                }
                if (!found)
                {
                    out[type]++;
                }
            }
        }
    }
    return 0;
}

int CInventory::CheckNeedItemList(const std::vector<std::pair<int, int> >& items) const
{
    if (m_pParent->getGmQuestFlag())
    {
        return 1;
    }
    if (items.size() == 0)
    {
        return 1;
    }
    for (std::vector<std::pair<int, int> >::const_iterator it = items.begin(); it != items.end();
         ++it)
    {
        int itemIdx = it->first;
        int count = it->second;
        if (itemIdx == 0)
        {
            if (m_money < count)
            {
                return 0;
            }
        }
        else if (itemIdx == 2)
        {
            if (*(int*)((char*)((CUserCharacInfo*)m_pParent)->getPVPResultR() + 0x58) < count)
            {
                return 0;
            }
        }
        else
        {
            int type = GetItemType(itemIdx);
            if (type == 0)
            {
                continue;
            }
            int start = 0;
            int end = 0;
            StartEndPos((Inven_Item::ITEM_TYPE)type, start, end);
            if (type == 1)
            {
                int found = 0;
                for (int i = start; i <= end; i++)
                {
                    if (m_pEquipSlot[i].m_addInfo == itemIdx)
                    {
                        found++;
                        if (found == count)
                        {
                            break;
                        }
                    }
                }
                if (found < count)
                {
                    for (int i = 3; i < 9; i++)
                    {
                        if (m_pEquipSlot[i].m_addInfo == itemIdx)
                        {
                            found++;
                            if (found == count)
                            {
                                break;
                            }
                        }
                    }
                }
                if (found < count)
                {
                    for (int i = 0; i < 0x1a; i++)
                    {
                        if (m_invenItem[i].m_addInfo == itemIdx)
                        {
                            found++;
                            if (found == count)
                            {
                                break;
                            }
                        }
                    }
                }
                if (found < count)
                {
                    return 0;
                }
            }
            else if (type == 5)
            {
                if (*(int*)((char*)&m_invenItem[22] + 0x0a) == itemIdx)
                {
                    return 1;
                }
                for (int i = start; i <= end; i++)
                {
                    if (m_pCreatureSlot[i].m_addInfo == itemIdx)
                    {
                        return 1;
                    }
                }
                return 0;
            }
            else
            {
                bool found = false;
                for (int i = start; i <= end; i++)
                {
                    if (m_pEquipSlot[i].m_addInfo == itemIdx)
                    {
                        if (m_pEquipSlot[i].m_addInfo2 < count)
                        {
                            return 0;
                        }
                        found = true;
                        break;
                    }
                }
                if (!found)
                {
                    for (int i = 3; i < 9; i++)
                    {
                        if (m_pEquipSlot[i].m_addInfo == itemIdx)
                        {
                            if (m_pEquipSlot[i].m_addInfo2 < count)
                            {
                                return 0;
                            }
                            found = true;
                            break;
                        }
                    }
                    if (!found)
                    {
                        return 0;
                    }
                }
            }
        }
    }
    return 1;
}

// ===================== 使用 / 装备 / Avatar（第九批 I2 补全） =====================

int CInventory::use_item(int slot, int count, int invenType)
{
    if (count < 0)
    {
        LogManager::logFormat(1, "inventory.cpp", __PRETTY_FUNCTION__,
                              0xf91, "count(%d) < 0", count);
        return 1;
    }
    if (invenType == 7)
    {
        if (m_pCreatureSlot[slot].m_addInfo == 0)
        {
            return 0x17;
        }
        if (m_pParent != 0)
        {
            CItem* pItem = G_CDataManager()->find_item(m_pCreatureSlot[slot].m_addInfo);
            if (pItem != 0)
            {
                int price = 0;
                G_Store()->GetSellItemPrice(m_pCreatureSlot[slot], pItem, (short)count, false,
                                            price);
                GetInstanceValueStatistic()->AddValueStatistic(
                    (VALUE_STATISTIC_FIELD)0x15, m_pParent, (unsigned int)price);
            }
        }
        if (m_pCreatureSlot[slot].m_addInfo2 == count)
        {
            int result = m_creatureMgr.UseItem(&m_pCreatureSlot[slot], slot);
            if (result != 0)
            {
                return result;
            }
            if (m_pParent != 0)
            {
                m_pParent->m_historyLog.ItemDel(
                    (INVEN_TYPE)3, 0, count, m_pCreatureSlot[slot], (eItemDelReason)3);
            }
            m_pCreatureSlot[slot].reset();
        }
        else
        {
            if (m_pCreatureSlot[slot].m_addInfo2 <= count)
            {
                m_pCreatureSlot[slot].reset();
                LogManager::logFormat(
                    1, "inventory.cpp", __PRETTY_FUNCTION__, 0xfd5,
                    "%s - m_CreatureSlot[slot_no].add_info(%d) < count(%d)",
                    ((CUserCharacInfo*)m_pParent)->getCurCharacName(),
                    m_pCreatureSlot[slot].m_addInfo2, count);
                return 1;
            }
            int result = m_creatureMgr.UseItem(&m_pCreatureSlot[slot], slot);
            if (result != 0)
            {
                return result;
            }
            m_pCreatureSlot[slot].m_addInfo2 -= count;
            if (m_pParent != 0)
            {
                m_pParent->m_historyLog.ItemDel(
                    (INVEN_TYPE)3, m_pCreatureSlot[slot].m_addInfo2, count,
                    m_pCreatureSlot[slot], (eItemDelReason)3);
            }
        }
        return 0;
    }
    if ((slot < 3 || slot > 8) && (slot < 0x39 || slot > 0x68))
    {
        return 4;
    }
    if (m_pEquipSlot[slot].m_addInfo == 0)
    {
        return 0x17;
    }
    CItem* pItem = G_CDataManager()->find_item(m_pEquipSlot[slot].m_addInfo);
    if (pItem == 0)
    {
        return 1;
    }
    if (pItem->GetItemType() == 0x12)
    {
        return 0;
    }
    if (m_pParent != 0)
    {
        int price = 0;
        G_Store()->GetSellItemPrice(m_pEquipSlot[slot], pItem, (short)count, false, price);
        GetInstanceValueStatistic()->AddValueStatistic((VALUE_STATISTIC_FIELD)0x15, m_pParent,
                                                       (unsigned int)price);
    }
    if (m_pEquipSlot[slot].m_addInfo2 == count)
    {
        int result = m_creatureMgr.UseItem(&m_pEquipSlot[slot], slot);
        if (result != 0)
        {
            return result;
        }
        if (m_pParent != 0)
        {
            m_pParent->m_historyLog.ItemDel(
                (INVEN_TYPE)1, 0, count, m_pEquipSlot[slot], (eItemDelReason)3);
        }
        m_pEquipSlot[slot].reset();
    }
    else
    {
        if (m_pEquipSlot[slot].m_addInfo2 <= count)
        {
            m_pEquipSlot[slot].reset();
            LogManager::logFormat(
                1, "inventory.cpp", __PRETTY_FUNCTION__, 0x1021,
                "%s - m_InvenSlot[slot_no].add_info(%d) < count(%d)",
                ((CUserCharacInfo*)m_pParent)->getCurCharacName(),
                m_pEquipSlot[slot].m_addInfo2, count);
            return 1;
        }
        int result = m_creatureMgr.UseItem(&m_pEquipSlot[slot], slot);
        if (result != 0)
        {
            return result;
        }
        m_pEquipSlot[slot].m_addInfo2 -= count;
        if (m_pParent != 0)
        {
            m_pParent->m_historyLog.ItemDel(
                (INVEN_TYPE)1, m_pEquipSlot[slot].m_addInfo2, count, m_pEquipSlot[slot],
                (eItemDelReason)3);
        }
    }
    return 0;
}

int CInventory::insertCreatureIntoSpecificSlot(Inven_Item item, int slot,
                                                eItemAddReason reason, bool bLog)
{
    if (slot >= 0xf2)
    {
        return -1;
    }
    int result = -1;
    if (item.m_addInfo == 0)
    {
        return -1;
    }
    if (m_pCreatureSlot[slot].m_addInfo == 0)
    {
        m_pCreatureSlot[slot] = item;
        result = slot;
    }
    else if (!m_pCreatureSlot[slot].isEquipableItemType())
    {
        if (m_pCreatureSlot[slot].m_addInfo == item.m_addInfo)
        {
            if (!checkStackableLimit((unsigned long)m_pCreatureSlot[slot].m_addInfo,
                                     m_pCreatureSlot[slot].get_add_info() + item.get_add_info()))
            {
                return -10;
            }
            m_pCreatureSlot[slot].set_add_info(
                m_pCreatureSlot[slot].get_add_info() + item.get_add_info());
            result = slot;
        }
        else
        {
            for (int i = 0; i < 0xf2; i++)
            {
                if (m_pCreatureSlot[i].m_addInfo == 0)
                {
                    m_pCreatureSlot[i] = item;
                    result = i;
                    break;
                }
                if (m_pCreatureSlot[i].m_addInfo == item.m_addInfo)
                {
                    m_pCreatureSlot[i].set_add_info(
                        m_pCreatureSlot[i].get_add_info() + item.get_add_info());
                    result = i;
                    break;
                }
            }
        }
    }
    if (result >= 0 && item.m_field1 != 5 && bLog)
    {
        m_pParent->m_historyLog.CreatureItemAdd(
            (INVEN_TYPE)3, item.m_addInfo, m_pCreatureSlot[result].get_add_info(),
            item.get_add_info(), item.m_field1, reason);
    }
    return result;
}

int CInventory::AddAvatarItem(int itemIdx, int avatarUid, bool b, char c, char d,
                              const char* agency, eAvatarItemAddReason reason, bool e,
                              unsigned short f)
{
    CItem* pItem = G_CDataManager()->find_item(itemIdx);
    if (pItem == 0 || !pItem->IsAvatarItem())
    {
        return -1;
    }
    stAvatarEmblemInfo_t emblem;
    emblem.init();
    ((CEquipItem*)pItem)->getAvatarSocket((int)c, emblem);
    for (int i = 0; i <= 0x68; i++)
    {
        if (m_pAvatarSlot[i].m_addInfo == 0)
        {
            int tick = OS_API::GetDateTimeTick();
            m_pAvatarSlot[i].m_addInfo = itemIdx;
            m_pAvatarSlot[i].m_addInfo2 = (int)m_avatarItemMgr.RegistNotAssignedAvatarItem(
                avatarUid, tick, agency, e, emblem);
            m_pAvatarSlot[i].m_field1 = 8;
            m_pAvatarSlot[i].m_fieldb = (short)c;
            m_pAvatarSlot[i].m_field0 = 0;
            *(unsigned short*)&m_pAvatarSlot[i].m_fieldd = 0;
            if (pItem->IsHiddenOption())
            {
                *(unsigned short*)&m_pAvatarSlot[i].m_fieldd = f;
            }
            *(unsigned short*)((char*)&m_pAvatarSlot[i].m_fieldd + 2) = 0;
            m_pAvatarSlot[i].ResetItemAttr();
            stAvatarExpansionInfo_t expansion;
            expansion.init();
            _SaveAvatarItem(i, m_avatarItemMgr.GetExpireDate(m_pAvatarSlot[i].m_addInfo2), b, c,
                            emblem, agency, m_pAvatarSlot[i].m_fieldd, expansion);
            m_pParent->m_historyLog.AvatarItemAdd(
                itemIdx, 0xffffffff, agency, reason);
            return i;
        }
    }
    return -1;
}

int CInventory::SortInvenSlot(Inven_Item::ITEM_TYPE type)
{
    int start = 0;
    int end = 0;
    int last = 0;
    StartEndPos(type, start, end);
    last = 0;
    for (int i = end; start <= i; i--)
    {
        if (m_pEquipSlot[i].m_addInfo != 0)
        {
            last = i;
            break;
        }
    }
    if (last == start || last == 0)
    {
        return 0x12;
    }
    bool full = true;
    for (int i = start; i <= last; i++)
    {
        if (m_pEquipSlot[i].m_addInfo == 0)
        {
            full = false;
            break;
        }
    }
    if (full)
    {
        return 0x12;
    }
    int dst = -1;
    for (int i = end; start <= i; i--)
    {
        if (m_pEquipSlot[i].m_addInfo != 0)
        {
            int moveResult = MoveItemToEmptySlotOfInventory(i, start, end);
            if (moveResult != 0)
            {
                cMyTrace trace(__PRETTY_FUNCTION__, 0x194d, 5);
                trace("MoveItemToEmptySlotOfInventory return %d ( index %d,s %d,e %d )",
                      moveResult, i, start, end);
                return moveResult;
            }
            int newLast = 0;
            for (int j = end; start <= j; j--)
            {
                if (m_pEquipSlot[j].m_addInfo != 0)
                {
                    newLast = j;
                    break;
                }
            }
            bool newFull = true;
            for (int j = start; j <= newLast; j++)
            {
                if (m_pEquipSlot[j].m_addInfo == 0)
                {
                    newFull = false;
                    break;
                }
            }
            if (newFull)
            {
                break;
            }
        }
    }
    return 0;
}

int CInventory::ChangeEquip(INVEN_TYPE invenType, int slot, int equipSlot)
{
    if (CheckValidSlot(invenType, slot) != 1)
    {
        return 1;
    }
    switch (invenType)
    {
    case 1:
    {
        if (equipSlot <= 9)
        {
            return 0x15;
        }
        Inven_Item slotItem1 = GetInvenSlot(1, slot);
        if (slotItem1.m_field0 != 0)
        {
            int secuResult = GlobalData::s_pSecuProtectionField->Check(m_pParent, (SECURITY_PROTCTION)0x16);
            if (secuResult != 0)
            {
                return secuResult;
            }
        }
        if (m_pParent != 0)
        {
            if (m_pParent->CheckItemLock(invenType, slot) &&
                GetInvenSlot(1, slot).m_field0 != 0)
            {
                return 0xd5;
            }
        }
        int result = 0;
        result = move_item((INVEN_TYPE)0, equipSlot, (INVEN_TYPE)1, slot);
        if (result != 0)
        {
            return result;
        }
        return 0;
    }
    case 2:
    {
        if (equipSlot > 9)
        {
            return 0x15;
        }
        int result = 0;
        result = move_item((INVEN_TYPE)0, equipSlot, (INVEN_TYPE)2, slot);
        if (result != 0)
        {
            return result;
        }
        return 0;
    }
    default:
    {
        return 0;
    }
    }
}

bool IsCreatureItemType(int itemType)
{
    return itemType == Inven_Item::ITEM_TYPE_CREATURE_EQUIP ||
           itemType == Inven_Item::ITEM_TYPE_CREATURE_STACK ||
           itemType == Inven_Item::ITEM_TYPE_ARTIFACT;
}

bool isEquipableItemType(int itemType)
{
    return itemType == Inven_Item::ITEM_TYPE_EQUIP ||
           itemType == Inven_Item::ITEM_TYPE_CREATURE_EQUIP ||
           itemType == Inven_Item::ITEM_TYPE_ARTIFACT;
}

int my_compare_unsigned_short(const void* a, const void* b)
{
    return *(const unsigned short*)a - *(const unsigned short*)b;
}

bool checkStackableLimit(unsigned long itemIdx, int count)
{
    CItem* item = G_CDataManager()->find_item(itemIdx);
    if (item != 0 && item->is_stackable())
    {
        CStackableItem* sitem = static_cast<CStackableItem*>(item);
        if (sitem->getStackableLimit() < count || count < 0)
        {
            return false;
        }
    }
    return true;
}

bool CheckEquipable(const Inven_Item& a, const Inven_Item& b)
{
    if (a.m_addInfo != 0)
    {
        if (a.m_field1 != Inven_Item::ITEM_TYPE_EQUIP &&
            a.m_field1 != Inven_Item::ITEM_TYPE_AVATAR)
        {
            return false;
        }
    }
    if (b.m_addInfo != 0)
    {
        if (b.m_field1 != Inven_Item::ITEM_TYPE_EQUIP &&
            b.m_field1 != Inven_Item::ITEM_TYPE_AVATAR)
        {
            return false;
        }
    }
    if (a.m_addInfo == 0 && b.m_addInfo == 0)
    {
        return false;
    }
    return true;
}

int getItemChecksum(int slot, int itemIdx, int a, int b, int c)
{
    // ORIG 0x889234d：先置 0 的局部复用；slot*10 产生 shl2/add/add 形态；/10 为 magic 常数除。
    int checksum = 0;
    checksum = (itemIdx + a + b + c) * (slot * 10) / 10;
    int high = checksum & 0xffff0000;
    checksum &= 0xffff;
    checksum += high >> 16;
    return checksum;
}
int finishItemChecksum(int* sum, int money, int coin, int a, int b)
{
    if (sum == 0)
    {
        return 0;
    }
    *sum = *sum + (money + coin + a);
    *sum = *sum ^ b;
    return *sum;
}

// ---- 迁移自 GameStubs.cpp 的 CInventory 桩方法（ORIG 符号签名已核对）----
// ORIG _ZN10CInventory15_SaveAvatarItemEiibcRK20stAvatarEmblemInfo_tPKcmRK23stAvatarExpansionInfo_t
// @0x08509e78：m_pParent 为空返回 0；否则经 DB_Q 投递 SIG_AVATAR_INSERT（0x31 头 +
// UID），set(槽位+10/到期/标志/agency/hidden) + setJewelSocket + sig+0x4f 写 expansion
// 前 4 字节，MsgQueueMgr::put(DB_Q)。
bool CInventory::_SaveAvatarItem(int slot, int expireDate, bool b, char c,
                                 const stAvatarEmblemInfo_t& emblem,
                                 const char* agency, unsigned long hidden,
                                 const stAvatarExpansionInfo_t& expansion)
{
    if (m_pParent == 0)
        return false;
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("inventory.cpp", 0x2360), true);
    **guard << 0x31;
    **guard << (int)m_pParent->GetUID();
    SIG_AVATAR_INSERT* sig = guard->GetInBuffer<SIG_AVATAR_INSERT>();
    unsigned int avatarUid = m_pAvatarSlot[slot].m_addInfo;
    int characNo = m_pParent->get_charac_no(-1);
    sig->set(characNo, avatarUid, slot + 10, expireDate, b, c, agency, 0, hidden);
    sig->setJewelSocket(emblem);
    // ORIG：*(int*)(sig + 0x4f) = *(int*)&expansion（stAvatarExpansionInfo_t 前 4 字节，
    // 即 m_color[0..1]；超出 0x4c 镜像区按 ORIG 原样写流缓冲）。
    short* dstColor = reinterpret_cast<short*>(reinterpret_cast<char*>(sig) + 0x4f);
    dstColor[0] = expansion.m_color[0];
    dstColor[1] = expansion.m_color[1];
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return true;
}

// ---- SIG_AVATAR_INSERT 非静态成员（ORIG 0x850d44c，this=ins） ----
// ORIG 反汇编逐字段：emblem(+0x00..0x1c) 7 组 dword + 1 word 写入 this+0x2d..+0x4a，
// 与 stAvatarEmblemInfo_t(pack(1), 5×(u16+u32)=0x1e) 完整镜像。
void SIG_AVATAR_INSERT::setJewelSocket(const stAvatarEmblemInfo_t& emblem)
{
    // ORIG 0x850d44c：8 条直拷（7 dword + 1 word）写入 this+0x2d 起；
    // 每语句独立以 this/emblem 直接寻址（O0 无局部指针中转，匹配 mov 形态）。
    *reinterpret_cast<int*>((char*)this + 0x2d) =
        *reinterpret_cast<const int*>((const char*)&emblem + 0x00);
    *reinterpret_cast<int*>((char*)this + 0x31) =
        *reinterpret_cast<const int*>((const char*)&emblem + 0x04);
    *reinterpret_cast<int*>((char*)this + 0x35) =
        *reinterpret_cast<const int*>((const char*)&emblem + 0x08);
    *reinterpret_cast<int*>((char*)this + 0x39) =
        *reinterpret_cast<const int*>((const char*)&emblem + 0x0c);
    *reinterpret_cast<int*>((char*)this + 0x3d) =
        *reinterpret_cast<const int*>((const char*)&emblem + 0x10);
    *reinterpret_cast<int*>((char*)this + 0x41) =
        *reinterpret_cast<const int*>((const char*)&emblem + 0x14);
    *reinterpret_cast<int*>((char*)this + 0x45) =
        *reinterpret_cast<const int*>((const char*)&emblem + 0x18);
    *reinterpret_cast<unsigned short*>((char*)this + 0x49) =
        *reinterpret_cast<const unsigned short*>((const char*)&emblem + 0x1c);
}

// ============================================================================
// CInventory::move_item（ORIG 0x08500688，_ZN10CInventory9move_itemE10INVEN_TYPEiS0_i）。
// 语义（docs/class_func_reports/CInventory/move_item.md 逐指令对照）：
//   - 双端 CheckValidSlot 失败 → cMyTrace + 0x11；
//   - 源为 0..2 且目的为 CREATURE 时 swap 两端（CREATURE 恒为源）；
//   - EQUIP↔INVEN：isIdentified 成对校验、IsAvatarItem/IsCreatureItem/IsEquipable、
//     IsRandomOption&&m_random.m_random.empty 拒绝、PeriodAfterUnsealing 写 m_fieldb
//     剩余天数、装备时清 m_field0 并 seal_Item 日志、0xd1 服务限制分支；
//   - EQUIP↔EQUIP / AVATAR↔AVATAR 三段交换；AVATAR 分支 MoveClearAvatar/
//     SendAvatarEvent(0x32)；CREATURE 走 CCreatureMgr::SwapCreatureItem。
// ============================================================================
int CInventory::move_item(INVEN_TYPE invenType, int slot, INVEN_TYPE dstType, int dstSlot)
{
    if (!(CheckValidSlot((int)invenType, slot) == 1 &&
          CheckValidSlot((int)dstType, dstSlot) == 1))
    {
        cMyTrace trace(__PRETTY_FUNCTION__,
                       0x112c, 5);
        trace("CInventory::move_item move_item return false 0, %d,%d,%d,%d",
              (int)invenType, slot, (int)dstType, dstSlot);
        return 0x11;
    }
    if ((unsigned)invenType < 3 && dstType == INVEN_TYPE_CREATURE)
    {
        std::swap<int>(slot, dstSlot);
        std::swap<INVEN_TYPE>(invenType, dstType);
    }
    if ((int)invenType == 1)
    {
        if ((int)dstType == 1)
        {
            // ---- EQUIP ↔ EQUIP ----
            int srcType = GetSlotItemType(slot);
            int dstItemType = GetSlotItemType(dstSlot);
            if (srcType == Inven_Item::ITEM_TYPE_INVALID ||
                dstItemType == Inven_Item::ITEM_TYPE_INVALID)
            {
                return 0x11;
            }
            if (srcType != 0 && dstItemType != 0 && srcType != dstItemType)
            {
                return 0x11;
            }
            if (srcType != 0 && m_pEquipSlot[dstSlot].m_addInfo != 0 &&
                (int)m_pEquipSlot[dstSlot].m_field1 != srcType)
            {
                return 0x11;
            }
            if (dstItemType != 0 && m_pEquipSlot[slot].m_addInfo != 0 &&
                (int)m_pEquipSlot[slot].m_field1 != dstItemType)
            {
                return 0x11;
            }
            Inven_Item tmp;
            tmp.setCopy(m_pEquipSlot[slot]);
            m_pEquipSlot[slot].setCopy(m_pEquipSlot[dstSlot]);
            m_pEquipSlot[dstSlot].setCopy(tmp);
            return 0;
        }
        if ((int)dstType > 1)
        {
            if ((int)dstType == 2)
            {
                return 0x11;
            }
            if ((int)dstType != 3)
            {
                return 0;
            }
            return 0x11;
        }
        if ((int)dstType != 0)
        {
            return 0;
        }
        // ---- EQUIP(src) → INVEN(dst)：脱下并换回原背包格 ----
        if (!CheckEquipable(m_pEquipSlot[slot], m_invenItem[dstSlot]))
        {
            return 0x11;
        }
        if (!m_pEquipSlot[slot].m_amp.isIdentified() ||
            !m_invenItem[dstSlot].m_amp.isIdentified())
        {
            return 0x11;
        }
        if (GetSlotItemType(slot) == Inven_Item::ITEM_TYPE_INVALID)
        {
            return 0x11;
        }
        Inven_Item tmp;
        tmp.setCopy(m_pEquipSlot[slot]);
        if (tmp.m_addInfo != 0)
        {
            CItem* pItem = G_CDataManager()->find_item(tmp.m_addInfo);
            if (pItem == 0 || m_pParent == 0)
            {
                cMyTrace trace(__PRETTY_FUNCTION__,
                               0x1276, 5);
                trace("CInventory::move_item move_item return false 4,%d", tmp.m_addInfo);
                return 0x11;
            }
            if (pItem->IsAvatarItem())
            {
                return 0x11;
            }
            if (pItem->IsCreatureItem())
            {
                return 0x11;
            }
            int equipable = IsEquipable(m_pParent, pItem, dstSlot);
            if (equipable != 0)
            {
                return equipable;
            }
            if (pItem->IsRandomOption() && m_pEquipSlot[slot].m_random.m_random.empty())
            {
                return 0x11;
            }
            if (pItem->PeriodAfterUnsealing() != 0)
            {
                int nExpire = GlobalData::s_systemTime_.getCurSec() +
                              pItem->PeriodAfterUnsealing() * 0x15180;
                if (nExpire != 0)
                {
                    tmp.m_fieldb =
                        (unsigned short)((nExpire - 0x44a54a80) / 0x15180);
                }
            }
        }
        m_pEquipSlot[slot].setCopy(m_invenItem[dstSlot]);
        tmp.m_field0 = 0;
        m_invenItem[dstSlot].setCopy(tmp);
        return 0;
    }
    if ((int)invenType > 1)
    {
        if ((int)invenType == 2)
        {
            if ((int)dstType == 1)
            {
                cMyTrace trace(__PRETTY_FUNCTION__,
                               0x1306, 5);
                trace("CInventory::move_item move_item return false 8");
                return 0x11;
            }
            if ((int)dstType == 2)
            {
                // ---- AVATAR ↔ AVATAR ----
                int srcAddInfo2 = m_pAvatarSlot[slot].m_addInfo2;
                int dstAddInfo2 = m_pAvatarSlot[dstSlot].m_addInfo2;
                Inven_Item tmp;
                tmp.setCopy(m_pAvatarSlot[slot]);
                m_pAvatarSlot[slot].setCopy(m_pAvatarSlot[dstSlot]);
                m_pAvatarSlot[dstSlot].setCopy(tmp);
                SendAvatarEvent(0x32, slot + 10, dstSlot + 10, srcAddInfo2, dstAddInfo2);
                return 0;
            }
            if ((int)dstType != 0)
            {
                return 0;
            }
            // ---- AVATAR(src) → INVEN(dst) ----
            Inven_Item tmp;
            tmp.setCopy(m_pAvatarSlot[slot]);
            if (!CheckEquipable(m_pAvatarSlot[slot], m_invenItem[dstSlot]))
            {
                return 0x11;
            }
            if (m_invenItem[dstSlot].m_addInfo != 0)
            {
                CItem* pItem = G_CDataManager()->find_item(m_invenItem[dstSlot].m_addInfo);
                if (m_pParent != 0 && pItem != 0)
                {
                    if (!pItem->IsAvatarItem())
                    {
                        return 0x11;
                    }
                    int equipable = IsEquipable(m_pParent, pItem, dstSlot);
                    if (equipable != 0)
                    {
                        return equipable;
                    }
                }
            }
            int srcAddInfo2 = m_pAvatarSlot[slot].m_addInfo2;
            int dstAddInfo2 = m_invenItem[dstSlot].m_addInfo2;
            MoveClearAvatar(slot + 10, srcAddInfo2, m_pAvatarSlot[slot].m_addInfo,
                            dstSlot, dstAddInfo2, m_invenItem[dstSlot].m_addInfo);
            m_pAvatarSlot[slot].setCopy(m_invenItem[dstSlot]);
            m_invenItem[dstSlot].setCopy(tmp);
            SendAvatarEvent(0x32, slot + 10, dstSlot, srcAddInfo2, dstAddInfo2);
            return 0;
        }
        if ((int)invenType != 3)
        {
            return 0;
        }
        if ((int)dstType == 1)
        {
            cMyTrace trace(__PRETTY_FUNCTION__,
                           0x131c, 5);
            trace("CInventory::move_item move_item return false 9");
            return 0x11;
        }
        if ((int)dstType == 3)
        {
            // ---- CREATURE ↔ CREATURE（首参为目的侧，与 ORIG 压栈序一致）----
            if (GetCreatureMgrW()->SwapCreatureItem(
                    &m_pCreatureSlot[dstSlot], dstSlot, (int)dstType,
                    &m_pCreatureSlot[slot], slot, (int)invenType))
            {
                return 0;
            }
            return 0x11;
        }
        if ((int)dstType != 0)
        {
            return 0;
        }
        // ---- CREATURE(src) → INVEN(dst) ----
        if (GetCreatureMgrW()->SwapCreatureItem(
                &m_invenItem[dstSlot], dstSlot, (int)dstType,
                &m_pCreatureSlot[slot], slot, (int)invenType))
        {
            return 0;
        }
        return 0x11;
    }
    if ((int)invenType != 0)
    {
        return 0;
    }
    if ((int)dstType == 1)
    {
        // ---- INVEN(src) → EQUIP(dst)：穿戴并与原装备交换 ----
        if (!CheckEquipable(m_invenItem[slot], m_pEquipSlot[dstSlot]))
        {
            return 0x11;
        }
        if (!isEquipmentPlaceableAtInventory(dstSlot))
        {
            return 0x11;
        }
        CItem* pItem = G_CDataManager()->find_item(m_pEquipSlot[dstSlot].m_addInfo);
        if (m_pParent != 0 && pItem != 0)
        {
            if (pItem->IsAvatarItem())
            {
                return 0x11;
            }
            if (pItem->IsCreatureItem())
            {
                return 0x11;
            }
            int equipable = IsEquipable(m_pParent, pItem, slot);
            if (equipable != 0)
            {
                return equipable;
            }
            if (pItem->IsRandomOption() &&
                m_pEquipSlot[dstSlot].m_random.m_random.empty())
            {
                return 0x11;
            }
        }
        if (GetSlotItemType(dstSlot) == Inven_Item::ITEM_TYPE_INVALID)
        {
            return 0x11;
        }
        if (!m_invenItem[slot].m_amp.isIdentified() ||
            !m_pEquipSlot[dstSlot].m_amp.isIdentified())
        {
            return 0x11;
        }
        if (m_pEquipSlot[dstSlot].m_field0 != 0 &&
            ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
                m_pParent, (RestrictCategory::Enum)1, 0xe))
        {
            return 0xd1;
        }
        Inven_Item tmp;
        tmp.setCopy(m_invenItem[slot]);
        m_invenItem[slot].setCopy(m_pEquipSlot[dstSlot]);
        if (m_pParent != 0 && m_invenItem[slot].m_field0 != 0)
        {
            m_pParent->m_historyLog.seal_Item(
                m_invenItem[slot].m_addInfo,
                (short)m_invenItem[slot].GetReSealCount(),
                eSealItemReason_1);
        }
        m_invenItem[slot].m_field0 = 0;
        if (pItem != 0 && pItem->PeriodAfterUnsealing() != 0)
        {
            int nExpire = GlobalData::s_systemTime_.getCurSec() +
                          pItem->PeriodAfterUnsealing() * 0x15180;
            if (nExpire != 0)
            {
                m_invenItem[slot].m_fieldb =
                    (unsigned short)((nExpire - 0x44a54a80) / 0x15180);
            }
        }
        m_pEquipSlot[dstSlot].setCopy(tmp);
        return 0;
    }
    if ((int)dstType < 2)
    {
        if ((int)dstType != 0)
        {
            return 0;
        }
        cMyTrace trace(__PRETTY_FUNCTION__, 0x122a, 5);
        trace("CInventory::move_item move_item return false 3");
        return 0x11;
    }
    if ((int)dstType != 2)
    {
        if ((int)dstType != 3)
        {
            return 0;
        }
        cMyTrace trace(__PRETTY_FUNCTION__, 0x1224, 5);
        trace("CInventory::move_item move_item return false 10");
        return 0x11;
    }
    // ---- INVEN(src) → AVATAR(dst) ----
    if (!CheckEquipable(m_invenItem[slot], m_pAvatarSlot[dstSlot]))
    {
        return 0x11;
    }
    if (m_pAvatarSlot[dstSlot].m_addInfo != 0)
    {
        CItem* pItem = G_CDataManager()->find_item(m_pAvatarSlot[dstSlot].m_addInfo);
        if (m_pParent != 0 && pItem != 0)
        {
            if (!pItem->IsAvatarItem())
            {
                return 0x11;
            }
            int equipable = IsEquipable(m_pParent, pItem, slot);
            if (equipable != 0)
            {
                return equipable;
            }
        }
    }
    int srcAddInfo2 = m_invenItem[slot].m_addInfo2;
    int dstAddInfo2 = m_pAvatarSlot[dstSlot].m_addInfo2;
    MoveClearAvatar(dstSlot + 10, dstAddInfo2, m_pAvatarSlot[dstSlot].m_addInfo,
                    slot, srcAddInfo2, m_invenItem[slot].m_addInfo);
    Inven_Item tmp;
    tmp.setCopy(m_invenItem[slot]);
    m_invenItem[slot].setCopy(m_pAvatarSlot[dstSlot]);
    m_pAvatarSlot[dstSlot].setCopy(tmp);
    SendAvatarEvent(0x32, slot, dstSlot + 10, srcAddInfo2, dstAddInfo2);
    return 0;
}

// ============================================================================
// CInventory::MakeEquipList（ORIG 0x084fe2e2，_ZNK10CInventory13MakeEquipListEPvb13ENUM_USERINFO14ENUM_EQUIPSLOT）。
// out 为 InterfacePacketBuf：put_byte 计数占位 → 逐非空槽写（flag=0 精简格式 /
// flag=1 全格式含 amplify、avatar jewel/expansion、creature 剩余天数、
// make3rdChroniclePacket、put_packet）→ 回填计数 → flag=0 时尾部写
// base+0x295（槽 10 m_amp 起始，ORIG 以 UpgradeSeparateInfo::GetUpgradeSeparate
// 单字节读取器跨类型读取）。槽位基址：userInfo==2 或 equipSlot==0 → m_invenItem，
// 否则 CExpandEquipslot::getExpandEquipslotW(equipSlot)（GetCharacExpandData(9)）。
// ============================================================================
int CInventory::MakeEquipList(void* out, bool flag, ENUM_USERINFO userInfo,
                              ENUM_EQUIPSLOT equipSlot) const
{
    InterfacePacketBuf* buf = (InterfacePacketBuf*)out;
    if (buf == 0)
    {
        return 0;
    }
    int count = 0;
    int listCount;
    if (!flag)
    {
        listCount = 0xc;
    }
    else if ((int)equipSlot == 0)
    {
        listCount = 0x1a;
    }
    else
    {
        listCount = 0xc;
    }
    int countIdx = buf->get_index();
    buf->put_byte(count);
    int nowTick = OS_API::GetDateTimeTick();
    CExpandEquipslot* expand = 0;
    if (m_pParent != 0)
    {
        expand = m_pParent->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)9);
    }
    const Inven_Item* slots;
    if ((int)userInfo == 2 || (int)equipSlot == 0)
    {
        slots = m_invenItem;
    }
    else
    {
        slots = expand->getExpandEquipslotW(equipSlot);
    }
    for (int i = 0; i < listCount; ++i)
    {
        int slotNo = ((int)equipSlot == 0) ? i : i + 10;
        if (slots[i].m_addInfo == 0)
        {
            continue;
        }
        ++count;
        if (!flag)
        {
            buf->put_byte(slotNo);
            if ((int)userInfo == 2 && GetClearAvatar(slotNo) != 0)
            {
                buf->put_int(GetClearAvatar(slotNo));
            }
            else
            {
                buf->put_int(slots[i].m_addInfo);
            }
            if ((int)userInfo == 2)
            {
                stAvatarExpansionInfo_t expansion;
                expansion.init();
                if (-1 < i && i < 10)
                {
                    // ORIG 读 this+(i+0xd0)*8+8/+10 → m_avatarVisible[i].m_field4/m_field6
                    expansion.m_color[0] = m_avatarVisible[i].m_field4;
                    expansion.m_color[1] = m_avatarVisible[i].m_field6;
                }
                buf->put_int(4);
                buf->put_binary((char*)&expansion, 4);
            }
            else if ((int)userInfo == 0)
            {
                stAvatarExpansionInfo_t expansion;
                expansion.init();
                if (-1 < i && i < 10 && slots[i].m_addInfo != 0)
                {
                    void* pExpansion =
                        GetAvatarItemMgrR()->GetExpansionInfo(slots[i].m_addInfo2);
                    if (pExpansion != 0)
                    {
                        memcpy(&expansion, pExpansion, 4);
                    }
                }
                buf->put_int(4);
                buf->put_binary((char*)&expansion, 4);
            }
            if (G_GameWorld()->IsIntegratedPvPBaseChannel() &&
                G_CDataManager()->GetUpgradeRevisionPvPChannel() == 0)
            {
                char upgradeByte = 0;
                if (slotNo != 10)
                {
                    if (slotNo < 10 || 9 < slotNo - 0xc)
                    {
                        upgradeByte = (char)slots[i].GetUpgrade();
                    }
                    else
                    {
                        upgradeByte = 0;
                    }
                }
                char ampFlag = 0;
                if (slots[i].m_amp.getAbilityType() != 0)
                {
                    ampFlag = 1;
                }
                buf->put_byte((unsigned char)((ampFlag & 1) | (upgradeByte * 2)));
            }
            else
            {
                // ORIG：读未初始化栈字节仅置 bit0（amp 有值→1），高位恒 0【按 ORIG 语义】
                unsigned char attrByte = 0;
                if (slots[i].m_amp.getAbilityType() != 0)
                {
                    attrByte |= 1;
                }
                buf->put_byte(attrByte);
            }
        }
        else
        {
            buf->put_byte(slotNo);
            buf->put_int(slots[i].m_addInfo);
            if (slotNo < 10)
            {
                buf->put_int(
                    GetAvatarItemMgrR()->GetRemainDate(slots[i].m_addInfo2, nowTick));
            }
            else
            {
                buf->put_int(slots[i].m_addInfo2);
            }
            if (G_GameWorld()->IsIntegratedPvPBaseChannel() &&
                G_CDataManager()->GetUpgradeRevisionPvPChannel() == 0)
            {
                unsigned char attrByte = 0;
                if ((unsigned)slotNo < 0x16 && ((1 << slotNo) & 0x3ff400) != 0)
                {
                    attrByte = (unsigned char)(slots[i].GetReSealCount() << 5);
                }
                else
                {
                    attrByte = slots[i].GetItemAttr();
                }
                buf->put_byte(attrByte);
            }
            else
            {
                buf->put_byte((unsigned char)(slots[i].GetItemAttr() & 0xff));
            }
            buf->put_short((int)slots[i].m_fieldb);
        }
        buf->put_int(GetClearAvatar(slotNo));
        if (!G_GameWorld()->IsEnchantRevisionChannel())
        {
            buf->put_int(slots[i].m_fieldd);
        }
        else
        {
            buf->put_int(0);
        }
        if (flag)
        {
            buf->put_byte((int)slots[i].m_amp.getAbilityType() & 0xff);
            buf->put_short((int)slots[i].m_amp.getAbilityValue() & 0xffff);
            if (slots[i].isAvatarItemType())
            {
                buf->put_int(0x1e);
                buf->put_binary(
                    (char*)GetAvatarItemMgrR()->getJewelSocketData(slots[i].m_addInfo2),
                    0x1e);
                buf->put_int(4);
                if (m_pParent != 0 &&
                    (const void*)((CUserCharacInfo*)m_pParent)->getTagCharacInvenR() ==
                        (const void*)this)
                {
                    stAvatarExpansionInfo_t expansion;
                    expansion.init();
                    if (-1 < i && i < 10)
                    {
                        expansion.m_color[0] = m_avatarVisible[i].m_field4;
                        expansion.m_color[1] = m_avatarVisible[i].m_field6;
                    }
                    buf->put_binary((char*)&expansion, 4);
                }
                else
                {
                    char* pExpansion =
                        (char*)GetAvatarItemMgrR()->GetExpansionInfo(slots[i].m_addInfo2);
                    if (pExpansion == 0)
                    {
                        stAvatarExpansionInfo_t expansion;
                        expansion.init();
                        buf->put_binary((char*)&expansion, 4);
                    }
                    else
                    {
                        buf->put_binary(pExpansion, 4);
                    }
                }
            }
            if (slots[i].m_field1 == 5)
            {
                int creatureTick = OS_API::GetDateTimeTick();
                int remainDate =
                    GetCreatureMgrR()->GetRemainDate(slots[i].m_addInfo2, creatureTick);
                buf->put_int(remainDate);
            }
            G_CDataManager()->m_itemUpgrade->make3rdChroniclePacket(
                m_pParent, slots[i], (PacketGuard*)buf);
            buf->put_packet(slots[i]);
        }
    }
    buf->put_byte(countIdx, count);
    if (flag)
    {
        return 1;
    }
    if (G_GameWorld()->IsIntegratedPvPBaseChannel() &&
        G_CDataManager()->GetUpgradeRevisionPvPChannel() == 0)
    {
        buf->put_byte(0);
    }
    else
    {
        // ORIG 0x84febb0：base+0x295 = 槽 10 m_amp 起始；以单字节读取器
        // UpgradeSeparateInfo::GetUpgradeSeparate 跨类型读取（ORIG 原样）。
        buf->put_byte(((UpgradeSeparateInfo*)((char*)slots + 0x295))->GetUpgradeSeparate());
    }
    return 1;
}

bool CInventory::delete_item(INVEN_TYPE invenType, int slot, int count,
                             eItemDelReason reason, bool bLog)
{
    if (CheckValidSlot(invenType, slot) != 1)
    {
        return 0;
    }
    if (count < 1)
    {
        return 0;
    }
    if (invenType == 1)
    {
        if (slot == 0)
        {
            return use_money(count, (eMoneySubReason)9, bLog);
        }
        if (m_pEquipSlot[slot].m_addInfo == 0)
        {
            return 0;
        }
        if (m_pEquipSlot[slot].m_field1 == 1)
        {
            if (bLog)
            {
                if (m_pParent == 0)
                {
                    cMyTrace local_7c(
                        __PRETTY_FUNCTION__,
                        0x1831, 5);
                    local_7c("History ERROR, m_pParent NULL, ItemDel ");
                }
                else
                {
                    m_pParent->m_historyLog.ItemDel((INVEN_TYPE)1, 0, 1, m_pEquipSlot[slot], reason);
                }
            }
            m_pEquipSlot[slot].reset();
        }
        else if (m_pEquipSlot[slot].m_addInfo2 == count)
        {
            if (bLog)
            {
                if (m_pParent == 0)
                {
                    cMyTrace local_6c(
                        __PRETTY_FUNCTION__,
                        0x184d, 5);
                    local_6c("History ERROR, m_pParent NULL, ItemDel ");
                }
                else
                {
                    m_pParent->m_historyLog.ItemDel((INVEN_TYPE)1, 0, m_pEquipSlot[slot].m_addInfo2,
                                                    m_pEquipSlot[slot], reason);
                }
            }
            m_pEquipSlot[slot].reset();
        }
        else
        {
            int cur = m_pEquipSlot[slot].m_addInfo2;
            if (cur == count || cur - count < 0)
            {
                LogManager::logFormat(
                    1, "inventory.cpp",
                    __PRETTY_FUNCTION__,
                    0x186f, "%s - m_InvenSlot[slot_no].add_info(%d) < count(%d)",
                    ((CUserCharacInfo*)m_pParent)->getCurCharacName(), cur, count);
                return 0;
            }
            m_pEquipSlot[slot].m_addInfo2 = cur - count;
            if (bLog)
            {
                if (m_pParent == 0)
                {
                    cMyTrace local_5c(
                        __PRETTY_FUNCTION__,
                        0x1868, 5);
                    local_5c("History ERROR, m_pParent NULL, ItemDel ");
                }
                else
                {
                    m_pParent->m_historyLog.ItemDel((INVEN_TYPE)1, m_pEquipSlot[slot].m_addInfo2, count,
                                                    m_pEquipSlot[slot], reason);
                }
            }
        }
    }
    else if (invenType < 2)
    {
        if (invenType == 0)
        {
            if (m_invenItem[slot].m_addInfo == 0)
            {
                return 0;
            }
            if (bLog)
            {
                if (m_pParent == 0)
                {
                    cMyTrace local_8c(
                        __PRETTY_FUNCTION__,
                        0x1811, 5);
                    local_8c("History ERROR, m_pParent NULL, ItemDel ");
                }
                else
                {
                    m_pParent->m_historyLog.ItemDel((INVEN_TYPE)0, 0, 1, m_invenItem[slot], reason);
                }
            }
            m_invenItem[slot].reset();
            if (slot < 10)
            {
                SendAvatarEvent(0x33, slot, 0, 0, 0);
            }
        }
    }
    else if (invenType == 2)
    {
        if (m_pAvatarSlot[slot].m_addInfo == 0)
        {
            return 0;
        }
        if (bLog)
        {
            if (m_pParent == 0)
            {
                cMyTrace local_4c(
                    __PRETTY_FUNCTION__,
                    0x187e, 5);
                local_4c("History ERROR, m_pParent NULL, ItemDel ");
            }
            else
            {
                const char* agency = m_avatarItemMgr.GetIPGAgencyNo(
                    m_pAvatarSlot[slot].m_addInfo2);
                m_pParent->m_historyLog.AvatarItemDel(m_pAvatarSlot[slot].m_addInfo,
                                                      m_pAvatarSlot[slot].m_addInfo2, agency,
                                                      (eAvatarItemDelReason)reason);
            }
        }
        m_pAvatarSlot[slot].reset();
    }
    else if (invenType == 3)
    {
        if (m_pCreatureSlot[slot].m_addInfo == 0)
        {
            return 0;
        }
        if (!m_pCreatureSlot[slot].isEquipableItemType())
        {
            if (m_pCreatureSlot[slot].m_addInfo2 == count)
            {
                if (bLog)
                {
                    if (m_pParent == 0)
                    {
                        cMyTrace local_2c(
                            __PRETTY_FUNCTION__,
                            0x18bb, 5);
                        local_2c("History ERROR, m_pParent NULL, ItemDel ");
                    }
                    else
                    {
                        m_pParent->m_historyLog.CreatureItemDel(
                            (INVEN_TYPE)3, m_pCreatureSlot[slot].m_addInfo, 0,
                            m_pCreatureSlot[slot].m_addInfo2, m_pCreatureSlot[slot].m_field1,
                            reason);
                    }
                }
                m_pCreatureSlot[slot].reset();
            }
            else
            {
                int cur = m_pCreatureSlot[slot].m_addInfo2;
                if (cur == count || cur - count < 0)
                {
                    LogManager::logFormat(
                        1, "inventory.cpp",
                        __PRETTY_FUNCTION__,
                        0x18da, "%s - m_CreatureSlot[slot_no].add_info(%d) < count(%d)",
                        ((CUserCharacInfo*)m_pParent)->getCurCharacName(), cur, count);
                    return 0;
                }
                m_pCreatureSlot[slot].m_addInfo2 = cur - count;
                if (bLog)
                {
                    if (m_pParent == 0)
                    {
                        cMyTrace local_1c(
                            __PRETTY_FUNCTION__,
                            0x18d2, 5);
                        local_1c("History ERROR, m_pParent NULL, ItemDel ");
                    }
                    else
                    {
                        m_pParent->m_historyLog.CreatureItemDel(
                            (INVEN_TYPE)3, m_pCreatureSlot[slot].m_addInfo, m_pCreatureSlot[slot].m_addInfo2,
                            count, m_pCreatureSlot[slot].m_field1, reason);
                    }
                }
            }
        }
        else
        {
            if (bLog)
            {
                if (m_pParent == 0)
                {
                    cMyTrace local_3c(
                        __PRETTY_FUNCTION__,
                        0x18a3, 5);
                    local_3c("History ERROR, m_pParent NULL, ItemDel ");
                }
                else
                {
                    m_pParent->m_historyLog.CreatureItemDel(
                        (INVEN_TYPE)3, m_pCreatureSlot[slot].m_addInfo, m_pCreatureSlot[slot].m_addInfo2, 1,
                        m_pCreatureSlot[slot].m_field1, reason);
                }
            }
            m_pCreatureSlot[slot].reset();
        }
    }
    return 1;
}

// ============================================================================
// CInventory::insert_event_items（ORIG 0x08506bb2，_ZN10CInventory18insert_event_itemsERKSt6vectorISt4pairIiiESaIS2_EERS0_IiSaIiEE15eMoneyAddReason14eItemAddReasonbPKcSD_）。
// items 元素 {itemIdx, count}；itemIdx==0 为金钱条目（>499999 走 cMyTrace 审计，
// gain_money 后 outSlots.push(0,0,gained)，差额邮件兜底）。
// 物品条目按 is_stackable/IsAvatarItem/造物蛋(type 0x16) 三路分发：
//   堆叠 → make_item+insertItemIntoInventory(reason,1,1)；
//   头像 → genIPGNo+AddAvatarItem(itemIdx,usablePeriod,...)+GetRemainDate；
//   造物蛋 → create_item+SetEggItem+insertItemIntoInventory(8,1,0)
//            +InsertCreatureItem；其余非堆叠逐份插入，失败邮件兜底。
// reason==8 时累计 cube 统计与 Store 卖价，函数尾 AddValueStatistic(10)。
// ORIG 怪癖（忠实保留）：头像/造物蛋在 itemReason!=8、SetEggItem 失败时
// 不推进迭代器直接回到循环条件；堆叠/头像插入失败且无邮件时跳到函数尾。
// 依赖：docs/class_func_reports/CInventory/insert_event_items.md。
// ============================================================================
int CInventory::insert_event_items(const std::vector<std::pair<int, int> >& items,
                                   std::vector<int>& outSlots, eMoneyAddReason moneyReason,
                                   eItemAddReason itemReason, bool bLog,
                                   const char* a, const char* b)
{
    typedef std::vector<std::pair<int, int> >::const_iterator Iterator;
    int count = 0;
    if (items.size() == 0)
    {
        return count;
    }
    std::pair<int, int> cur;
    Iterator it = items.begin();
    Inven_Item item;
    int totalValue = 0;
    while (it != items.end())
    {
        item.reset();
        cur = *it;
        int itemIdx = cur.first;
        int cnt = cur.second;
        if (itemIdx == 0)
        {
            // ---- 金钱条目 ----
            if (cnt > 499999 && m_pParent != 0)
            {
                cMyTrace trace(
                    __PRETTY_FUNCTION__,
                    0x1dc7, 4);
                trace("Quest reward money(%d) mid(%s) characno(%d) reason(%d)", cnt,
                      NumberToString(m_pParent->get_acc_id(), 0),
                      m_pParent->get_charac_no(-1), (int)moneyReason);
            }
            int gained = cnt;
            if (m_pParent != 0 && ((CUserCharacInfo*)m_pParent)->getCurCharacR() != 0)
            {
                // ORIG 0x08506d8d：gain_money 调在 charac 的 invenW 子背包实例上
                // （m_pParent->getCurCharacInvenW()，enableSaveInven 后返回 +0xda）。
                gained = ((CUserCharacInfo*)m_pParent)->getCurCharacInvenW()->
                    gain_money(cnt, moneyReason, true, 0);
            }
            outSlots.push_back(0);
            outSlots.push_back(0);
            outSlots.push_back(gained);
            ++count;
            ++it;   // ORIG：迭代器先推进，再判定邮件
            if ((int)moneyReason == 3)
            {
                totalValue += gained;
            }
            if (gained < cnt && bLog)
            {
                char title[0x15];
                strncpy(title, g_scriptStringManager_.findString(4, a, (bool*)0), 0x14);
                char body[0x100];
                strncpy(body, g_scriptStringManager_.findString(4, b, (bool*)0), 0xff);
                WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                    title, item, cnt - gained, m_pParent->getCurCharacNo(), body,
                    strlen(body), 0, m_pParent->GetServerGroup(), false, false);
            }
            continue;
        }

        CItem* pItem = G_CDataManager()->find_item(itemIdx);
        if (pItem == 0)
        {
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0x1e14, 5);
            trace("CInventory::insert_event_items, G_CDataManager()->find_item(%d)",
                  itemIdx);
            ++it;
            continue;
        }

        item.m_addInfo = itemIdx;
        if (!pItem->is_stackable())
        {
            if (!pItem->IsAvatarItem())
            {
                if (pItem->IsCreatureItem() && pItem->GetItemType() == 0x16)
                {
                    // ---- 造物蛋 ----
                    if ((int)itemReason != 8)
                    {
                        continue;   // ORIG 怪癖：不推进迭代器
                    }
                    G_CDataManager()->m_itemList->create_item(
                        ENUM_ITEM_CREATE_TYPE_0, item, cnt);
                    if (item.m_field1 == 5)
                    {
                        if (!user_creature::CCreatureItemConverter::SetEggItem(&item))
                        {
                            continue;   // ORIG 怪癖：不推进迭代器
                        }
                        int r = insertItemIntoInventory(item, (eItemAddReason)8, true, false);
                        if (r >= 0)
                        {
                            int expirationDate = pItem->getExpirationDate();
                            int usablePeriod = pItem->getUsablePeriod();
                            GetCreatureMgrW()->InsertCreatureItem(&item, r, 8,
                                                                  usablePeriod,
                                                                  expirationDate);
                            ++count;
                            outSlots.push_back(r);
                            outSlots.push_back(itemIdx);
                            outSlots.push_back(cnt);
                        }
                    }
                    ++it;
                    continue;
                }
                // ---- 普通非堆叠：逐份插入 ----
                for (int k = 0; k < cnt; ++k)
                {
                    pItem->make_item(item);
                    int r = insertItemIntoInventory(item, itemReason, true, true);
                    if (r == -1)
                    {
                        if (!bLog)
                        {
                            break;   // ORIG：跳出循环并推进主迭代器
                        }
                        char title[0x15];
                        strncpy(title, g_scriptStringManager_.findString(4, a, (bool*)0),
                                0x14);
                        char body[0x100];
                        strncpy(body, g_scriptStringManager_.findString(4, b, (bool*)0),
                                0xff);
                        WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                            title, item, 0, m_pParent->getCurCharacNo(), body,
                            strlen(body), 0, m_pParent->GetServerGroup(), false, false);
                        continue;   // 尝试下一份
                    }
                    ++count;
                    outSlots.push_back(r);
                    outSlots.push_back(itemIdx);
                    outSlots.push_back(item.m_addInfo2);
                    if ((int)itemReason == 8)
                    {
                        int price = 0;
                        G_Store()->GetSellItemPrice(item, pItem,
                                                    (short)item.m_addInfo2, false,
                                                    price);
                        totalValue += price;
                    }
                }
                ++it;
                continue;
            }
            // ---- Avatar 物品 ----
            if ((int)itemReason != 8)
            {
                continue;   // ORIG 怪癖：不推进迭代器
            }
            char ipg[0x18];
            WongWork::CGenUniqueNo::genIPGNo(
                (WongWork::CGenUniqueNo::ENUM_IPGNO_TYPE)2,
                (unsigned int)((CUserCharacInfo*)m_pParent)->getCurCharacNo(), ipg);
            int usablePeriod = pItem->getUsablePeriod();
            int r = AddAvatarItem(itemIdx, usablePeriod, false, 0, (char)-1, ipg,
                                  (eAvatarItemAddReason)8, false, 0);
            if (r == -1)
            {
                if (!bLog)
                {
                    goto finish_value_statistic;   // ORIG：跳到函数尾统计
                }
                WongWork::CMailBoxHelper::ReqDBSendNewAvatarMail(
                    m_pParent, ((CUserCharacInfo*)m_pParent)->getCurCharacNo(), itemIdx,
                    0, '\0', (char)-1, ipg, (char*)0, 0);
                ++it;
                continue;
            }
            ++count;
            outSlots.push_back(r);
            outSlots.push_back(itemIdx);
            int nowTick = GlobalData::s_systemTime_.getCurSec();
            int remainDate = GetAvatarItemMgrR()->GetRemainDate(
                GetInvenSlot(2, r).m_addInfo2, nowTick);
            outSlots.push_back(remainDate);
            ++it;
            continue;
        }
        // ---- 堆叠物品 ----
        item.m_addInfo2 = cnt;
        pItem->make_item(item);
        int r = insertItemIntoInventory(item, itemReason, true, true);
        if (r == -1)
        {
            if (!bLog)
            {
                goto finish_value_statistic;   // ORIG：跳到函数尾统计
            }
            char title[0x15];
            strncpy(title, g_scriptStringManager_.findString(4, a, (bool*)0), 0x14);
            char body[0x100];
            strncpy(body, g_scriptStringManager_.findString(4, b, (bool*)0), 0xff);
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                title, item, 0, m_pParent->getCurCharacNo(), body, strlen(body), 0,
                m_pParent->GetServerGroup(), false, false);
            ++it;
            continue;
        }
        ++count;
        outSlots.push_back(r);
        outSlots.push_back(itemIdx);
        outSlots.push_back(item.m_addInfo2);
        if ((int)itemReason == 8)
        {
            if (m_pParent != 0)
            {
                GetInstanceCubeStatistic()->collectCubeStatistics(
                    pItem, item.m_addInfo2, m_pParent, (CUBE_STATISTIC_FIELD)4);
            }
            int price = 0;
            G_Store()->GetSellItemPrice(item, pItem, (short)item.m_addInfo2, false,
                                        price);
            totalValue += price;
        }
        ++it;
        continue;
    }

finish_value_statistic:
    if (((int)itemReason == 8 || (int)moneyReason == 3) && m_pParent != 0)
    {
        GetInstanceValueStatistic()->AddValueStatistic((VALUE_STATISTIC_FIELD)0xa,
                                                       m_pParent, totalValue);
    }
    return count;
}

void CInventory::SendItemLockList(const Inven_Item* items, int count,
                                  ENUM_ITEMSPACE space) const
{
    if (m_pParent == 0)
        return;
    PacketGuard packet;
    InterfacePacketBuf* buf = (InterfacePacketBuf*)&packet;
    buf->clear();
    buf->put_header(0, 0xfb);
    int countIdx = buf->get_index();
    int cnt = 0;
    buf->put_short(0);
    for (int i = 0; i < count; ++i) {
        if (items[i].m_amp.GetLock() != 0) {
            buf->put_byte((int)space);
            buf->put_short(i);
            CExpandEquipslot* expand = m_pParent->GetCharacExpandDataR(
                (ENUM_CHARAC_EXPAND_TYPE)2);
            reinterpret_cast<const item_lock::CItemLock*>(expand)
                ->MakeItemLockPacket(packet,
                                     (unsigned char)items[i].m_amp.GetLock());
            ++cnt;
        }
    }
    buf->put_byte(countIdx, cnt);
    buf->finalize(true);
    m_pParent->Send(packet);
}

void CInventory::SendItemLockListInven() const
{
    SendItemLockList(m_pEquipSlot, 0x138, (ENUM_ITEMSPACE)0);
}

// ORIG 0x084fafbe / 0x084fafee：分别把 m_pAvatarSlot(0x69 槽)/m_pCreatureSlot(0xf2 槽)
// 的锁清单通过 SendItemLockList 发包（ENUM_ITEMSPACE 1=avatar、7=creature）。
void CInventory::SendItemLockListAvatar() const
{
    SendItemLockList(m_pAvatarSlot, 0x69, (ENUM_ITEMSPACE)1);
}

void CInventory::SendItemLockListCreature() const
{
    SendItemLockList(m_pCreatureSlot, 0xf2, (ENUM_ITEMSPACE)7);
}

// ============================================================================
// CInventory::MakeItemList（ORIG 0x084fd7b6 T，docs/class_func_reports/
// CInventory/MakeItemList.md 逐指令转写）。
// m_pParent 为空 → cMyTrace 报错返回 0；否则 interface packet 写头 0xd 后按
// INVEN_TYPE 分发：
//   EQUIP(1)：space 0 + 容量 + 计数槽；slot0/1/2 为固定货币/胜负点虚拟条目
//     （addInfo2 分别为 m_money、三币之和、win point），slot3..0x137 遍历
//     m_pEquipSlot 非空槽（IsEnchantRevisionChannel 时 fieldd 置 0、写
//     amplify type/value、make3rdChroniclePacket）；
//   AVATAR(2)：space 1，遍历 m_pAvatarSlot[0..0x68]，含剩余天数/jewel socket/
//     expansion info；
//   CREATURE(3)：space 7，遍历 m_pCreatureSlot[0..0xf1]，ITEM_TYPE 5 宠补
//     GetRemainDate。
// 各分支回填 short 计数后 finalize 返回 1。
// ============================================================================
int CInventory::MakeItemList(INVEN_TYPE invenType, void* out) const
{
    if (m_pParent == 0)
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__, 0xa6b, 5);
        trace("inventory::MakeItemList m_pParent is NULL.");
        return 0;
    }

    InterfacePacketBuf* buf = (InterfacePacketBuf*)out;
    buf->clear();
    buf->put_header(0, 0xd);

    if (invenType == INVEN_TYPE_AVATAR)
    {
        buf->put_byte(1);
        int countIdx = buf->get_index();
        int count = 0;
        buf->put_short(0);
        int nowTick = OS_API::GetDateTimeTick();
        for (int slot = 0; slot < 0x69; ++slot)
        {
            Inven_Item* item = &m_pAvatarSlot[slot];
            if (item->m_addInfo == 0)
                continue;
            buf->put_short(slot);
            buf->put_int(item->m_addInfo);
            buf->put_int(m_avatarItemMgr.GetRemainDate(item->m_addInfo2, nowTick));
            buf->put_byte(item->GetItemAttr());
            buf->put_short(item->m_fieldb);
            buf->put_byte(item->m_field0);
            buf->put_int(item->m_fieldd);
            buf->put_byte(0);
            buf->put_short(0);
            ++count;
            buf->put_int(0x1e);
            buf->put_binary((char*)m_avatarItemMgr.getJewelSocketData(
                                item->m_addInfo2), 0x1e);
            char* expansion =
                (char*)m_avatarItemMgr.GetExpansionInfo(item->m_addInfo2);
            buf->put_int(4);
            if (expansion == 0)
            {
                stAvatarExpansionInfo_t expInfo;
                expInfo.init();
                buf->put_binary((char*)&expInfo, 4);
            }
            else
            {
                buf->put_binary(expansion, 4);
            }
            buf->put_byte(0);
            buf->put_packet(*item);
        }
        buf->put_short(countIdx, count);
    }
    else if (invenType == INVEN_TYPE_CREATURE)
    {
        buf->put_byte(7);
        int countIdx = buf->get_index();
        int count = 0;
        buf->put_short(0);
        for (int slot = 0; slot < 0xf2; ++slot)
        {
            Inven_Item* item = &m_pCreatureSlot[slot];
            if (item->m_addInfo == 0)
                continue;
            buf->put_short(slot);
            buf->put_int(item->m_addInfo);
            buf->put_int(item->m_addInfo2);
            buf->put_byte(item->GetItemAttr());
            buf->put_short(item->m_fieldb);
            buf->put_byte(item->m_field0);
            buf->put_int(item->m_fieldd);
            buf->put_byte(0);
            buf->put_short(0);
            if (item->m_field1 ==
                (unsigned char)Inven_Item::ITEM_TYPE_CREATURE_EQUIP)
            {
                int nowTick = OS_API::GetDateTimeTick();
                buf->put_int(m_creatureMgr.GetRemainDate(item->m_addInfo2, nowTick));
            }
            buf->put_byte(0);
            buf->put_packet(*item);
            ++count;
        }
        buf->put_short(countIdx, count);
    }
    else if (invenType == INVEN_TYPE_EQUIP)
    {
        buf->put_byte(0);
        buf->put_short(get_inventory_capacity());
        int countIdx = buf->get_index();
        int count = 3;
        // ORIG 固定前导 short 0x6（MakeItemList.md +0x383 段，语义未证）。
        buf->put_short(6);

        // slot0：金币条目（addInfo=0，addInfo2=m_money）
        buf->put_short(0);
        buf->put_int(0);
        buf->put_int(m_money);
        buf->put_byte(0);
        buf->put_short(0);
        buf->put_byte(0);
        buf->put_int(0);
        buf->put_byte(0);
        buf->put_short(0);
        buf->put_byte(0);
        buf->put_packet(g_emptySlot);

        // slot1：代币条目（addInfo=1，addInfo2=coin+payCoin+eventCoin）
        buf->put_short(1);
        buf->put_int(1);
        buf->put_int(m_coin + m_payCoin + m_eventCoin);
        buf->put_byte(0);
        buf->put_short(0);
        buf->put_byte(0);
        buf->put_int(0);
        buf->put_byte(0);
        buf->put_short(0);
        buf->put_byte(0);
        buf->put_packet(g_emptySlot);

        // slot2：胜利点条目（addInfo=2，addInfo2=GetWinPoint）
        buf->put_short(2);
        buf->put_int(2);
        buf->put_int(((CUserCharacInfo*)m_pParent)->GetWinPoint());
        buf->put_byte(0);
        buf->put_short(0);
        buf->put_byte(0);
        buf->put_int(0);
        buf->put_byte(0);
        buf->put_short(0);
        buf->put_byte(0);
        buf->put_packet(g_emptySlot);

        for (int slot = 3; slot < 0x138; ++slot)
        {
            Inven_Item* item = &m_pEquipSlot[slot];
            if (item->m_addInfo == 0)
                continue;
            buf->put_short(slot);
            buf->put_int(item->m_addInfo);
            buf->put_int(item->m_addInfo2);
            buf->put_byte(GetIntegratedPvPItemAttr(*item, invenType, slot));
            buf->put_short(item->m_fieldb);
            buf->put_byte(item->m_field0);
            GameWorld* world = G_GameWorld();
            if (!world->IsEnchantRevisionChannel())
                buf->put_int(item->m_fieldd);
            else
                buf->put_int(0);
            buf->put_byte((unsigned char)item->m_amp.getAbilityType() & 0xff);
            buf->put_short(item->m_amp.getAbilityValue());
            G_CDataManager()->m_itemUpgrade->make3rdChroniclePacket(
                m_pParent, *item, (PacketGuard*)buf);
            buf->put_packet(*item);
            ++count;
        }
        buf->put_short(countIdx, count);
    }

    buf->finalize(true);
    return 1;
}

// 链接桩：ORIG ChangeEquip 以 4 参自由函数形态调用成员 GetInvenSlot
// （压栈 out,this,1,slot），转发到成员实现。
int GetInvenSlot(Inven_Item* out, CInventory* self, int invenType, int slot)
{
    *out = self->GetInvenSlot(invenType, slot);
    return 0;
}
