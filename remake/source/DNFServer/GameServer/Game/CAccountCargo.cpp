// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CAccountCargo/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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
#include "CAccountCargo/CAccountCargo.inc"
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

#include "CAccountCargo/SetDBData.inc"
                              unsigned int capacity)
{
    m_pUser = user;
    m_capacity = capacity;
    m_money = money;
    m_alter = 0;
    if (items != 0)
        memcpy(m_slots, items, 0xd58);
}

#include "CAccountCargo/Clear.inc"
{
    m_pUser = 0;
    m_capacity = 0;
    m_money = 0;
    m_alter = 0;
    memset(m_slots, 0, 0xd58);
}

#include "CAccountCargo/ResetSlot.inc"
{
    // [b28c-11] AE（i486 旗标下）：P223 尾空标签 goto 形复现 ORIG `jne 尾 nop 岛`
    // + body 尾 jmp；已清历史 `register unsigned char v asm("eax")` 绑定（N96/P0）。
    // ORIG 0x082898c0：call CheckValidSlot; xor $1; test %al; jne <尾 nop 岛>;
    // body(call Inven_Item::reset); jmp end; end-1: nop。P223 尾空标签形复现
    // （禁 register asm 绑定，P0/N96）。
    if (!CheckValidSlot(slot))
        goto L_end;
    m_slots[slot].reset();
L_end:;
}

#include "CAccountCargo/GetCapacity.inc"
{
    return m_capacity;
}

#include "CAccountCargo/SetCapacity.inc"
{
    m_capacity = capacity;
}

#include "CAccountCargo/GetMoney.inc"
{
    return m_money;
}

#include "CAccountCargo/SetStable.inc"
{
    m_alter = 0;
}

#include "CAccountCargo/IsAlter.inc"
{
    return m_alter;
}

// ---- 槽位访问 ----
#include "CAccountCargo/CheckValidSlot.inc"
{
    if (slot < 0 || (int)m_capacity <= slot || 0x37 < slot)
        return false;
    return true;
}

#include "CAccountCargo/GetSlot.inc"
{
    // [b28d-solo-CAccountCargo] ORIG 0x082898f8：`call CheckValidSlot; xor $1;
    // test %al; je <valid 块>` ⇒ 否定 guard 早返，invalid 块 fall-through 在前、
    // valid 块在后（P59/P168a①）。原 if(valid) 正极性令 4.4 产 `test; je <invalid>`
    // （invalid 后置）块序相反。
    if (!CheckValidSlot(slot)) {
        Inven_Item out;
        return out;
    }
    return m_slots[slot];
}

#include "CAccountCargo/GetSlotRef.inc"
{
    // 技巧 31：ORIG 帧 sub $0x18，4.4 裸形只给 0x8 ⇒ 4 个 dead int 撑满 16B
    //（无指令，仅占槽）。
    unsigned int g0, g1, g2, g3;
    (void)g0; (void)g1; (void)g2; (void)g3;
    // P168a ① 裸形（CheckValidSlot 返回 bool）：`if(!call()) return 0;`
    // 4.4 直产 `call; xor$1; test %al,%al; je BODY; mov $0x0,%eax; jmp end; BODY: …`
    // 与 ORIG 逐条同形（探针 /tmp/probeB.cpp ka 实证，无 nop 残差）。
    if (!CheckValidSlot(slot))
        return 0;
    m_alter = 1;
    return &m_slots[slot];
}

// ---- 插入条件 / 插入 / 删除 / 移动 ----
#include "CAccountCargo/CheckInsertCondition.inc"
{
    // [b28d-solo-CAccountCargo 判词·DIFF（N 封顶）] ORIG 0x08289a4a：172 vs 175。
    // 已修真 bug：①0x08289bc6 虚调用 vptr+0x10 = _ZTV5CItem slot 4 = IsAvatarItem
    // （原误写 IsCreatureItem slot 5，P282）；②case 3 为 `m_field0 == 0 → return 0`
    // （ORIG `xor $1; test; je 继续`；原 `!= 1` 误拒 m_field0>=2 的物品）。
    // 已复现范式：isGM/avatar 嵌套否定早返（P59）、期限段1 条件极性
    // `usable==0 && expire!=0`（复现 `jne 段2` / `je false 块`）、GetLock 具名 uchar
    // 缓存（P0/P48）、find_item 实参先物化 %ebx（P24/P52）、expireSec 具名局部。
    // 残差封顶（不可复现）：①ORIG 6 项 switch 跳表（`cmp $5; ja; mov <A>(,%eax,4);
    //   jmp *%eax`）vs 4.4 比较链；补 case 0/4/5 撑宽值域反生 bitmask（P163 反例，
    //   实测已回退）；②虚调用 eax↔edx 寄存器对调（N7，4.4.7 间接 call 目标恒 %eax）；
    //   ③段1 else 的 return 1 未与尾部共享出口（4.4 独立发射，+2 条）。
    // p167 CLEAR（14/14，语义闭合）。
    // [b28d-46 P24/P52] ORIG 先把 item.m_addInfo 物化 %ebx 再 call G_CDataManager，
    // 链式形复现（具名 dm 局部使 receiver 先调用、实参后装载）。
    CItem* ci = G_CDataManager()->find_item(item.m_addInfo);
    if (ci == 0)
        return 0;
    // [b28d-46 P59/P168a] ORIG `call isPackagable; xor $1; test %al; je BODY;
    // mov $0;jmp END; BODY:` ⇒ 早期返回 guard 形（非 if 包裹 + 尾部 return 0）。
    if (!ci->isPackagable())
        return 0;
    {
        // [b28d-46 P0/P48] ORIG 把 GetLock() 存 bool 槽 -0x15（cmpb 判定）后复用；
        // 内联两次调用形在 4.4 多一次 call（无 CSE）⇒ 具名 uchar 局部。
        unsigned char lock = item.m_amp.GetLock();
        if (lock != 0) {
            if (((item_lock::CItemLock*)m_pUser->GetCharacExpandDataR(
                    (ENUM_CHARAC_EXPAND_TYPE)2))->CheckItemLock(lock))
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
        // [b28d-solo-CAccountCargo] ORIG 0x08289b48：switch 值有具名槽 -0x14
        // （P0/P28），跳表覆盖 0..5（P163 补未列 case 0/4/5 撑宽 range）；
        // case 3 判定为 `m_field0 == 0 → return 0`（ORIG `xor $1; test; je 继续`，
        // 原 `!= 1` 会误拒 m_field0>=2 的物品=真语义 bug）。
        // [L8 2026-09-10 判据⑨ 旧判词重核] ORIG 0x08289b56 段为**跳转表**
        // （`mov -0x14(%ebp),%eax; cmp $0x5,%eax; ja +0x13e; mov <A>(,%eax,4),%eax;
        // jmp *%eax`，6 项 0..5）且用**无符号** `ja`。旧判词「补 case 0/4/5 反生
        // bitmask」是在 `int` 表达式下测的（有符号 lowering）；改 **unsigned** 并补
        // [L8 实测] 补 case 0/4/5 反使 4.4 走 **bitmask**（`mov $0x1,%edx; shl %cl,%ebx`
        // 段，189 条 > 原 177）⇒ 已回退补 case，仅保留 **unsigned** 表达式。
        // [2026-09-11 负结论·ORIG 跳表已 dump] 读 ORIG 0x08c14820 处 6 项跳表：
        // [0]=0x08289b88(↓IsTradeRestriction 段，即 default/继续)、[1]=[2]=0x08289b67
        // (return 0)、[3]=0x08289b71(case3 体)、[4]=[5]=0x08289b88 ⇒ ORIG 源码实有
        // 6 个 case（0/4/5 空体）。两形实测均**更差**：
        //   ① 6 个独立 case（0/4/5 `break;`）⇒ 4.4 发 `cmp $0x2; ja` + 双层比较（177 条）；
        //   ② case 1/2 各自 `return 0;` ⇒ 发重复 `cmp $0x1; je` + 比较链（181 条）。
        // ⇒ 4.4 的 `expand_case` 对本值域不选跳表（旧 bitmask 结论同为负），**封顶**。
        unsigned int attachType = (unsigned int)ci->GetAttachType();
        switch (attachType) {
        case 1:
        case 2:
            return 0;
        case 3:
            if (item.m_field0 == 0)
                return 0;
            break;
        }
        if (item.m_upgradeSep.IsTradeRestriction())
            return 0;
        // [b28d-46 判词·真 bug/P282] ORIG 0x08289bc6 虚调用 vptr+0x10 = CItem vtable
        // slot 4 = IsAvatarItem（非 slot 5 IsCreatureItem）；按 _ZTV5CItem 0x08c96da0
        // 逐槽核对（vptr=_ZTV+8，slot12=IsImpossibleUpgrade 与头声明序吻合）。
        // [b28d-solo-CAccountCargo] ORIG 0x08289ba9/0x08289bc1：GM/avatar 为两个
        // 独立早返（各自 `xor $1; test; je <return 1 岛>`，P63/P133），非合并 &&。
        if (!m_pUser->isGMUser()) {
            if (!ci->IsAvatarItem()) {
                // [2026-09-11 语义修正] ORIG 0x08289bde..0x08289c0a 的判据是
                // `!(usable == 0 && expire == 0)`：objdump 形为
                // `call getUsablePeriod; test; jne <TRUE>; call getExpirationDate;
                //  test; je <FALSE>; TRUE: mov $1; jmp <conv>; FALSE: mov $0;
                //  conv: test %al,%al; je <return 1 岛>` —— 即“两者都为 0 才跳过”，
                // 等价 `usable != 0 || expire != 0`（与 Ghidra bVar2 一致）。
                // 原实现写成 `usable == 0 && expire != 0`（`jne`+`je` 均落 FALSE）
                // ⇒ 有使用期限(usable!=0) 的物品被误放行、跳过到期时间检查 = 真语义 bug。
                if (!(ci->getUsablePeriod() == 0 &&
                      ci->getExpirationDate() == 0)) {
                    // ORIG 0x08289c0c..0x08289c36：内层重复同一对调用，极性为
                    // `a == 0 && b == 0`（两跳均 `jne <FALSE>`）→ 命中即 return 0
                    // （外层为真时不可达，ORIG 原样保留该双调用形）。
                    if (ci->getUsablePeriod() == 0 &&
                        ci->getExpirationDate() == 0) {
                        return 0;
                    }
                    // [b28d-46 P0] ORIG 把 imul/add 结果先落 -0x10 槽再 call
                    // getCurSec（具名 expireSec）；endurance 内联 item.m_fieldb。
                    int expireSec =
                        (int)((unsigned int)item.m_fieldb * 0x15180 + 0x44a54a80);
                    int curSec = GlobalData::s_systemTime_.getCurSec();
                    if (expireSec <= curSec)
                        return 0;
                }
            }
        }
        return 1;
    }
}

#include "CAccountCargo/InsertItem.inc"
{
    // [b28d-solo-CAccountCargo 真语义 bug] ORIG 0x08289c82：非装备路径为
    // 「同类累加 / 异类直接结构覆盖」，**不存在 return -1 分支**（原源码多写
    // -1 路径=行为偏差，Ghidra C 与反汇编双向证实）。ret 具名局部 -0xc（P0 有槽
    // 往返），isEquipable 正极性（`test %al; je <非装备块>`，装备块 fall-through）。
    if (!CheckValidSlot(slot))
        return -1;
    int ret = -1;
    if (item.isEquipableItemType()) {
        if (m_slots[slot].m_addInfo == 0) {
            m_slots[slot] = item;
            ret = slot;
        }
    } else {
        if (item.m_addInfo == m_slots[slot].m_addInfo) {
            m_slots[slot].m_addInfo2 = m_slots[slot].m_addInfo2 + item.m_addInfo2;
            ret = slot;
        } else {
            m_slots[slot] = item;
            ret = slot;
        }
    }
    m_alter = 1;
    return ret;
}

#include "CAccountCargo/DeleteItem.inc"
{
    // [b28d-solo-CAccountCargo] ORIG 0x08289e3c：两个独立早返（P133 拆守卫，
    // 各自 `mov $0; jmp` 出口）+ `isEquipableItemType` 正极性（ORIG `test %al;
    // je <非装备>`，无 xor ⇒ 禁 ! 形）。
    if (!CheckValidSlot(slot))
        return 0;
    if (m_slots[slot].m_addInfo == 0 || count < 1)
        return 0;
    if (m_slots[slot].isEquipableItemType()) {
        ResetSlot(slot);
    } else {
        if (m_slots[slot].m_addInfo2 < count)
            return 0;
        if (count < m_slots[slot].m_addInfo2)
            m_slots[slot].m_addInfo2 = m_slots[slot].m_addInfo2 - count;
        else
            ResetSlot(slot);
    }
    m_alter = 1;
    return 1;
}

#include "CAccountCargo/MoveItem.inc"
{
    // [b28d-solo-CAccountCargo] ORIG 0x08289f26：`!a || !b || from == to` 短路链
    // （前两项各 `xor $1; test; jne <fail 岛>`，第三项物化 mov $1/$0 后 `je <body>`；
    // P133/P214①）。原 `bool fail = !(a && b && c)` 生栈槽 bool 物化 + 极性相反。
    if (!CheckValidSlot(from) || !CheckValidSlot(to) || from == to)
        return false;
    Inven_Item tmp;
    tmp = m_slots[from];
    m_slots[from] = m_slots[to];
    m_slots[to] = tmp;
    m_alter = 1;
    return true;
}

// ---- 查询 ----
#include "CAccountCargo/GetEmptySlot.inc"
{
    // [b28d-solo-CAccountCargo 判词·AE] ORIG 0x0828a580：28/28 全等；i486 路由
    // （Main 已落 dnf_game_cxx.sh）消三目 MIN_EXPR cmova，复现 ORIG
    // `cmp $0x38,%eax; jbe; mov $0x38,%eax`（P230）。p167 CLEAR（0 call）。
    int limit = (int)(m_capacity > 0x38 ? 0x38 : m_capacity);
    for (int i = 0; i < limit; ++i) {
        if (m_slots[i].m_addInfo == 0)
            return i;
    }
    return -1;
}

#include "CAccountCargo/CheckSlotEmpty.inc"
{
    // P168a ① 裸形（CheckValidSlot 返回 bool）：ORIG `xor $1; test %al,%al;
    // je BODY; mov $0,%eax; jmp end; BODY: …` —— return 0 块在前、命中路径跳走。
    if (!CheckValidSlot(slot))
        return 0;
    // [L8 2026-09-10 P56/P47] ORIG 0x0828a607 段为**分支形态**（`test %eax,%eax;
    // jne L; mov $0x1,%eax; jmp end; L: mov $0x0,%eax`）；三目 `? 1 : 0` 4.4 恒折
    // `sete %al`（ndiff=5 的唯一残差）⇒ 改写 if-return 两条。
    if (m_slots[slot].m_addInfo == 0)
        return 1;
    return 0;
}

#include "CAccountCargo/GetSpecificItemSlot.inc"
{
    // [b28d-solo-CAccountCargo 判词·AE] ORIG 0x0828a61a：29/29 全等；同 GetEmptySlot
    // 的 i486 路由消 cmova（P230）。p167 CLEAR（0 call）。
    int limit = (int)(m_capacity > 0x38 ? 0x38 : m_capacity);
    for (int i = 0; i < limit; ++i) {
        if (itemIdx == m_slots[i].m_addInfo)
            return i;
    }
    return -1;
}

#include "CAccountCargo/CheckStackLimit.inc"
{
    // [b28d-solo-CAccountCargo] ORIG 0x0828a670：G_CDataManager 结果无槽往返
    // （P0 内联实参，禁 dm 具名局部）；ci 槽 -0x14 → si 槽 -0x10 → sum 槽 -0xc
    // 声明序（P28）；条件为 `limit < sum || sum < 0`（P133 短路链）。
    if (!CheckValidSlot(slot))
        return 0;
    if (m_slots[slot].m_addInfo != itemIdx)
        return 0;
    CItem* ci = G_CDataManager()->find_item(itemIdx);
    if (ci == 0)
        return 0;
    if (!ci->is_stackable())
        return 0;
    CStackableItem* si = (CStackableItem*)ci;
    int sum = m_slots[slot].m_addInfo2 + count;
    if (si->getStackableLimit() < sum || sum < 0)
        return 0;
    return 1;
}

#include "CAccountCargo/GetItemCount.inc"
{
    int count = 0;
    for (int i = 0; (int)m_capacity > i; ++i) {
        if (m_slots[i].m_addInfo != 0)
            ++count;
    }
    return count;
}

// ---- 金钱 ----
// [b28d-solo-CAccountCargo] Main 已落 CUser.h:844 int→bool（P292：ORIG 调用点
// 0x0828a1f0 段 `call; xor $0x1,%eax; test %al,%al; je`）。
#include "CAccountCargo/CheckMoneyLimit.inc"
{
    // [b28c-11] AE（i486 旗标下）：UpgradeInfo 具名视图消裸偏移 add$4 中转 +
    // (unsigned int) 零扩展消符号扩展段（ORIG `mov $0x0,%edx`）。
    UpgradeInfo* info =
        (UpgradeInfo*)G_CDataManager()->m_accountCargo.GetCurrUpgradeInfo(m_capacity);
    if (info == 0)
        return 0;
    // ORIG：64 位槽比较（total 先双清零落槽，比较链 ja/jb/jae）。
    unsigned long long total = 0;
    total = (unsigned long long)(m_money + money);
    if ((unsigned long long)(unsigned int)info->m_field4 < total)
        return 0;
    return 1;
}

#include "CAccountCargo/AddMoney.inc"
{
    if (money != 0)
        m_money = m_money + money;
}

#include "CAccountCargo/SubMoney.inc"
{
    if (money != 0 && money <= m_money)
        m_money = m_money - money;
}

// CUser.h:820 声明已同步为 bool（caller 0x0822508c test %al 直判证据，
// Main 代落头）；定义体返回类型随之改 bool，return 0/1 字面量语义不变。
#include "CAccountCargo/DepositMoney.inc"
{
    // [b28d-solo-CAccountCargo] ORIG 0x0828a12a：无 user/mgr 具名局部——isRestricted
    // 实参 m_pUser 先物化 %ebx 跨 Get() 保活（P52），其余调用点重读 m_pUser（P0）。
    if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
            m_pUser, (RestrictCategory::Enum)1, 0x1a)) {
        m_pUser->SendCmdErrorPacket((ENUM_CMDPACKET)0x134, 0xd1);
        return 0;
    }
    unsigned int err =
        GlobalData::s_pSecuProtectionField->Check(m_pUser, (SECURITY_PROTCTION)2);
    if (err != 0) {
        m_pUser->SendCmdErrorPacket((ENUM_CMDPACKET)0x134, (unsigned char)err);
        return 0;
    }
    if ((int)money > m_pUser->getCurCharacMoney() || (int)money < 0) {
        m_pUser->SendCmdErrorPacket((ENUM_CMDPACKET)0x134, 10);
        return 0;
    }
    if (!CheckMoneyLimit(money)) {
        m_pUser->SendCmdErrorPacket((ENUM_CMDPACKET)0x134, 0x5f);
        return 0;
    }
    // ORIG：money 实参先物化 %ebx 跨 getCurCharacInvenW 保活（P52）；无 inven 槽。
    if (!((CInventory*)m_pUser->getCurCharacInvenW())
             ->use_money(money, (eMoneySubReason)0x28, 1)) {
        m_pUser->SendCmdErrorPacket((ENUM_CMDPACKET)0x134, 10);
        return 0;
    }
    AddMoney(money);
    SendNotifyMoney((ENUM_CMDPACKET)0x134);
    m_alter = 1;
    return 1;
}

#include "CAccountCargo/WithdrawMoney.inc"
{
    // [b28d-solo-CAccountCargo] ORIG 0x0828a2f6：无 user/mgr/inven 具名局部
    // （isRestricted/gain_money 实参先物化 %ebx 跨 receiver call 保活，P52；
    // 其余调用点重读 m_pUser，P0）。
    if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
            m_pUser, (RestrictCategory::Enum)1, 0x1a)) {
        m_pUser->SendCmdErrorPacket((ENUM_CMDPACKET)0x135, 0xd1);
        return 0;
    }
    unsigned int err =
        GlobalData::s_pSecuProtectionField->Check(m_pUser, (SECURITY_PROTCTION)3);
    if (err != 0) {
        m_pUser->SendCmdErrorPacket((ENUM_CMDPACKET)0x135, (unsigned char)err);
        return 0;
    }
    // ORIG 0x0828a3a4：GetMoney() 访问器比较（ORIG 非内联调用）
    if (GetMoney() < money || (int)money < 0) {
        m_pUser->SendCmdErrorPacket((ENUM_CMDPACKET)0x135, 10);
        return 0;
    }
    if (!m_pUser->CheckMoney(money)) {
        m_pUser->SendCmdErrorPacket((ENUM_CMDPACKET)0x134, 0x5e);
        return 0;
    }
    SubMoney(money);
    if (((CInventory*)m_pUser->getCurCharacInvenW())
            ->gain_money(money, (eMoneyAddReason)0x1b, 1, 0) == 0) {
        m_pUser->SendCmdErrorPacket((ENUM_CMDPACKET)0x135, 10);
        return 0;
    }
    SendNotifyMoney((ENUM_CMDPACKET)0x135);
    m_alter = 1;
    return 1;
}

// ---- 发包 ----
#include "CAccountCargo/SendNotifyMoney.inc"
{
    PacketGuard packet;
    // ORIG：无 buf 槽，每调用点重新 lea packet 地址（P0 右值直传）。
    ((InterfacePacketBuf*)&packet)->put_header(1, cmd);
    ((InterfacePacketBuf*)&packet)->put_byte(1);
    ((InterfacePacketBuf*)&packet)->put_int(m_money);
    ((InterfacePacketBuf*)&packet)->finalize(true);
    m_pUser->Send(packet);
}

#include "CAccountCargo/SendItemList.inc"
{
    // [b28d-solo-CAccountCargo] ORIG 0x0828a88a：无 buf/world 具名局部——每调用点
    // 重新 `lea -0x18(%ebp)`，G_GameWorld() 结果 %eax 直通虚调用（P0 右值内联）。
    // 原 buf 局部占 -0x14 槽使帧 0x20→0x30、循环变量 -0xc→-0x10 全帧平移。
    PacketGuard packet;
    ((InterfacePacketBuf*)&packet)->put_header(0, 0xd);
    ((InterfacePacketBuf*)&packet)->put_byte(0xc);
    ((InterfacePacketBuf*)&packet)->put_short(m_capacity);
    ((InterfacePacketBuf*)&packet)->put_int(m_money);
    ((InterfacePacketBuf*)&packet)->put_short(GetItemCount());
    for (int i = 0; (int)m_capacity > i; ++i) {
        if (m_slots[i].m_addInfo != 0) {
            ((InterfacePacketBuf*)&packet)->put_short(i);
            ((InterfacePacketBuf*)&packet)->put_int(m_slots[i].m_addInfo);
            ((InterfacePacketBuf*)&packet)->put_int(m_slots[i].m_addInfo2);
            ((InterfacePacketBuf*)&packet)->put_byte(
                (unsigned char)GetIntegratedPvPItemAttr(m_slots[i]));
            ((InterfacePacketBuf*)&packet)->put_short(m_slots[i].m_fieldb);
            ((InterfacePacketBuf*)&packet)->put_byte(m_slots[i].m_field0);
            if (G_GameWorld()->IsEnchantRevisionChannel())
                ((InterfacePacketBuf*)&packet)->put_int(0);
            else
                ((InterfacePacketBuf*)&packet)->put_int(m_slots[i].m_fieldd);
            ((InterfacePacketBuf*)&packet)->put_byte(m_slots[i].m_amp.getAbilityType() & 0xff);
            ((InterfacePacketBuf*)&packet)->put_short(m_slots[i].m_amp.getAbilityValue() & 0xffff);
            ((InterfacePacketBuf*)&packet)->put_byte(0);
            ((InterfacePacketBuf*)&packet)->put_packet(m_slots[i]);
        }
    }
    ((InterfacePacketBuf*)&packet)->finalize(true);
    return m_pUser->Send(packet);
}

// [b28d-solo-CAccountCargo 判词·AE] ORIG 0x0828ab1c：158/158；空槽/非空块序按 ORIG
//   `test m_addInfo; je <空槽块尾部+0x196>` 反转为 `if (addInfo != 0) {非空} else {空槽}`
//   （P191/P222）。p167 CLEAR（27/27）。
#include "CAccountCargo/MakeItemPacket.inc"
{
    // [b28d-solo-CAccountCargo] ORIG 0x0828ab1c：`test m_addInfo; je <空槽块
    // +0x196 尾部>` ⇒ 非空块 fall-through、空槽块后置 ⇒ 源形 `if (addInfo != 0)
    // {非空} else {空槽}`（P191/P222 块序）。原 `== 0` 正极性令空槽块前置。
    ((InterfacePacketBuf*)packet)->put_short(slot);
    if (m_slots[slot].m_addInfo != 0) {
        ((InterfacePacketBuf*)packet)->put_int(m_slots[slot].m_addInfo);
        ((InterfacePacketBuf*)packet)->put_int(m_slots[slot].m_addInfo2);
        ((InterfacePacketBuf*)packet)->put_byte(
            (int)GetIntegratedPvPItemAttr(m_slots[slot]) & 0xff);
        ((InterfacePacketBuf*)packet)->put_short(m_slots[slot].m_fieldb);
        ((InterfacePacketBuf*)packet)->put_byte(m_slots[slot].m_field0);
        if (G_GameWorld()->IsEnchantRevisionChannel())
            ((InterfacePacketBuf*)packet)->put_int(0);
        else
            ((InterfacePacketBuf*)packet)->put_int(m_slots[slot].m_fieldd);
        ((InterfacePacketBuf*)packet)->put_byte(
            m_slots[slot].m_amp.getAbilityType() & 0xff);
        ((InterfacePacketBuf*)packet)->put_short(
            m_slots[slot].m_amp.getAbilityValue() & 0xffff);
        ((InterfacePacketBuf*)packet)->put_byte(0);
        ((InterfacePacketBuf*)packet)->put_packet(m_slots[slot]);
    } else {
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
    }
}

// ---- 创建 / 升级 / 删除（DB 联动） ----
#include "CAccountCargo/CreateAccountCargo.inc"
{
    // [b28d-solo-CAccountCargo 判词·AE] ORIG 0x0828ad72：217/217；裸偏移
    // `*(short*)((char*)getCurCharacR()+0x27)` 具名化为 `->m_level`（§4.3 清违，
    // 同时消 add-vs-disp 折叠差 N102）＋ `m_fieldC` 左操作数（P66）＋ delete_item
    // 守卫合并 `&&`。p167 CLEAR（30/30）。
    if (user->IsExistAccountCargo()) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x132, 0x14);
        return;
    }
    if (G_CDataManager()->m_accountCargo.m_reqLevel >
        (int)user->getCurCharacR()->m_level) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x132, 0xe);
        return;
    }
    UpgradeInfo* createInfo =
        (UpgradeInfo*)G_CDataManager()->m_accountCargo.GetCreateInfo();
    if (createInfo == 0) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x132, 0x13);
        return;
    }
    if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
            user, (RestrictCategory::Enum)1, 0x18)) {
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
    int slot = ((CInventory*)user->getCurCharacInvenR())->GetInvenData(itemIdx,
                                                                        item);
    if (createInfo->m_field8 != 0 &&
        (slot == -1 || createInfo->m_field8 > (int)item.m_addInfo2)) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x132, 0x16);
        return;
    }
    if (0 < createInfo->m_fieldC &&
        createInfo->m_fieldC > user->getCurCharacMoney()) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x132, 0x16);
        return;
    }
    if (createInfo->m_field8 != 0 &&
        !((CInventory*)user->getCurCharacInvenW())
             ->delete_item((INVEN_TYPE)1, slot, createInfo->m_field8,
                           (eItemDelReason)0x24, 1)) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x132, 0x16);
        return;
    }
    if (0 < createInfo->m_fieldC) {
        if (!((CInventory*)user->getCurCharacInvenW())
                 ->use_money(createInfo->m_fieldC, (eMoneySubReason)0x29, 1)) {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x132, 0x16);
            return;
        }
    }
    SendNotifyRecipe(user, slot, createInfo->m_field8 != 0);
    DB_CreateAccountCargo::makeRequest(user->GetUID(), user->get_acc_id(),
                                       (unsigned int)createInfo->m_field0);
}

#include "CAccountCargo/UpgradeAccountCargo.inc"
{
    // [b28d-solo-CAccountCargo 判词·AE] ORIG 0x0828b0b2：210/210；`upgradeInfo->m_fieldC`
    // 作左操作数（P66 左先物化 %ebx）＋ delete_item 守卫合并 `&&`（4.1 对 bool 左项
    // 的物化岛形）后全等。p167 CLEAR（29/29）。
    if (!user->IsExistAccountCargo()) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x132, 0x15);
        return;
    }
    CAccountCargo* cargo = user->GetAccountCargo();
    UpgradeInfo* upgradeInfo =
        (UpgradeInfo*)G_CDataManager()->m_accountCargo.GetNextUpgradeInfo(
            cargo->GetCapacity());
    if (upgradeInfo == 0) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x133, 0x13);
        return;
    }
    if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
            user, (RestrictCategory::Enum)1, 0x19)) {
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
    int slot = ((CInventory*)user->getCurCharacInvenR())->GetInvenData(itemIdx,
                                                                       item);
    if (upgradeInfo->m_field8 != 0 &&
        (slot == -1 || upgradeInfo->m_field8 > (int)item.m_addInfo2)) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x133, 0x16);
        return;
    }
    if (0 < upgradeInfo->m_fieldC &&
        upgradeInfo->m_fieldC > user->getCurCharacMoney()) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x133, 0x16);
        return;
    }
    if (upgradeInfo->m_field8 != 0 &&
        !((CInventory*)user->getCurCharacInvenW())
             ->delete_item((INVEN_TYPE)1, slot, upgradeInfo->m_field8,
                           (eItemDelReason)0x24, 1)) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x133, 0x16);
        return;
    }
    if (0 < upgradeInfo->m_fieldC) {
        if (!((CInventory*)user->getCurCharacInvenW())
                 ->use_money(upgradeInfo->m_fieldC, (eMoneySubReason)0x29, 1)) {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x133, 0x16);
            return;
        }
    }
    SendNotifyRecipe(user, slot, upgradeInfo->m_field8 != 0);
    DB_UpgradeAccountCargo::makeRequest(user->GetUID(), user->get_acc_id(),
                                        (unsigned int)upgradeInfo->m_field0);
}

#include "CAccountCargo/SendNotifyRecipe.inc"
{
    PacketGuard packet;
    // ORIG：无 buf 槽，每调用点重新 lea packet 地址（P0 右值直传）。
    ((InterfacePacketBuf*)&packet)->put_header(0, 0xe);
    ((InterfacePacketBuf*)&packet)->put_byte(0);
    if (flag) {
        ((InterfacePacketBuf*)&packet)->put_short(2);
        ((CInventory*)user->getCurCharacInvenR())->MakeItemPacket(INVEN_TYPE_EQUIP, slot, packet);
    } else {
        ((InterfacePacketBuf*)&packet)->put_short(1);
    }
    ((CInventory*)user->getCurCharacInvenR())->MakeItemPacket(INVEN_TYPE_EQUIP, 0, packet);
    ((InterfacePacketBuf*)&packet)->finalize(true);
    user->Send(packet);
}

#include "CAccountCargo/DeleteAccountCargo.inc"
{
    // ORIG 形：`if (!IsExistAccountCargo()) return;`（gcc 对 bool 否定发
    // xor $0x1; test; jne end + 尾 jmp/nop 块布局）。ORIG 还在调 makeRequest 前
    // 取一次 GetAccountCargo 结果入栈槽 -0xc 后不再引用（P0：具名局部只写）。
    if (!user->IsExistAccountCargo())
    {
        return;
    }
    CAccountCargo* cargo = user->GetAccountCargo();
    (void)cargo;
    DB_DeleteAccountCargo::makeRequest(user->GetUID(), user->get_acc_id());
    user->DeleteCargo();
}
