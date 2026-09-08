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

// ============================================================================
// 构造 / 析构（ORIG 0x86949da / 0x86949f8，weak）
// ============================================================================
extern unsigned int GetIntegratedPvPItemAttr(const Inven_Item& item);
extern Inven_Item g_emptySlot;
bool checkStackableLimit(unsigned long itemIdx, int count);
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
    m_pItems = memory->m_items4;   // InventoryMemory +0x9d07
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
    if (m_capacity <= 0) return 0;
    m_sorted = 0;
    memcpy(m_pItems, data, 0x2438);
    _GetItemCount();
    return 1;
}

// ---- get_cargo @ 0x850b164 ----
bool CCargo::get_cargo(void* out) const
{
    if (m_capacity <= 0) return 0;
    memcpy(out, m_pItems, 0x2438);
    return 1;
}

// ---- 槽位访问 ----
unsigned int CCargo::GetCapacity() const
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

int CCargo::get_empty_slot() const
{
    // P210 定义顺序定律：本函数定义置于 IsOperatorable 之前 ⇒ 4.4 不按其
    // 帧继承 outgoing，保守大帧 0x28/slot@-0xc 与 ORIG 全等（b26 NL30）。
    int slot = 0;
    for (; slot < 0x98; ++slot) {
        if (m_pItems[slot].m_addInfo == 0)
            break;
    }
    if (IsOperatorable(slot))
        return slot;
    return -1;
}

bool CCargo::IsOperatorable(int idx) const
{
    if (idx < 0 || m_capacity <= idx || idx > 0x97)
        return false;
    return true;
}

Inven_Item* CCargo::GetCargoRef(int idx) const
{
    // ORIG 0x850b38c `sub $0x18`：dead pad 撑帧（技巧31）。
    char framePad[0x10];
    (void)framePad;
    if (!IsOperatorable(idx))
        return 0;
    return &m_pItems[idx];
}

Inven_Item* CCargo::GetCargoRef(int idx)
{
    double framePad[2];              // ORIG sub $0x18：+0x10 帧撑（P190 double 对齐撑帧）
    (void)&framePad[0];
    if (!IsOperatorable(idx))
        return 0;
    return &m_pItems[idx];
}


// ---- get_cargo_slot @ 0x850b2b4（按值返回 Inven_Item） ----
// ORIG 0x850b2b4：call IsOperatorable → xor $0x1 → test → je 0xaf（空 item 分支）。
// 即 IsOperatorable==true（索引合法）→ 返回 memset 全 0 的空 item；
// IsOperatorable==false → memmove 返回 m_pItems[idx] 原样内容。分支与直觉相反，以 ORIG 为准。
Inven_Item CCargo::get_cargo_slot(int idx) const
{
    if (!IsOperatorable(idx)) {
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
    // ORIG 调用点 `call; xor $0x1,%eax; test %al,%al; je`（int callee 的 bool 取反物化，
    // X1 调用点自救形）。
    register unsigned char eq asm("eax") = (unsigned char)item.isEquipableItemType();
    eq ^= 1;
    if (eq) {
        for (; slot < 0x98; ++slot) {
            if (_AddStackItem(item, slot) >= 0)
                return slot;
        }
    }
    slot = 0;
    // ORIG 尾置条件 for（setle 形，与第一循环同构）。
    for (; slot < 0x98; ++slot) {
        if (m_pItems[slot].m_addInfo == 0)
        {
            _AddItem(item, slot);
            return slot;
        }
    }
    return -1;
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
                // P48 去缓存：ORIG +0x0f3 对最后 += 的加数独立重调
                // item.get_add_info（不复用 inc）。
                m_pItems[slot].m_addInfo2 =
                    m_pItems[slot].m_addInfo2 + item.get_add_info();
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
    // ORIG 调用点 xor $0x1 形（int callee 取反，X1 调用点自救）。
    register unsigned char eq asm("eax") = (unsigned char)item.isEquipableItemType();
    eq ^= 1;
    if (eq) {
        // ORIG 块排布：cmp je 正跳到 then 体（物理在后）；声明序决定槽
        // ret1(-0xc) → ret2(-0x10) → i(-0x14)。
        // 4.4 同层变量按声明逆序分配槽（先声明 → 深地址），故逆序声明。
        int i;
        int ret2;
        int ret1;
        if (m_pItems[slot].m_addInfo != item.m_addInfo) {
            for (i = 0; i < 0x98; ++i) {
                ret2 = _tryAddStackItem(item, i);
                if (ret2 >= 0)
                    return i;
                if (ret2 == -10)
                    return -1;
            }
        } else {
            ret1 = _AddStackItem(item, slot);
            if (ret1 >= 0)
                return slot;
            if (ret1 == -10)
                return -1;
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
bool CCargo::delete_item(int idx, int count, eItemDelReason reason)
{
    if (!IsOperatorable(idx))
        return 0;
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
            m_pParent->m_historyLog.ItemDelCargo(
                m_pItems[idx].m_addInfo, count,
                m_pItems[idx].m_addInfo2, reason);
        }
    } else {
        _ResetSlot(idx, reason);
    }
    return 1;
}

// ---- _ResetSlot @ 0x850b1a0 ----
void CCargo::_ResetSlot(int idx, eItemDelReason reason)
{
    Inven_Item saved;
    memcpy(&saved, &m_pItems[idx], sizeof(Inven_Item));
    m_pItems[idx].reset();
    if (saved.isEquipableItemType()) {
        m_pParent->m_historyLog.ItemDelCargo(saved.m_addInfo, 1, 0, reason);
    } else {
        m_pParent->m_historyLog.ItemDelCargo(saved.m_addInfo,
                                             saved.m_addInfo2, 0, reason);
    }
    --m_itemCount;
}

int CCargo::move_item(int from, int to)
{
    if (!(IsOperatorable(from) && IsOperatorable(to)))
        return 0;
    if (m_pItems[from].m_addInfo == 0 && m_pItems[to].m_addInfo == 0)
        return 0;
    Inven_Item tmp;
    tmp = m_pItems[from];
    m_pItems[from] = m_pItems[to];
    m_pItems[to] = tmp;
    m_sorted = 0;
    return 1;
}

void CCargo::update_item(int idx, const Inven_Item& item)
{
    if (!IsOperatorable(idx))
        return;
    if (m_pItems[idx].m_addInfo == 0) {
        _AddItem(item, idx);
    } else {
        memcpy(&m_pItems[idx], &item, sizeof(Inven_Item));
    }
}
// ---- 查询 ----
int CCargo::check_slot_empty(int idx) const
{
    char framePad[0x10];
    (void)framePad;
    if (!IsOperatorable(idx))
        return 0;
    return m_pItems[idx].m_addInfo == 0 ? 1 : 0;
}

int CCargo::check_item_exist(int itemIdx) const
{
    for (int i = 0; i <= 0x97; ++i) {
        if (m_pItems[i].m_addInfo == itemIdx)
            return i;
    }
    return -1;
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
        if (!m_pItems[i].isEquipableItemType()) continue;
        if (m_pItems[i].m_addInfo != itemIdx) continue;
        if (m_pItems[i].m_addInfo2 != slot) continue;
        return 1;
    }
    return 0;
}

int CCargo::FindItemLock(unsigned char lock, ENUM_ITEMSPACE& space, int& slot) const
{
    space = (ENUM_ITEMSPACE)2;
    slot = 0;
    // ORIG 尾置 while（jmp cond 先行，cond 读 *param_3 后 setle/test），
    // 增量以 *param_3 = *param_3 + 1 两步写回（lea 0x1 形）。
    while (slot <= 0x97) {
        if (m_pItems[slot].m_amp.GetLock() == lock)
            return 1;
        slot = slot + 1;
    }
    return 0;
}

// ORIG 0x850c656：m_capacity==0 或 m_capacity>capacity 拒绝；仅 m_capacity<capacity
// 升级（==拒绝）。ORIG 汇编 test/je→mov$0 + Ghidra C 双证，旧注释语义相反。
int CCargo::UpgradeCargo(int capacity)
{
    if (m_capacity == 0 || m_capacity > capacity)
        return 0;
    if (m_capacity < capacity)
    {
        m_capacity = capacity;
        return 1;
    }
    return 0;
}

// ORIG 0x850c698：同上判定，capacity<=0x98 才可升。
int CCargo::isUpgradable(int capacity) const
{
    if (m_capacity != 0 && m_capacity > capacity)
        return 0;
    if (m_capacity >= capacity) return 0;
    if (capacity > 0x98) return 0;
    return 1;
}

int CCargo::_CompareSlot(const void* a, const void* b)
{
    const Inven_Item* ia = (const Inven_Item*)a;
    const Inven_Item* ib = (const Inven_Item*)b;
    // ORIG 反汇编：ta/tb 用 jge/jle（有符号 int）；m_addInfo 用 jae（无符号）。
    int ta = ia->m_addInfo == 0 ? 9999 : (int)ia->m_field1;
    int tb = ib->m_addInfo == 0 ? 9999 : (int)ib->m_field1;
    if (ta < tb)
        return -1;
    if (ta > tb)
        return 1;
    // ORIG 尾块：ta==9999 与 addInfo 相等共享 mov$0 返回块（je 正跳），
    // 4.1 identical-return 合并 ⇒ 源码用嵌套 if 形态（探针 /tmp/p_cs2.cpp）。
    // 残差 N：4.1 尾块串联 jmp 跳板（mov$1→+0xa1→+0xa8→end 两级中继）4.4 直达，
    // 助记符序列 58/60，语义逐分支全等。
    if (ta != 9999) {
        if (ia->m_addInfo != ib->m_addInfo) {
            if ((unsigned int)ia->m_addInfo < (unsigned int)ib->m_addInfo)
                return -1;
            return 1;
        }
    }
    return 0;
}

void CCargo::sort()
{
    if (!m_sorted) {
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
            reinterpret_cast<item_lock::CItemLock*>(expand)->MakeItemLockPacket(
                packet, lock);
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
            G_CDataManager()->m_itemUpgrade->make3rdChroniclePacket(
                m_pParent, m_pItems[i], packet);
            buf->put_packet(m_pItems[i]);
        }
    }
    buf->finalize(true);
    return 1;
}

// b21 verdict=DIFF（N 布局残余）：调用点集与 ORIG 全等（put_short/int×/byte×/GetIntegratedPvPItemAttr/
//   G_GameWorld/IsEnchantRevisionChannel/getAbilityType/getAbilityValue/G_CDataManager/
//   make3rdChroniclePacket/put_packet×2，28=28）；仅余 4.1 把空槽块置函数尾由 `je` 跳入 vs 4.4
//   内联 `jne` 后（块调度）及少量寄存器分配 (190/189)。禁 buf/world 引用局部（P0：ORIG 右值内联）。
//   语义完成，byte 层 N 封顶。
void CCargo::MakeItemPacket(PacketGuard* packet, int slot) const
{
    ((InterfacePacketBuf*)packet)->put_short(slot);
    if (m_pItems[slot].m_addInfo == 0) {
        ((InterfacePacketBuf*)packet)->put_int(-1);
        ((InterfacePacketBuf*)packet)->put_int(0);
        ((InterfacePacketBuf*)packet)->put_byte(0);
        ((InterfacePacketBuf*)packet)->put_short(0);
        ((InterfacePacketBuf*)packet)->put_byte(0);
        ((InterfacePacketBuf*)packet)->put_int(0);
        ((InterfacePacketBuf*)packet)->put_byte(0);
        ((InterfacePacketBuf*)packet)->put_short(0);
        ((InterfacePacketBuf*)packet)->put_byte(0);
        ((InterfacePacketBuf*)packet)->put_packet(g_emptySlot);
    } else {
        ((InterfacePacketBuf*)packet)->put_int(m_pItems[slot].m_addInfo);
        ((InterfacePacketBuf*)packet)->put_int(m_pItems[slot].m_addInfo2);
        ((InterfacePacketBuf*)packet)->put_byte(
            (int)GetIntegratedPvPItemAttr(m_pItems[slot]) & 0xff);
        ((InterfacePacketBuf*)packet)->put_short(m_pItems[slot].m_fieldb);
        ((InterfacePacketBuf*)packet)->put_byte(m_pItems[slot].m_field0);
        if (G_GameWorld()->IsEnchantRevisionChannel())
            ((InterfacePacketBuf*)packet)->put_int(0);
        else
            ((InterfacePacketBuf*)packet)->put_int(m_pItems[slot].m_fieldd);
        ((InterfacePacketBuf*)packet)->put_byte(
            m_pItems[slot].m_amp.getAbilityType() & 0xff);
        ((InterfacePacketBuf*)packet)->put_short(
            m_pItems[slot].m_amp.getAbilityValue() & 0xffff);
        G_CDataManager()->m_itemUpgrade->make3rdChroniclePacket(
            m_pParent, m_pItems[slot], packet);
        ((InterfacePacketBuf*)packet)->put_packet(m_pItems[slot]);
    }
}
