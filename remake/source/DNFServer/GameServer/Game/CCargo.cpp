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
// ORIG 0x850b2b4：`call IsOperatorable; xor $0x1; test %al,%al; je +0xaf` ——
// je 落点 +0xaf 为 `mov this+0x4,%edx; imul $0x3d; memcpy(sret, &m_pItems[idx],0x3d)`，
// 即 **IsOperatorable==1（合法索引）→ 返回 m_pItems[idx] 原内容**；
// IsOperatorable==0 → `Inven_Item out; memset(&out,0,0x3d);` 返空 item。
// [b28d 真语义 bug 修正] 旧码分支内容互换（!IsOperatorable 返回原内容），已按 ORIG 翻转。
// [b28d-P59 修复] 否定守卫 + 尾返回成员后 68=68 **AE**（旧码 104 条）。
// 语义与 ORIG 分支一一对应（IsOperatorable==1 返 m_pItems[idx] / ==0 返空 item）。
Inven_Item CCargo::get_cargo_slot(int idx) const
{
    // [b28d-P59] ORIG 块序 = 空 item 分支直落、memcpy 分支在尾（`je +0xaf`）
    // ⇒ 源码用否定守卫 `if (!IsOperatorable(idx)) {...return;}` 再 fall 到返回成员。
    if (!IsOperatorable(idx))
    {
        Inven_Item out;
        memset(&out, 0, sizeof(out));
        return out;
    }
    return m_pItems[idx];
}

// ---- GetCargoData @ 0x850c6e0 ----
int CCargo::GetCargoData(int itemIdx, Inven_Item& out) const
{
    for (int i = 0; i <= 0x97; ++i) {
        if (m_pItems[i].m_addInfo == itemIdx) {
            out = m_pItems[i];   // [b28d-N84] ORIG 逐 dword 拷贝非 memcpy
            return i;
        }
    }
    return -1;
}

 // ---- 插入 ----
int CCargo::insert_item(const Inven_Item& item)
{
    int slot = 0;
    // [b28c-16 P0 清绑] 原 register asm("eax") 绑定违反 P0 全域禁则；callee
    // isEquipableItemType 返回 bool ⇒ P168a① 裸形 `if (!call())` 直产
    // `call; xor $0x1,%eax; test %al,%al; je`，无需绑定。
    if (!item.isEquipableItemType()) {
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

// [b28d-N18b] fresh 复验 90=90：残差 = `cmp mem,reg` 折叠方向（ORIG `cmp %eax,%ecx;
// setl` vs 4.4 `cmp %eax,%edx; jge`，N18b/N7 族）+ 帧 0x10 vs 0x20；语义全等。
int CCargo::_AddStackItem(const Inven_Item& item, int slot)
{
    if (m_pItems[slot].m_addInfo == item.m_addInfo) {
        m_sorted = 0;
        // [b28d-P0] ORIG 无槽往返（add 直入 sub 的寄存器）⇒ 内联实参。
        if (0x7fffffff - item.get_add_info() < m_pItems[slot].m_addInfo2) {
            m_pItems[slot].m_addInfo2 = 0;
        } else {
            // [b28d-P226] ORIG 两 get_add_info 在同一实参表达式内：首值升格 %ebx
            // 跨第二 call 保活（`call; mov %eax,%ebx; call; lea (%ebx,%eax,1),%edx`）
            // ⇒ 禁具名 cur/inc 局部（会槽化）。
            if (checkStackableLimit((unsigned long)m_pItems[slot].m_addInfo,
                                    m_pItems[slot].get_add_info()
                                        + item.get_add_info())) {
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

// [b28d-N18b] fresh 复验 65/64 NEAR：内联实参后仅余 cmp 操作数序（N18b）。
int CCargo::_tryAddStackItem(const Inven_Item& item, int slot) const
{
    if (m_pItems[slot].m_addInfo == item.m_addInfo) {
        if (0x7fffffff - item.get_add_info() < m_pItems[slot].m_addInfo2) {
            return -1;
        }
        // [b28d-P226] 同 _AddStackItem：内联实参保留首值 callee-saved 形。
        if (!checkStackableLimit((unsigned long)m_pItems[slot].m_addInfo,
                                 m_pItems[slot].get_add_info()
                                     + item.get_add_info()))
            return -10;
    } else {
        return -1;
    }
    return slot;
}

// [b28d-N82] fresh 复验 82/81：残差 = 循环体两块物理序（ORIG 先 ret 循环后 slot
// 循环，4.4 相反）+ 局部槽 -0x14/-0x10 平移（N93/N82）；返回码语义全等。
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
    // [b28c-16 P0 清绑] 同 insert_item：bool callee 裸形（P168a①）。
    if (!item.isEquipableItemType()) {
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
// [b28d-N82] fresh 复验 116/122：残差 = 局部槽平移 + 减法装载序（P211/N76）+
// isEquipableItemType 调用点 xor 位置；删除语义全等。
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
    // [b28d-N84] ORIG 无默认 ctor call，直接逐 dword 拷贝 ⇒ 拷贝初始化（隐式拷贝
    // 构造内联），禁「默认构造 + 赋值」两段（多 3 条 ctor call）。
    Inven_Item saved = m_pItems[idx];
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
        // [b28d-N84] 同 _AddItem：ORIG 逐 dword 拷贝（0x0850bb.. 段）非 memcpy。
        m_pItems[idx] = item;
    }
}

// [b28d-P210②] ORIG 0x0850c4ba 位于全部 caller 之后（insert_item/update_item/
// tryInsertItemSpecialSlot 均在前）⇒ 4.4 对「未见定义」的 callee 用保守 outgoing
// 帧（sub $0x18，与 ORIG 一致）；定义前置会使 caller 帧塌缩为 0xc。
void CCargo::_AddItem(const Inven_Item& item, int slot)
{
    // ORIG 为逐 dword 字段拷贝（packed 类对齐=1 路径，16×mov+movzbl 尾字节），
    // 非 memcpy 调用 ⇒ 结构体直接赋值（P54/P83/N84）。
    m_pItems[slot] = item;
    ++m_itemCount;
    m_sorted = 0;
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

bool CCargo::FindItemLock(unsigned char lock, ENUM_ITEMSPACE& space, int& slot) const
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
bool CCargo::UpgradeCargo(int capacity)   // b28d-Main：与 CUserCharacInfo.h:263 声明同步（P292 bool callee）
{
    // [b28c-16] P222 注（两形互试）/同族 isUpgradable 实证：ORIG 第二段
    // `cmp; jl +0x31(升级块在后); fall mov $0; jmp end` ⇒ 源形必须是负极
    // `>= → return 0` + 升级体落尾块；正向 `if (<) {…} return 0` 4.4 反产
    // jge + 升级体 fall-through。
    if (m_capacity == 0 || m_capacity > capacity)
        return 0;
    if (m_capacity >= capacity)
        return 0;
    m_capacity = capacity;
    return 1;
}

// ORIG 0x850c698：同上判定，capacity<=0x98 才可升。
// [b28d-N] 真语义 bug 修正：ORIG 首段 `test %eax,%eax; je +0x18(mov$0)` ⇒
// m_capacity==0 直接 return 0（与 `m_capacity>capacity` 共享 ret0 块）。
// 旧写 `m_capacity != 0 && m_capacity > capacity` 在 capacity∈[1,0x98] 时误返 1；
// 短路形 `==0 || >` 同时复现 ORIG 的 je/fall 共享块（P214①/P59）。
bool CCargo::isUpgradable(int capacity) const   // b28d-Main：与 CUserCharacInfo.h:266 同步（P292 bool callee）
{
    if (m_capacity == 0 || m_capacity > capacity)
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
    // [b28d-P223] ORIG 0x0850bc5a 两 return 0 出口为**尾部独立块**：
    // `je +0xaa`（ta==9999）与 `je +0xa3`（addInfo 相等）各自 `mov $0`，块序
    // +0xa3 在 +0xaa 前 ⇒ 双 goto 尾标签串（平铺 return 会被 4.4 就地物化）。
    if (ta == 9999)
        goto ZERO_AA;
    if (ia->m_addInfo == ib->m_addInfo)
        goto ZERO_A3;
    if ((unsigned int)ia->m_addInfo < (unsigned int)ib->m_addInfo)
        return -1;
    return 1;
ZERO_A3:
    return 0;
ZERO_AA:
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
// [b28d-N82] fresh 复验 114/118：残差 = PacketGuard/InterfacePacketBuf 调用点
// 帧策略差（P210/N82）+ 循环内 GetLock 重读形；包字段序全等。
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

// [b28d-N82] fresh 复验 184/187：残差 = put_* 链的 4.1/4.4 实参装载序与帧差
// （P75/N82）；包字段序/条件分支全等。
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
// [b28d-N82] fresh 复验 190=190：残差 = 4.1/4.4 槽号错位（N82）+ 成员重读形
// （P183）；p167 调用点差集 CLEAR。
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
