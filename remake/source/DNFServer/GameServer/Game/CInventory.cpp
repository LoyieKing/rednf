// df_game_r CInventory 还原（第八批 H3，2026-08-16）。
// 布局/语义以 docs/class_func_reports/CInventory.md 逐条对照（AE 口径）。
// 依赖类（Inven_Item/CItem/CUser/CAvatarItemMgr/CCreatureMgr 等）由
// H1/H2/后续批次提供，本 TU 仅声明引用。

#include "CInventory.h"

#include <stdlib.h>
#include <string.h>

#include "GameTypes.h"
#include "STItemScript.h"

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
}

// ===================== 简单访问器（ORIG weak inline 形态） =====================

// CExpandEquipslot::GetInvenSlot 的 TU 内形态替身（ORIG case-5 按隐藏返回指针
// 调用 _ZNK16CExpandEquipslot12GetInvenSlotE10INVEN_TYPEi；CInventory.h 局部
// 类缺该成员声明，先用静态替身复现调用形态，头文件补齐后可换回成员调用）。
static Inven_Item GetInvenSlotByRefHelper(CExpandEquipslot* self, INVEN_TYPE type,
                                          int slot)
{
    Inven_Item item;
    if (slot < 10 || 0x16 < slot)
    {
        return item;
    }
    if (type == INVEN_TYPE_5)
    {
        item = *(Inven_Item*)((char*)self + 5 + (slot - 10) * 0x3d);
    }
    return item;
}

// CInventory::GetInvenSlot 的 TU 内形态替身（ORIG 隐藏返回指针约定；
// CInventory.h 声明为 int 返回，头文件改为 Inven_Item 后可直接换回成员调用）。
static Inven_Item GetInvenSlotSelfHelper(CInventory* self, int invenType, int slot)
{
    Inven_Item item;
    if (self->CheckValidSlot(invenType, slot) != 1)
    {
        return item;
    }
    switch (invenType)
    {
    case 0:
        item = self->m_invenItem[slot];
        break;
    case 1:
        item = self->m_pEquipSlot[slot];
        break;
    case 2:
        item = self->m_pAvatarSlot[slot];
        break;
    case 3:
        item = self->m_pCreatureSlot[slot];
        break;
    case 5:
        item = GetInvenSlotByRefHelper(
            self->m_pParent->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)9),
            (INVEN_TYPE)5, slot);
        break;
    default:
        break;
    }
    return item;
}

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

// ===================== 构造 / 析构 / 复制 =====================

CInventory::CInventory()
{
    reset();
}

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
    memcpy(m_invenItem, other.m_invenItem, 0x632);
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
            m_pParent->GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)9);
        // ORIG 调 _ZN16CExpandEquipslot12GetInvenRefWE10INVEN_TYPEi（W）；
        // CInventory.h 局部声明缺 W，先用 R（codegen 相同），头文件修复后改回。
        return pExpand->GetInvenRefR(INVEN_TYPE_5, slot);
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
        return (Inven_Item*)((char*)this + 0x27e);
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

int CInventory::GetInvenSlot(int invenType, int slot) const
{
    Inven_Item item;
    if (CheckValidSlot(invenType, slot))
    {
        if (m_pEquipSlot == 0 || m_pAvatarSlot == 0 || m_pCreatureSlot == 0)
        {
            memcpy(&item, &item, sizeof(item));
            return 0;
        }
        switch (invenType)
        {
        case 0:
            memcpy(&item, &m_invenItem[slot], 0x3d);
            break;
        case 1:
            memcpy(&item, &m_pEquipSlot[slot], 0x3d);
            break;
        case 2:
            memcpy(&item, &m_pAvatarSlot[slot], 0x3d);
            break;
        case 3:
            memcpy(&item, &m_pCreatureSlot[slot], 0x3d);
            break;
        default:
            memcpy(&item, &item, sizeof(item));
            break;
        case 5:
            int expand = (int)m_pParent->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)9);
            if (expand == 0)
            {
                memcpy(&item, &item, sizeof(item));
            }
            else
            {
                memcpy(&item,
                       m_pParent->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)9)
                           ->GetInvenRefR((INVEN_TYPE)5, slot),
                       0x3d);
            }
            break;
        }
    }
    return item.m_addInfo;
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
        item = GetInvenSlotByRefHelper(pExpand, (INVEN_TYPE)5, slot);
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
            memcpy(out, (char*)this + 0x27e, 0x2dc);
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
    if (CheckValidSlot(invenType, slot))
    {
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
}

void CInventory::StartEndPos(Inven_Item::ITEM_TYPE type, int& start, int& end) const
{
    if (!g_slotBoundChecker->get_slot_bound(type, m_inventoryCapacity, start, end))
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
        if (itemIdx == m_pEquipSlot[i].m_addInfo)
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
        if (avatarUid == m_pAvatarSlot[i].m_addInfo2)
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
            "int CInventory::GetRemainCapacity(INVEN_TYPE, Inven_Item::ITEM_TYPE) const",
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
                "int CInventory::GetRemainCapacity(INVEN_TYPE, Inven_Item::ITEM_TYPE) const",
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
                              "int CInventory::check_item_exist(int) const", 0x1aa4,
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
                "Inven_Item::ITEM_TYPE CInventory::GetItemType(int) const", 0x13c2,
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
    if (g_slotBoundChecker->get_item_type(slot, m_inventoryCapacity, type))
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

void CInventory::makeCheckSum(int a, int b) const
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
    finishItemChecksum(&sum, m_money, m_coin + m_payCoin + m_eventCoin, a, b);
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
    cMyTrace local_3c("int CInventory::UseCoin(eCoinSubReason)", 0x28e, 5);
    cMyTrace local_2c("int CInventory::UseCoin(eCoinSubReason)", 0x29d, 5);
    cMyTrace local_1c("int CInventory::UseCoin(eCoinSubReason)", 0x2ad, 5);

    if (m_coin == 0)
    {
        if (m_eventCoin == 0)
        {
            if (m_payCoin == 0)
            {
                m_coin = 0;
                m_payCoin = 0;
                m_eventCoin = 0;
            }
            else
            {
                m_payCoin--;
                if (m_pParent == 0)
                {
                    local_1c("History ERROR, m_pParent NULL, PayCoinSub");
                }
                else
                {
                    ((cUserHistoryLog*)((char*)m_pParent + 0x79700))->PayCoinSub(m_payCoin, 1, reason);
                }
            }
        }
        else
        {
            m_eventCoin--;
            if (m_pParent == 0)
            {
                local_2c("History ERROR, m_pParent NULL, EventCoinSub");
            }
            else
            {
                ((cUserHistoryLog*)((char*)m_pParent + 0x79700))->EventCoinSub(m_eventCoin, 1, reason);
            }
        }
    }
    else
    {
        m_coin--;
        if (m_pParent == 0)
        {
            local_3c("History ERROR, m_pParent NULL, CoinSub");
        }
        else
        {
            ((cUserHistoryLog*)((char*)m_pParent + 0x79700))->CoinSub(m_coin, 1, reason);
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
        cMyTrace local_3c("int CInventory::gain_money(int, eMoneyAddReason, bool, int)", 0xef1, 4);
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
        ((cUserHistoryLog*)((char*)m_pParent + 0x79700))->MoneyAdd(m_money, amount, reason);
    }
    if (m_pParent != 0)
    {
        if (reason == 4 || reason == 0x1a)
        {
            write_log_gain_money(reason, m_money - oldMoney - param, (unsigned int)m_pParent);
        }
        else
        {
            write_log_gain_money(reason, m_money - oldMoney, (unsigned int)m_pParent);
        }
        if (m_money - oldMoney > 0)
        {
            m_pParent->GetGoldControl()->AddGold(m_money - oldMoney, reason);
        }
    }
    return m_money - oldMoney;
}

int CInventory::use_money(int amount, eMoneySubReason reason, bool bLog)
{
    cMyTrace local_34("bool CInventory::use_money(int, eMoneySubReason, bool)", 0xf30, 4);
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
        write_log_use_money(reason, oldMoney - m_money, (unsigned int)m_pParent);
        if (oldMoney != m_money && oldMoney - m_money > 0)
        {
            m_pParent->GetGoldControl()->SubGold(oldMoney - m_money, reason);
        }
    }
    if (bLog && m_pParent != 0 && amount > 0)
    {
        ((cUserHistoryLog*)((char*)m_pParent + 0x79700))->MoneySub(m_money, amount, reason);
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

int CInventory::getEquipWeaponUpgradeSeparate() const
{
    // ORIG 以绝对偏移访问（+0x280 标志、+0x2b1 UpgradeSeparateInfo），逐指令还原。
    if (*(int*)((char*)this + 0x280) == 0)
    {
        return 0;
    }
    return ((UpgradeSeparateInfo*)((char*)this + 0x2b1))->GetUpgradeSeparate();
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
    while (i <= end)
    {
        if (m_pEquipSlot[i].m_addInfo == 0)
        {
            dst = i;
            break;
        }
        i++;
    }
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
    if (slot < 3 || slot > 8)
    {
        if (slot < 9 || slot > 0x38)
        {
            return false;
        }
    }
    return true;
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
                        "int CInventory::insertInvenItemIntoQuickSlot(Inven_Item, eItemAddReason, bool)",
                        0x1528, "History ERROR, m_pParent NULL, ItemAdd ");
                }
                else if (item.m_field1 == 1)
                {
                    ((cUserHistoryLog*)((char*)m_pParent + 0x79700))->ItemAdd(
                        (INVEN_TYPE)1, 1, 1, item, reason);
                }
                else
                {
                    ((cUserHistoryLog*)((char*)m_pParent + 0x79700))->ItemAdd(
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
                        "int CInventory::insertInvenItemIntoCoresspondingSpace(Inven_Item, eItemAddReason, bool)",
                        0x155c, "History ERROR, m_pParent NULL, ItemAdd ");
                }
                else if (item.m_field1 == 1)
                {
                    ((cUserHistoryLog*)((char*)m_pParent + 0x79700))->ItemAdd(
                        (INVEN_TYPE)1, 1, 1, item, reason);
                }
                else
                {
                    ((cUserHistoryLog*)((char*)m_pParent + 0x79700))->ItemAdd(
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
                cMyTrace trace("int CInventory::stackInvenItemIntoQuickSlot(Inven_Item, eItemAddReason, bool)",
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
                        "int CInventory::stackInvenItemIntoQuickSlot(Inven_Item, eItemAddReason, bool)",
                        0x1597, "History ERROR, m_pParent NULL, ItemAdd ");
                }
                else
                {
                    ((cUserHistoryLog*)((char*)m_pParent + 0x79700))->ItemAdd(
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
                cMyTrace trace("int CInventory::stackInvenItemIntoCorrespondingSpace(Inven_Item, eItemAddReason, bool)",
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
                        "int CInventory::stackInvenItemIntoCorrespondingSpace(Inven_Item, eItemAddReason, bool)",
                        0x15d2, "History ERROR, m_pParent NULL, ItemAdd ");
                }
                else
                {
                    ((cUserHistoryLog*)((char*)m_pParent + 0x79700))->ItemAdd(
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
                "int CInventory::insertItemIntoEquipment(Inven_Item, eItemAddReason, bool, bool)",
                0x14f9, "History ERROR, m_pParent NULL, ItemAdd ");
        }
        else
        {
            ((cUserHistoryLog*)((char*)m_pParent + 0x79700))->ItemAdd(
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
        "int CInventory::insertItemIntoCreature(Inven_Item&, eItemAddReason, bool, bool)",
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
            "void CInventory::WriteCreatureLog(INVEN_TYPE, int, int, int, int, eItemAddReason) const",
            0x24ce, "History ERROR, m_pParent NULL, ItemAdd ");
    }
    else
    {
        ((cUserHistoryLog*)((char*)m_pParent + 0x79700))->CreatureItemAdd(
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
            "bool CInventory::CheckExistItemInInventoryOrEquipment(int, int&, bool&) const",
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
            cMyTrace trace("bool CInventory::removeDupliacteEquipItem()", 0x16d2, 5);
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
                "void CInventory::ShowAllExistItemsForErrorCheck() const", 0x1af0,
                "Inventory(%d) : %d\n", i, m_pEquipSlot[i].m_addInfo);
        }
    }
    for (int i = 0; i < 0x1a; i++)
    {
        if (m_invenItem[i].m_addInfo != 0)
        {
            LogManager::logFormat(
                1, "inventory.cpp",
                "void CInventory::ShowAllExistItemsForErrorCheck() const", 0x1af6,
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

// TODO(G1-3 后续)：完整翻译 ORIG 0x84fc6bc（0x1009 字节，按 invenType 分 4 组
// 槽位发包）。当前仅为恢复链接的最小桩（CUser::SendUpdateItemList 引用）。
void CInventory::MakeItemPacket(int invenType, int slot, PacketGuard& packet) const
{
    // ORIG 的 PacketGuard 实际继承 InterfacePacketBuf（调用点按 put_short/put_int
    // 压栈）；CInventory.h 局部声明未建模继承，桩内以布局强转调用。
    InterfacePacketBuf& buf =
        reinterpret_cast<InterfacePacketBuf&>(packet);
    buf.put_short(slot);
    buf.put_int(0);
    buf.put_int(m_money);
    buf.put_byte(0);
    buf.put_short(0);
    buf.put_byte(0);
    buf.put_int(0);
    buf.put_byte(0);
    buf.put_short(0);
    buf.put_byte(0);
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
                    "static void CInventory::RemoveKCItem(Inven_Item*, Inven_Item*)", 0x23fe,
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
                    "static void CInventory::RemoveKCItem(Inven_Item*, Inven_Item*)", 0x2416,
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
                ((cUserHistoryLog*)((char*)m_pParent + 0x79700))->AvatarItemAdd(
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
            ((cUserHistoryLog*)((char*)m_pParent + 0x79700))->AvatarItemAdd(
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
    if (*(int*)((char*)pItem + 0x234) < 10)
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

void CInventory::erase_repeated_item(const std::vector<std::pair<int, int> >& items,
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
                ((CUserCharacInfo*)m_pParent)->useWinPoint(amount, (eWPSubReason)3);
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
                            break;
                        }
                    }
                }
            }
            if (!found)
            {
                LogManager::logFormat(
                    1, "inventory.cpp",
                    "int CInventory::delete_event_items(const std::vector<std::pair<int, int>, std::allocator<std::pair<int, int> > >&, std::vector<int, std::allocator<int> >&, bool&, eMoneySubReason, eItemDelReason, bool)",
                    0x1d3a, "NOT CRITICAL BUT FOR OBSERVING, No Event Item %d FOUND!", itemIdx);
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
            LogManager::logFormat(
                1, "inventory.cpp",
                "int CInventory::insert_item_special_slot(Inven_Item, int, int, eMoneyAddReason, eItemAddReason)",
                0x1742, "[%s][%d]",
                "int CInventory::insert_item_special_slot(Inven_Item, int, int, eMoneyAddReason, eItemAddReason)",
                0x1742);
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
                ((cUserHistoryLog*)((char*)m_pParent + 0x79700))->ItemAdd(
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
                    m_pEquipSlot[i].m_addInfo2 = 0x7fffffff;
                }
                else
                {
                    m_pEquipSlot[i].m_addInfo2 += a;
                }
                if (m_pParent != 0)
                {
                    ((cUserHistoryLog*)((char*)m_pParent + 0x79700))->ItemAdd(
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
                }
                else
                {
                    m_pEquipSlot[i].m_addInfo2 += a;
                }
                if (m_pParent != 0)
                {
                    ((cUserHistoryLog*)((char*)m_pParent + 0x79700))->ItemAdd(
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
                ((cUserHistoryLog*)((char*)m_pParent + 0x79700))->ItemAdd(
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
                if (a - 10 < 0 || a - 10 > 0x68)
                {
                    return;
                }
                // 颜色（AvatarVariation::colorRGB）映射与 SetAvatarColor
                // 依赖后续批次的 CAvatarItemMgr 扩展，本批仅保留清除形态迁移。
                SendAvatarEvent(0x31a, m_pAvatarSlot[a - 10].m_addInfo, c, 0, 0);
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
            1, "inventory.cpp", "void CInventory::SendAvatarEvent(int, int, int, int, int) const",
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
        char buf[0x14];
        memset(buf, 0, sizeof(buf));
        *(int*)(buf + 0) = b;
        *(int*)(buf + 8) = characNo;
        *(int*)(buf + 0x10) = c;
        guard->put_binary(buf, 0x14);
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
        char buf[0x28];
        memset(buf, 0, sizeof(buf));
        *(int*)(buf + 0) = b;
        *(int*)(buf + 4) = c;
        buf[8] = (char)d;
        *(int*)(buf + 0x24) = characNo;
        guard->put_binary(buf, 0x28);
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
                if (*(int*)((char*)this + 0x55c) == itemIdx)
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
        LogManager::logFormat(1, "inventory.cpp", "int CInventory::use_item(int, int, int)",
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
                ((cUserHistoryLog*)((char*)m_pParent + 0x79700))->ItemDel(
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
                    1, "inventory.cpp", "int CInventory::use_item(int, int, int)", 0xfd5,
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
                ((cUserHistoryLog*)((char*)m_pParent + 0x79700))->ItemDel(
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
            ((cUserHistoryLog*)((char*)m_pParent + 0x79700))->ItemDel(
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
                1, "inventory.cpp", "int CInventory::use_item(int, int, int)", 0x1021,
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
            ((cUserHistoryLog*)((char*)m_pParent + 0x79700))->ItemDel(
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
        ((cUserHistoryLog*)((char*)m_pParent + 0x79700))->CreatureItemAdd(
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
            ((cUserHistoryLog*)((char*)m_pParent + 0x79700))->AvatarItemAdd(
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
                cMyTrace trace("int CInventory::SortInvenSlot(Inven_Item::ITEM_TYPE)", 0x194d, 5);
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
        Inven_Item slotItem1 = GetInvenSlotSelfHelper(this, 1, slot);
        if (slotItem1.m_field0 != 0)
        {
            int secuResult = CSecu_ProtectionField::Check(
                GlobalData::s_pSecuProtectionField, m_pParent, (SECURITY_PROTCTION)0x16);
            if (secuResult != 0)
            {
                return secuResult;
            }
        }
        if (m_pParent != 0)
        {
            if (m_pParent->CheckItemLock(invenType, slot) &&
                GetInvenSlotSelfHelper(this, 1, slot).m_field0 != 0)
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
