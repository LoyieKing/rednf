// ============================================================================
// df_game_r 还原 —— CAccountCargo（账号金库，G4 商城/交易域批次）
// 参照 docs/class_func_reports/CAccountCargo.md + ORIG 反汇编逐函数核对。
// 布局（0xd68）见 CUser.h CAccountCargo 注释；55 槽 Inven_Item + 金钱/容量。
// 跨类调用以 ORIG 真实符号声明（asm-label extern）。
// ============================================================================

#include <cstring>

#include "CUser.h"
#include "Inven_Item.h"
#include "InterfacePacketBuf.h"
#include "CDataManager.h"
#include "GameWorld.h"
#include "CInventory.h"
#include "CTitleBook.h"
#include "DB_AccountCargoSync.h"
#include "ServiceRestrictManager.h"
#include "Arad_DataManager.h"

extern unsigned int GetIntegratedPvPItemAttr(const Inven_Item& item);
extern Inven_Item g_emptySlot;  // ORIG 全局（0x943ddc0），定义于 Inven_Item.cpp（ORIG Inven_Item TU）

// ============================================================================
// 构造 / 基础存取（ORIG 0x8289794 / 0x8289816 / 0x828986c / 0x82898c0）
// ============================================================================
CAccountCargo::CAccountCargo() : m_pUser(0)
{
    // m_slots[0x38] 为具名 Inven_Item 数组（CUser.h:747），编译器于 ctor 自动逐元素默认构造
    // (ORIG 0x8289797..0x82897ca 的 Inven_ItemC1 循环即编译器自动数组初始化)；此前 for 内
    // 手动 placement-new 二次构造冗余 -> 删除。m_pUser(+0x00) 声明先于 m_slots(+0x04)，
    // 故放入成员初始化列表使 m_pUser=0 于自动数组初始化循环之前执行，匹配 ORIG 指令序。
    m_money = 0;
    m_capacity = 0;
    m_alter = 0;
    memset(m_slots, 0, 0xd58);
}

void CAccountCargo::SetDBData(CUser* user, Inven_Item* items, unsigned int money,
                              unsigned int capacity)
{
    m_pUser = user;
    m_capacity = capacity;
    m_money = money;
    m_alter = 0;
    if (items != 0)
        memcpy(m_slots, items, 0xd58);
}

void CAccountCargo::Clear()
{
    m_pUser = 0;
    m_capacity = 0;
    m_money = 0;
    m_alter = 0;
    memset(m_slots, 0, 0xd58);
}

void CAccountCargo::ResetSlot(int slot)
{
    // X1 形（XOR_FAMILY.tsv，probe 实证）：被调 `CheckValidSlot` 返回 int，
    // 4.4 对 `if (intcall())` 恒产两段 `test %eax,%eax; setne %al; test`；
    // ORIG 是单段 `call; xor $0x1,%eax; test %al,%al; jne end; body`。
    // 绑 %eax 的具名 unsigned char + 显式 `^= 1` + `if (!v)` 可精确复现（形 B）。
    register unsigned char v asm("eax") = (unsigned char)CheckValidSlot(slot);
    v ^= 1;
    if (!v)
        m_slots[slot].reset();
}

int CAccountCargo::GetCapacity()
{
    return m_capacity;
}

void CAccountCargo::SetCapacity(unsigned int capacity)
{
    m_capacity = capacity;
}

unsigned int CAccountCargo::GetMoney()
{
    return m_money;
}

void CAccountCargo::SetStable()
{
    m_alter = 0;
}

char CAccountCargo::IsAlter()
{
    return m_alter;
}

// ---- 槽位访问 ----
int CAccountCargo::CheckValidSlot(int slot) const
{
    if (slot < 0 || (int)m_capacity <= slot || 0x37 < slot)
        return 0;
    return 1;
}

Inven_Item CAccountCargo::GetSlot(int slot)
{
    // ORIG 0x828986c：valid 直接返回槽值（编译器逐字段展开拷贝）；
    // invalid 构造空 Inven_Item（C1 清零）返回。
    if (CheckValidSlot(slot)) {
        return m_slots[slot];
    }
    Inven_Item out;
    return out;
}

Inven_Item* CAccountCargo::GetSlotRef(int slot)
{
    // 技巧 31：ORIG 帧 `sub $0x18,%esp`（4.1 为 call outgoing 区 + 对齐保守分配），
    // 4.4 只给 0x8 ⇒ 用 4 个 dead int 局部精确撑满 16B。
    unsigned int g0, g1, g2, g3;
    register unsigned char v asm("eax") = (unsigned char)CheckValidSlot(slot);
    v ^= 1;
    // X1 + 落点岛（技巧 24）：ORIG `xor $1; test %al,%al; je BODY;
    // mov $0x0,%eax; jmp end; BODY: movb $0x1,0xd64(%eax); …`
    // ⇒ **return 0 走 fall-through，命中路径跳走**。
    if (!v) goto BODY;
    return 0;
BODY:
    (void)g0; (void)g1; (void)g2; (void)g3;
    {
        m_alter = 1;
        return &m_slots[slot];
    }
}

// ---- 插入条件 / 插入 / 删除 / 移动 ----
int CAccountCargo::CheckInsertCondition(Inven_Item& item)
{
    CDataManager* dm = G_CDataManager();
    CItem* ci = dm->find_item(item.m_addInfo);
    if (ci == 0)
        return 0;
    if (ci->isPackagable()) {
        if (item.m_amp.GetLock() != 0) {
            CExpandEquipslot* expand = m_pUser->GetCharacExpandDataR(
                (ENUM_CHARAC_EXPAND_TYPE)2);
            if (!((item_lock::CItemLock*)expand)->CheckItemLock(item.m_amp.GetLock()))
                return 0;
        }
        if (item.m_field1 == 4 || item.m_field1 == 5 || item.m_field1 == 6 ||
            item.m_field1 == 7 || item.m_field1 == 8)
            return 0;
        // ORIG 0x8289a4a +0xe7/+0xed：m_addInfo 落在 [0x1964, 0x1b57](6999) 区间内才
        // 直接返回 0；区间之外继续后续检查（GETATTACH 等）。
        if ((unsigned int)item.m_addInfo >= 0x1964 &&
            (unsigned int)item.m_addInfo <= 0x1b57)
            return 0;
        switch (ci->GetAttachType()) {
        case 1:
        case 2:
            return 0;
        case 3:
            if (item.m_field0 != 1)
                return 0;
            break;
        }
        if (item.m_upgradeSep.IsTradeRestriction())
            return 0;
        if (!m_pUser->isGMUser() && !ci->IsCreatureItem()) {
            // ORIG 0x194..0x1c8 + 0x1c2..0x1ec：两段相同的期限判定。
            // 第一段两字段均为 0 → 无期限放行；否则第二段再判，两字段仍均 0 →
            // （不可达）持久时限检查，否则（有期限物品）返回 0 不允许存入。
            if ((ci->getUsablePeriod() == 0 && ci->getExpirationDate() == 0)) {
                // 无期限物品可存入
            } else {
                if ((ci->getUsablePeriod() == 0 && ci->getExpirationDate() == 0)) {
                    unsigned short endurance = item.m_fieldb;
                    int curSec = GlobalData::s_systemTime_.getCurSec();
                    if ((int)((unsigned int)endurance * 0x15180 + 0x44a54a80) <= curSec)
                        return 0;
                } else {
                    return 0;
                }
            }
        }
        return 1;
    }
    return 0;
}

int CAccountCargo::InsertItem(Inven_Item& item, int slot)
{
    if (!CheckValidSlot(slot))
        return -1;
    if (!item.isEquipableItemType()) {
        if (m_slots[slot].m_addInfo == item.m_addInfo) {
            m_slots[slot].m_addInfo2 = m_slots[slot].m_addInfo2 + item.m_addInfo2;
            m_alter = 1;
            return slot;
        }
        // ORIG 0x08289c82：不同类型且槽非空 → 返回 -1（不覆盖）；仅空槽才拷贝。
        if (m_slots[slot].m_addInfo == 0) {
            memcpy(&m_slots[slot], &item, sizeof(Inven_Item));
            m_alter = 1;
            return slot;
        }
        m_alter = 1;
        return -1;
    }
    if (m_slots[slot].m_addInfo == 0) {
        memcpy(&m_slots[slot], &item, sizeof(Inven_Item));
        m_alter = 1;
        return slot;
    }
    m_alter = 1;
    return -1;
}

int CAccountCargo::DeleteItem(int slot, int count)
{
    if (CheckValidSlot(slot)) {
        if (m_slots[slot].m_addInfo == 0 || count < 1)
            return 0;
        if (!m_slots[slot].isEquipableItemType()) {
            if (m_slots[slot].m_addInfo2 < count)
                return 0;
            if (count < m_slots[slot].m_addInfo2)
                m_slots[slot].m_addInfo2 = m_slots[slot].m_addInfo2 - count;
            else
                ResetSlot(slot);
        } else {
            ResetSlot(slot);
        }
        m_alter = 1;
        return 1;
    }
    return 0;
}

bool CAccountCargo::MoveItem(int from, int to)
{
    bool fail = !(CheckValidSlot(from) && CheckValidSlot(to) && from != to);
    if (!fail) {
        Inven_Item tmp;
        memcpy(&tmp, &m_slots[from], sizeof(Inven_Item));
        memcpy(&m_slots[from], &m_slots[to], sizeof(Inven_Item));
        memcpy(&m_slots[to], &tmp, sizeof(Inven_Item));
        m_alter = 1;
    }
    return !fail;
}

// ---- 查询 ----
int CAccountCargo::GetEmptySlot()
{
    unsigned int limit = m_capacity;
    if (0x38 < limit)
        limit = 0x38;
    for (int i = 0; (int)limit > i; ++i) {
        if (m_slots[i].m_addInfo == 0)
            return i;
    }
    return -1;
}

int CAccountCargo::CheckSlotEmpty(int slot)
{
    register unsigned char v asm("eax") = (unsigned char)CheckValidSlot(slot);
    v ^= 1;
    // 再叠落点岛（技巧 24）：ORIG `xor $1; test %al,%al; je BODY; mov $0,%eax;
    // jmp end; BODY: …` —— **return 0 走 fall-through、命中路径跳走**。
    if (!v) goto BODY;
    return 0;
BODY:
    return m_slots[slot].m_addInfo == 0 ? 1 : 0;
}

int CAccountCargo::GetSpecificItemSlot(int itemIdx)
{
    unsigned int limit = m_capacity;
    if (0x38 < limit)
        limit = 0x38;
    for (int i = 0; (int)limit > i; ++i) {
        if (itemIdx == m_slots[i].m_addInfo)
            return i;
    }
    return -1;
}

int CAccountCargo::CheckStackLimit(int slot, int itemIdx, int count)
{
    if (CheckValidSlot(slot)) {
        if (m_slots[slot].m_addInfo == itemIdx) {
            CDataManager* dm = G_CDataManager();
            CItem* ci = dm->find_item(itemIdx);
            if (ci == 0)
                return 0;
            if (ci->is_stackable()) {
                int cur = m_slots[slot].m_addInfo2;
                int limit = ((CStackableItem*)ci)->getStackableLimit();
                if (limit < cur + count || cur + count < 0)
                    return 0;
                return 1;
            }
            return 0;
        }
        return 0;
    }
    return 0;
}

int CAccountCargo::GetItemCount()
{
    int count = 0;
    for (int i = 0; (int)m_capacity > i; ++i) {
        if (m_slots[i].m_addInfo != 0)
            ++count;
    }
    return count;
}

// ---- 金钱 ----
int CAccountCargo::CheckMoneyLimit(unsigned int money)
{
    void* info = G_CDataManager()->m_accountCargo.GetCurrUpgradeInfo(m_capacity);
    if (info == 0)
        return 0;
    if (*(unsigned int*)((char*)info + 4) < (unsigned int)(m_money + money))
        return 0;
    return 1;
}

void CAccountCargo::AddMoney(unsigned int money)
{
    if (money != 0)
        m_money = m_money + money;
}

void CAccountCargo::SubMoney(unsigned int money)
{
    if (money != 0 && money <= m_money)
        m_money = m_money - money;
}

// CUser.h:820 声明已同步为 bool（caller 0x0822508c test %al 直判证据，
// Main 代落头）；定义体返回类型随之改 bool，return 0/1 字面量语义不变。
bool CAccountCargo::DepositMoney(unsigned int money)
{
    CUser* user = m_pUser;
    ServiceRestrictManager* mgr = ARAD::Singleton<ServiceRestrictManager>::Get();
    if (mgr->isRestricted(user, (RestrictCategory::Enum)1, 0x1a)) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x134, 0xd1);
        return 0;
    }
    unsigned int err = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)2);
    if (err != 0) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x134, (unsigned char)err);
        return 0;
    }
    int curMoney = user->getCurCharacMoney();
    if (curMoney < (int)money || (int)money < 0) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x134, 10);
        return 0;
    }
    if (!CheckMoneyLimit(money)) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x134, 0x5f);
        return 0;
    }
    CInventory* inven = (CInventory*)user->getCurCharacInvenW();
    if (inven->use_money(money, (eMoneySubReason)0x28, 1)) {
        AddMoney(money);
        SendNotifyMoney((ENUM_CMDPACKET)0x134);
        m_alter = 1;
        return 1;
    }
    user->SendCmdErrorPacket((ENUM_CMDPACKET)0x134, 10);
    return 0;
}

bool CAccountCargo::WithdrawMoney(unsigned int money)
{
    CUser* user = m_pUser;
    ServiceRestrictManager* mgr = ARAD::Singleton<ServiceRestrictManager>::Get();
    if (mgr->isRestricted(user, (RestrictCategory::Enum)1, 0x1a)) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x135, 0xd1);
        return 0;
    }
    unsigned int err = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)3);
    if (err != 0) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x135, (unsigned char)err);
        return 0;
    }
    // ORIG 0x0828a3a4：GetMoney() 访问器比较（ORIG 非内联调用）
    if (GetMoney() < money || (int)money < 0) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x135, 10);
        return 0;
    }
    if (!user->CheckMoney(money)) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x134, 0x5e);
        return 0;
    }
    SubMoney(money);
    CInventory* inven = (CInventory*)user->getCurCharacInvenW();
    if (inven->gain_money(money, (eMoneyAddReason)0x1b, 1, 0) == 0) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x135, 10);
        return 0;
    }
    SendNotifyMoney((ENUM_CMDPACKET)0x135);
    m_alter = 1;
    return 1;
}

// ---- 发包 ----
void CAccountCargo::SendNotifyMoney(ENUM_CMDPACKET cmd)
{
    PacketGuard packet;
    InterfacePacketBuf* buf = (InterfacePacketBuf*)&packet;
    buf->put_header(1, cmd);
    buf->put_byte(1);
    buf->put_int(m_money);
    buf->finalize(true);
    m_pUser->Send(packet);
}

int CAccountCargo::SendItemList()
{
    PacketGuard packet;
    InterfacePacketBuf* buf = (InterfacePacketBuf*)&packet;
    buf->put_header(0, 0xd);
    buf->put_byte(0xc);
    buf->put_short(m_capacity);
    buf->put_int(m_money);
    buf->put_short(GetItemCount());
    for (int i = 0; (int)m_capacity > i; ++i) {
        if (m_slots[i].m_addInfo != 0) {
            buf->put_short(i);
            buf->put_int(m_slots[i].m_addInfo);
            buf->put_int(m_slots[i].m_addInfo2);
            buf->put_byte((int)GetIntegratedPvPItemAttr(m_slots[i]) & 0xff);
            buf->put_short(m_slots[i].m_fieldb);
            buf->put_byte(m_slots[i].m_field0);
            GameWorld* world = G_GameWorld();
            if (!world->IsEnchantRevisionChannel())
                buf->put_int(m_slots[i].m_fieldd);
            else
                buf->put_int(0);
            buf->put_byte(m_slots[i].m_amp.getAbilityType() & 0xff);
            buf->put_short(m_slots[i].m_amp.getAbilityValue() & 0xffff);
            buf->put_byte(0);
            buf->put_packet(m_slots[i]);
        }
    }
    buf->finalize(true);
    return m_pUser->Send(packet);
}

void CAccountCargo::MakeItemPacket(PacketGuard* packet, int slot) const
{
    InterfacePacketBuf* buf = (InterfacePacketBuf*)packet;
    buf->put_short(slot);
    if (m_slots[slot].m_addInfo == 0) {
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
        buf->put_int(m_slots[slot].m_addInfo);
        buf->put_int(m_slots[slot].m_addInfo2);
        buf->put_byte((int)GetIntegratedPvPItemAttr(m_slots[slot]) & 0xff);
        buf->put_short(m_slots[slot].m_fieldb);
        buf->put_byte(m_slots[slot].m_field0);
        GameWorld* world = G_GameWorld();
        if (!world->IsEnchantRevisionChannel())
            buf->put_int(m_slots[slot].m_fieldd);
        else
            buf->put_int(0);
        buf->put_byte(m_slots[slot].m_amp.getAbilityType() & 0xff);
        buf->put_short(m_slots[slot].m_amp.getAbilityValue() & 0xffff);
        buf->put_byte(0);
        buf->put_packet(m_slots[slot]);
    }
}

// ---- 创建 / 升级 / 删除（DB 联动） ----
void CAccountCargo::CreateAccountCargo(CUser* user)
{
    if (user->IsExistAccountCargo()) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x132, 0x14);
        return;
    }
    void* script = &G_CDataManager()->m_accountCargo;
    int reqLevel = G_CDataManager()->m_accountCargo.m_reqLevel;
    const char* info = (const char*)user->getCurCharacR();
    if (*(short*)(info + 0x27) < reqLevel) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x132, 0xe);
        return;
    }
    unsigned int* createInfo = (unsigned int*)((AccountCargoScript*)script)->GetCreateInfo();
    if (createInfo == 0) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x132, 0x13);
        return;
    }
    ServiceRestrictManager* mgr = ARAD::Singleton<ServiceRestrictManager>::Get();
    if (mgr->isRestricted(user, (RestrictCategory::Enum)1, 0x18)) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x132, 0xd1);
        return;
    }
    unsigned int err = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)4);
    if (err != 0) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x132, (unsigned char)err);
        return;
    }
    Inven_Item item;
    int itemIdx = G_CDataManager()->m_accountCargo.m_itemIdx;
    CInventory* inven = (CInventory*)user->getCurCharacInvenR();
    int slot = inven->GetInvenData(itemIdx, item);
    if (createInfo[2] != 0 &&
        (slot == -1 || (int)createInfo[2] > item.get_add_info())) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x132, 0x16);
        return;
    }
    if (0 < (int)createInfo[3] &&
        user->getCurCharacMoney() < (int)createInfo[3]) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x132, 0x16);
        return;
    }
    if (createInfo[2] != 0) {
        CInventory* invenW = (CInventory*)user->getCurCharacInvenW();
        if (!invenW->delete_item((INVEN_TYPE)1, slot, createInfo[2],
                                 (eItemDelReason)0x24, 1)) {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x132, 0x16);
            return;
        }
    }
    if (0 < (int)createInfo[3]) {
        CInventory* invenW = (CInventory*)user->getCurCharacInvenW();
        if (!invenW->use_money(createInfo[3], (eMoneySubReason)0x29, 1)) {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x132, 0x16);
            return;
        }
    }
    SendNotifyRecipe(user, slot, createInfo[2] != 0);
    DB_CreateAccountCargo::makeRequest(user->GetUID(), user->get_acc_id(), createInfo[0]);
}

void CAccountCargo::UpgradeAccountCargo(CUser* user)
{
    if (!user->IsExistAccountCargo()) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x132, 0x15);
        return;
    }
    CAccountCargo* cargo = user->GetAccountCargo();
    int capacity = cargo->GetCapacity();
    void* script = &G_CDataManager()->m_accountCargo;
    unsigned int* upgradeInfo =
        (unsigned int*)((AccountCargoScript*)script)->GetNextUpgradeInfo(capacity);
    if (upgradeInfo == 0) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x133, 0x13);
        return;
    }
    ServiceRestrictManager* mgr = ARAD::Singleton<ServiceRestrictManager>::Get();
    if (mgr->isRestricted(user, (RestrictCategory::Enum)1, 0x19)) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x133, 0xd1);
        return;
    }
    unsigned int err = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)5);
    if (err != 0) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x133, (unsigned char)err);
        return;
    }
    Inven_Item item;
    int itemIdx = G_CDataManager()->m_accountCargo.m_itemIdx;
    CInventory* inven = (CInventory*)user->getCurCharacInvenR();
    int slot = inven->GetInvenData(itemIdx, item);
    if (upgradeInfo[2] != 0 &&
        (slot == -1 || (int)upgradeInfo[2] > item.get_add_info())) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x133, 0x16);
        return;
    }
    if (0 < (int)upgradeInfo[3] &&
        user->getCurCharacMoney() < (int)upgradeInfo[3]) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x133, 0x16);
        return;
    }
    if (upgradeInfo[2] != 0) {
        CInventory* invenW = (CInventory*)user->getCurCharacInvenW();
        if (!invenW->delete_item((INVEN_TYPE)1, slot, upgradeInfo[2],
                                 (eItemDelReason)0x24, 1)) {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x133, 0x16);
            return;
        }
    }
    if (0 < (int)upgradeInfo[3]) {
        CInventory* invenW = (CInventory*)user->getCurCharacInvenW();
        if (!invenW->use_money(upgradeInfo[3], (eMoneySubReason)0x29, 1)) {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x133, 0x16);
            return;
        }
    }
    SendNotifyRecipe(user, slot, upgradeInfo[2] != 0);
    DB_UpgradeAccountCargo::makeRequest(user->GetUID(), user->get_acc_id(), upgradeInfo[0]);
}

void CAccountCargo::SendNotifyRecipe(CUser* user, int slot, bool flag)
{
    PacketGuard packet;
    InterfacePacketBuf* buf = (InterfacePacketBuf*)&packet;
    buf->put_header(0, 0xe);
    buf->put_byte(0);
    if (flag) {
        buf->put_short(2);
        ((CInventory*)user->getCurCharacInvenR())->MakeItemPacket(INVEN_TYPE_EQUIP, slot, packet);
    } else {
        buf->put_short(1);
    }
    ((CInventory*)user->getCurCharacInvenR())->MakeItemPacket(INVEN_TYPE_EQUIP, 0, packet);
    buf->finalize(true);
    user->Send(packet);
}

void CAccountCargo::DeleteAccountCargo(CUser* user)
{
    if (user->IsExistAccountCargo()) {
        DB_DeleteAccountCargo::makeRequest(user->GetUID(), user->get_acc_id());
        user->DeleteCargo();
    }
}
