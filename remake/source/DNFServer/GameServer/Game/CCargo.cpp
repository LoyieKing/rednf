// ============================================================================
// df_game_r 还原 —— CCargo（角色仓库，G4 商城/交易域批次）
// 参照 docs/class_func_reports/CCargo.md + ORIG 反汇编。
// 布局（0x14，_Charac_info +0xdaa 内嵌）见 CUserCharacInfo.h 注释；
// 物品缓冲 0x98 槽（0x2438 = 0x98 * 0x3d），由 SetInventoryMemory 绑定。
// 跨类调用以 ORIG 真实符号声明（asm-label extern）。
// ============================================================================

#include <cstdlib>
#include <cstring>

#include "CUserCharacInfo.h"
#include "Inven_Item.h"
#include "InterfacePacketBuf.h"
#include "CDataManager.h"
#include "GameWorld.h"
#include "CInventory.h"

// ---- 跨类符号（ORIG 真实符号，定义在其它 TU） ----
extern "C" void sub_CUserHistoryLog_ItemDelCargo(void* log, int itemIdx,
                                                 int count, int a,
                                                 eItemDelReason reason)
    asm("_ZN15cUserHistoryLog12ItemDelCargoEiii14eItemDelReason");
extern "C" void sub_CItemLock_MakeItemLockPacket(void* self, PacketGuard& packet,
                                                 unsigned char lock)
    asm("_ZNK9item_lock9CItemLock18MakeItemLockPacketER11PacketGuardh");
extern "C" void sub_CItemUpgrade_make3rdChroniclePacket(
    void* upgradeMgr, CUser* user, const Inven_Item& item, PacketGuard* packet)
    asm("_ZN8WongWork12CItemUpgrade22make3rdChroniclePacketEP5CUserRK10Inven_ItemP11PacketGuard");

extern unsigned int GetIntegratedPvPItemAttr(const Inven_Item& item);
extern Inven_Item g_emptySlot;
bool checkStackableLimit(unsigned long itemIdx, int count);

// ============================================================================
// 构造 / 析构（ORIG 0x86949da / 0x86949f8，weak）
// ============================================================================
CCargo::CCargo()
{
    m_pItems = 0;
    reset();
}

CCargo::~CCargo()
{
}

void CCargo::set_parent(CUser* user)
{
    m_pParent = user;
}

void CCargo::SetCapacity(int capacity)
{
    m_capacity = capacity;
}

void CCargo::SetInventoryMemory(InventoryMemory* memory)
{
    m_pItems = (Inven_Item*)((char*)memory + 0x9d07);
}

// ---- reset @ 0x850b0c2 ----
void CCargo::reset()
{
    m_itemCount = 0;
    m_pParent = 0;
    m_capacity = 0;
    m_sorted = 0;
    if (m_pItems != 0)
        memset(m_pItems, 0, 0x2438);
}

// ---- set_cargo @ 0x850b116 ----
bool CCargo::set_cargo(void* data)
{
    if (0 < m_capacity) {
        m_sorted = 0;
        memcpy(m_pItems, data, 0x2438);
        _GetItemCount();
    }
    return 0 < m_capacity;
}

// ---- get_cargo @ 0x850b164 ----
bool CCargo::get_cargo(void* out) const
{
    if (0 < m_capacity) {
        memcpy(out, m_pItems, 0x2438);
    }
    return 0 < m_capacity;
}

// ---- 槽位访问 ----
int CCargo::GetCapacity() const
{
    return m_capacity;
}

int CCargo::GetItemCount() const
{
    return m_itemCount;
}

const Inven_Item* CCargo::get_cargo_start() const
{
    return m_pItems;
}

bool CCargo::IsOperatorable(int idx) const
{
    return idx >= 0 && m_capacity > idx && idx <= 0x97;
}

Inven_Item* CCargo::GetCargoRef(int idx) const
{
    if (IsOperatorable(idx))
        return &m_pItems[idx];
    return 0;
}

Inven_Item* CCargo::GetCargoRef(int idx)
{
    if (IsOperatorable(idx))
        return &m_pItems[idx];
    return 0;
}

// ---- get_cargo_slot @ 0x850b2b4（按值返回 Inven_Item） ----
Inven_Item CCargo::get_cargo_slot(int idx) const
{
    if (IsOperatorable(idx)) {
        Inven_Item out;
        memmove(&out, &m_pItems[idx], 0x3d);
        return out;
    }
    Inven_Item out;
    memset(&out, 0, sizeof(out));
    return out;
}

// ---- GetCargoData @ 0x850c6e0 ----
int CCargo::GetCargoData(int itemIdx, Inven_Item& out) const
{
    for (int i = 0; i <= 0x97; ++i) {
        if (m_pItems[i].m_addInfo == itemIdx) {
            memcpy(&out, &m_pItems[i], sizeof(Inven_Item));
            return i;
        }
    }
    return -1;
}

// ---- 插入 ----
int CCargo::insert_item(const Inven_Item& item)
{
    int slot = 0;
    if (!item.isEquipableItemType()) {
        for (; slot < 0x98; ++slot) {
            int ret = _AddStackItem(item, slot);
            if (ret >= 0)
                return slot;
        }
    }
    slot = 0;
    for (;;) {
        if (slot > 0x97)
            return -1;
        if (m_pItems[slot].m_addInfo == 0)
            break;
        ++slot;
    }
    _AddItem(item, slot);
    return slot;
}

void CCargo::_AddItem(const Inven_Item& item, int slot)
{
    memcpy(&m_pItems[slot], &item, sizeof(Inven_Item));
    ++m_itemCount;
    m_sorted = 0;
}

int CCargo::_AddStackItem(const Inven_Item& item, int slot)
{
    if (m_pItems[slot].m_addInfo == item.m_addInfo) {
        m_sorted = 0;
        int add = item.get_add_info();
        if (0x7fffffff - add < m_pItems[slot].m_addInfo2) {
            m_pItems[slot].m_addInfo2 = 0;
        } else {
            int cur = m_pItems[slot].get_add_info();
            int inc = item.get_add_info();
            if (checkStackableLimit((unsigned long)m_pItems[slot].m_addInfo, cur + inc)) {
                m_pItems[slot].m_addInfo2 = m_pItems[slot].m_addInfo2 + inc;
            } else {
                return -10;
            }
        }
    } else {
        return -1;
    }
    return slot;
}

int CCargo::_tryAddStackItem(const Inven_Item& item, int slot) const
{
    if (m_pItems[slot].m_addInfo == item.m_addInfo) {
        int add = item.get_add_info();
        if (0x7fffffff - add < m_pItems[slot].m_addInfo2) {
            return -1;
        }
        int cur = m_pItems[slot].get_add_info();
        int inc = item.get_add_info();
        if (!checkStackableLimit((unsigned long)m_pItems[slot].m_addInfo, cur + inc))
            return -10;
    } else {
        return -1;
    }
    return slot;
}

int CCargo::tryInsertItemSpecialSlot(const Inven_Item& item, int slot) const
{
    if (!IsOperatorable(slot))
        return -2;
    if (!item.isEquipableItemType()) {
        if (m_pItems[slot].m_addInfo == item.m_addInfo) {
            int ret = _tryAddStackItem(item, slot);
            if (ret >= 0)
                return slot;
            if (ret == -10)
                return -1;
        } else {
            for (int i = 0; i < 0x98; ++i) {
                int ret = _tryAddStackItem(item, i);
                if (ret >= 0)
                    return i;
                if (ret == -10)
                    return -1;
            }
        }
    }
    if (m_pItems[slot].m_addInfo != 0)
        return -1;
    return slot;
}

int CCargo::insert_item_special_slot(Inven_Item& item, int slot)
{
    if (!IsOperatorable(slot))
        return -2;
    if (!item.isEquipableItemType()) {
        if (m_pItems[slot].m_addInfo == item.m_addInfo) {
            int ret = _AddStackItem(item, slot);
            if (ret >= 0)
                return slot;
            if (ret == -10)
                return -1;
        } else {
            for (int i = 0; i < 0x98; ++i) {
                int ret = _tryAddStackItem(item, i);
                if (ret >= 0)
                    return i;
                if (ret == -10)
                    return -1;
            }
        }
    }
    if (m_pItems[slot].m_addInfo == 0) {
        _AddItem(item, slot);
    } else {
        return -1;
    }
    return slot;
}

// ---- 删除 ----
int CCargo::delete_item(int idx, int count, eItemDelReason reason)
{
    if (IsOperatorable(idx)) {
        if (m_pItems[idx].m_addInfo == 0)
            return 0;
        if (count < 1)
            return 0;
        m_sorted = 0;
        if (!m_pItems[idx].isEquipableItemType()) {
            if (m_pItems[idx].m_addInfo2 == count) {
                _ResetSlot(idx, reason);
            } else {
                int cur = m_pItems[idx].m_addInfo2;
                if (cur == count || cur - count < 0)
                    return 0;
                m_pItems[idx].m_addInfo2 = cur - count;
                sub_CUserHistoryLog_ItemDelCargo(
                    (char*)m_pParent + 0x79700, m_pItems[idx].m_addInfo, count,
                    m_pItems[idx].m_addInfo2, reason);
            }
        } else {
            _ResetSlot(idx, reason);
        }
        return 1;
    }
    return 0;
}

// ---- _ResetSlot @ 0x850b1a0 ----
void CCargo::_ResetSlot(int idx, eItemDelReason reason)
{
    Inven_Item saved;
    memcpy(&saved, &m_pItems[idx], sizeof(Inven_Item));
    m_pItems[idx].reset();
    if (saved.isEquipableItemType()) {
        sub_CUserHistoryLog_ItemDelCargo((char*)m_pParent + 0x79700, saved.m_addInfo,
                                         1, 0, reason);
    } else {
        sub_CUserHistoryLog_ItemDelCargo((char*)m_pParent + 0x79700, saved.m_addInfo,
                                         saved.m_addInfo2, 0, reason);
    }
    --m_itemCount;
}

int CCargo::move_item(int from, int to)
{
    if (IsOperatorable(from) && IsOperatorable(to)) {
        if (m_pItems[from].m_addInfo == 0 && m_pItems[to].m_addInfo == 0)
            return 0;
        Inven_Item tmp;
        memcpy(&tmp, &m_pItems[from], sizeof(Inven_Item));
        memcpy(&m_pItems[from], &m_pItems[to], sizeof(Inven_Item));
        memcpy(&m_pItems[to], &tmp, sizeof(Inven_Item));
        m_sorted = 0;
        return 1;
    }
    return 0;
}

void CCargo::update_item(int idx, const Inven_Item& item)
{
    if (IsOperatorable(idx)) {
        if (m_pItems[idx].m_addInfo == 0) {
            _AddItem(item, idx);
        } else {
            memcpy(&m_pItems[idx], &item, sizeof(Inven_Item));
        }
    }
}

// ---- 查询 ----
int CCargo::check_slot_empty(int idx) const
{
    if (IsOperatorable(idx))
        return m_pItems[idx].m_addInfo == 0 ? 1 : 0;
    return 0;
}

int CCargo::check_item_exist(int itemIdx) const
{
    for (int i = 0; i <= 0x97; ++i) {
        if (m_pItems[i].m_addInfo == itemIdx)
            return i;
    }
    return -1;
}

int CCargo::get_empty_slot() const
{
    int slot = 0;
    while (slot < 0x98 && m_pItems[slot].m_addInfo != 0)
        ++slot;
    if (!IsOperatorable(slot))
        return -1;
    return slot;
}

int CCargo::_GetItemCount()
{
    m_itemCount = 0;
    for (int i = 0; i < 0x98; ++i) {
        if (m_pItems[i].m_addInfo != 0)
            ++m_itemCount;
    }
    return m_itemCount;
}

void CCargo::calcCargoItemCount()
{
    _GetItemCount();
}

int CCargo::_IsExistEquipItem(int itemIdx, int slot)
{
    for (int i = 0; i <= 0x97; ++i) {
        if (m_pItems[i].isEquipableItemType() &&
            m_pItems[i].m_addInfo == itemIdx && m_pItems[i].m_addInfo2 == slot)
            return 1;
    }
    return 0;
}

int CCargo::FindItemLock(unsigned char lock, ENUM_ITEMSPACE& space, int& slot) const
{
    space = (ENUM_ITEMSPACE)2;
    slot = 0;
    for (;;) {
        if (slot > 0x97)
            return 0;
        if (m_pItems[slot].m_amp.GetLock() == lock)
            break;
        ++slot;
    }
    return 1;
}

// ---- 扩容 / 排序 ----
int CCargo::UpgradeCargo(int capacity)
{
    if (m_capacity == 0 || capacity < m_capacity)
        return 0;
    if (m_capacity < capacity) {
        m_capacity = capacity;
        return 1;
    }
    return 0;
}

int CCargo::isUpgradable(int capacity) const
{
    if (m_capacity == 0 || capacity < m_capacity)
        return 0;
    if (m_capacity < capacity)
        return capacity < 0x99 ? 1 : 0;
    return 0;
}

int CCargo::_CompareSlot(const void* a, const void* b)
{
    const Inven_Item* ia = (const Inven_Item*)a;
    const Inven_Item* ib = (const Inven_Item*)b;
    unsigned int ta = ia->m_addInfo == 0 ? 9999 : (unsigned int)ia->m_field1;
    unsigned int tb = ib->m_addInfo == 0 ? 9999 : (unsigned int)ib->m_field1;
    if (ta < tb)
        return -1;
    if (tb < ta)
        return 1;
    if (ta == 9999)
        return 0;
    if (ia->m_addInfo == ib->m_addInfo)
        return 0;
    return (unsigned int)ia->m_addInfo < (unsigned int)ib->m_addInfo ? -1 : 1;
}

void CCargo::sort()
{
    if (m_sorted != 1) {
        qsort(m_pItems, 0x98, sizeof(Inven_Item), _CompareSlot);
        m_sorted = 1;
    }
}

// ---- 发包 ----
void CCargo::SendItemLockList() const
{
    if (m_pParent == 0)
        return;
    PacketGuard packet;
    InterfacePacketBuf* buf = (InterfacePacketBuf*)&packet;
    buf->clear();
    buf->put_header(0, 0xfb);
    int countIdx = buf->get_index();
    int count = 0;
    buf->put_short(0);
    for (int i = 0; i < 0x98; ++i) {
        if (m_pItems[i].m_amp.GetLock() != 0) {
            buf->put_byte(2);
            buf->put_short(i);
            unsigned char lock = m_pItems[i].m_amp.GetLock();
            CExpandEquipslot* expand = m_pParent->GetCharacExpandDataR(
                (ENUM_CHARAC_EXPAND_TYPE)2);
            sub_CItemLock_MakeItemLockPacket(expand, packet, lock);
            ++count;
        }
    }
    buf->put_byte(countIdx, count);
    buf->finalize(true);
    m_pParent->Send(packet);
}

int CCargo::MakeItemList(PacketGuard* packet) const
{
    InterfacePacketBuf* buf = (InterfacePacketBuf*)packet;
    buf->put_header(0, 0xd);
    buf->put_byte(2);
    buf->put_short(GetCapacity());
    buf->put_short(GetItemCount());
    for (int i = 0; i < 0x98; ++i) {
        if (m_pItems[i].m_addInfo != 0) {
            buf->put_short(i);
            buf->put_int(m_pItems[i].m_addInfo);
            buf->put_int(m_pItems[i].m_addInfo2);
            buf->put_byte((int)GetIntegratedPvPItemAttr(m_pItems[i]) & 0xff);
            buf->put_short(m_pItems[i].m_fieldb);
            buf->put_byte(m_pItems[i].m_field0);
            GameWorld* world = G_GameWorld();
            if (!world->IsEnchantRevisionChannel())
                buf->put_int(m_pItems[i].m_fieldd);
            else
                buf->put_int(0);
            buf->put_byte(m_pItems[i].m_amp.getAbilityType() & 0xff);
            buf->put_short(m_pItems[i].m_amp.getAbilityValue() & 0xffff);
            sub_CItemUpgrade_make3rdChroniclePacket(
                *(void**)((char*)G_CDataManager() + 0x20), m_pParent, m_pItems[i],
                packet);
            buf->put_packet(m_pItems[i]);
        }
    }
    buf->finalize(true);
    return 1;
}

void CCargo::MakeItemPacket(PacketGuard* packet, int slot) const
{
    InterfacePacketBuf* buf = (InterfacePacketBuf*)packet;
    buf->put_short(slot);
    if (m_pItems[slot].m_addInfo == 0) {
        buf->put_int(-1);
        buf->put_int(0);
        buf->put_byte(0);
        buf->put_short(0);
        buf->put_byte(0);
        buf->put_int(0);
        buf->put_byte(0);
        buf->put_short(0);
        buf->put_byte(0);
        buf->put_packet(g_emptySlot);
    } else {
        buf->put_int(m_pItems[slot].m_addInfo);
        buf->put_int(m_pItems[slot].m_addInfo2);
        buf->put_byte((int)GetIntegratedPvPItemAttr(m_pItems[slot]) & 0xff);
        buf->put_short(m_pItems[slot].m_fieldb);
        buf->put_byte(m_pItems[slot].m_field0);
        GameWorld* world = G_GameWorld();
        if (!world->IsEnchantRevisionChannel())
            buf->put_int(m_pItems[slot].m_fieldd);
        else
            buf->put_int(0);
        buf->put_byte(m_pItems[slot].m_amp.getAbilityType() & 0xff);
        buf->put_short(m_pItems[slot].m_amp.getAbilityValue() & 0xffff);
        sub_CItemUpgrade_make3rdChroniclePacket(
            *(void**)((char*)G_CDataManager() + 0x20), m_pParent, m_pItems[slot],
            packet);
        buf->put_packet(m_pItems[slot]);
    }
}
