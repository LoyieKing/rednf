// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CSecu_ProtectionField/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ CSecu_ProtectionField 实现（独立 TU，从 GameStubs.cpp 拆出）。
#include "CInventory.h"
// [Lane37CUser 2026-09-10 解环] CInventory.h 的 `#include "CUser.h"` 已改前向声明
//   （解环）⇒ 本 TU 解引用 CUser 成员，需自行直补完整定义。
#include "CUser.h"

// ORIG T 0x08288074：枚举名 SECURITY_PROTCTION_TYPE（23 字符，c++filt 实测），
// ORIG mangled _Z23ExceptionProtectionTypei23SECURITY_PROTCTION_TYPE_S_S_S_S_S_S_S_。
enum SECURITY_PROTCTION_TYPE
{
    SECURITY_PROTCTION_TYPE_0 = 0
};

// ORIG 0x8288074：对 base 依次检查 9 个类型参数，为 0 则清对应位（bit0..bit8）。
#include "CSecu_ProtectionField/ExceptionProtectionType.inc"
                            SECURITY_PROTCTION_TYPE t1, SECURITY_PROTCTION_TYPE t2,
                            SECURITY_PROTCTION_TYPE t3, SECURITY_PROTCTION_TYPE t4,
                            SECURITY_PROTCTION_TYPE t5, SECURITY_PROTCTION_TYPE t6,
                            SECURITY_PROTCTION_TYPE t7, SECURITY_PROTCTION_TYPE t8)
{
    if (t0 == 0) value &= ~0x01;
    if (t1 == 0) value &= ~0x02;
    if (t2 == 0) value &= ~0x04;
    if (t3 == 0) value &= ~0x08;
    if (t4 == 0) value &= ~0x10;
    if (t5 == 0) value &= ~0x20;
    if (t6 == 0) value &= ~0x40;
    if (t7 == 0) value &= ~0x80;
    if (t8 == 0) value &= ~0x100;
    return value;
}

#include "CSecu_ProtectionField/Check.inc"
{
    // ORIG T 0x08288a02（报告 Check.md）
    if (user == 0)
        return 1;

    unsigned int protType = GetProtectionType(type);
    if (protType == 0)
        return 0;

    int ret = 0;
    ret = CheckRestoreType(user, protType);
    if (ret != 0)
        return ret;
    ret = CheckTradingType(user, protType, type);
    if (ret != 0)
        return ret;
    ret = CheckEtcType(user, type);
    if (ret != 0)
        return ret;
    return 0;
}

// ORIG T 0x08288140（报告 GetOppositeErr.md）：switch 树状展开。
#include "CSecu_ProtectionField/GetOppositeErr.inc"
{
    switch (err)
    {
    case 0x7a: return 0xee;
    case 0x88: return 0xec;
    case 0x7b: return 0x7c;
    case 0x89: return 0x8a;
    case 0x72:
    case 0xe4:
    case 0xe5: return 0x73;
    default: return 1;
    }
}


#include "CSecu_ProtectionField/GetProtectionType.inc"
{
    // ORIG T 0x082880dc（报告 GetProtectionType.md）：map 查找，?: 布局。
    std::map<SECURITY_PROTCTION, unsigned int>::iterator it =
        m_typeMap.find(type);
    return it != m_typeMap.end() ? it->second : 0u;
}

// ORIG T 0x082881a2（报告 GetCashShopErr.md）：switch 树状展开。
#include "CSecu_ProtectionField/GetCashShopErr.inc"
{
    switch (err)
    {
    case 0x73: return 0x82;
    case 0x7a: return 0x83;
    case 0x7b: return 0x84;
    case 0x88: return 0x85;
    case 0x89: return 0x86;
    case 0x72: return 0x87;
    case 0xe5: return 0x88;
    case 0xe4: return 0x89;
    case 0xcf: return 0x8a;
    case 0x78: return 0x8b;
    default: return 1;
    }
}
#include "CSecu_ProtectionField/init.inc"
{
    // b28d-L3: DIFF/N18b — ORIG 0x082882b9 段 `mov -0x28(%ebp),%edx; mov -0x24,%eax; mov -0x14c(%eax,4),%eax; cmp %eax,%edx`（左操作数先物化）
    // ORIG T 0x0828824e（报告 init.md）：表为局部数组（GCC 生成 .rodata 模板 +
    // rep movsl 拷贝），长度走局部变量；第二轮前整表重置回 0x1ff（第一轮结果
    // 被覆盖，ORIG 原样保留）；插入使用 make_pair(enum, unsigned int& ) 实例。
    // [b28c-30] N18b 封顶：内层比较 ORIG 为 `mov i,-0x28,%edx; mov j,%eax;
    // mov tableA[j],%eax; cmp %eax,%edx`（左操作数先物化寄存器）；4.4 恒
    // `mov tableA[j],%eax; cmp i_mem,%eax`（mem 直比）。探针 /tmp/l30_probe3.cpp
    // 5 形（i==t[j]/t[j]==i/unsigned 双 cast/int cast/(t[j]==unsigned i)）全折
    // mem 直比；余差 2 条 mov（150 vs 152）。
    // b28d-L3 实测：i 声明 unsigned 使循环尾测 setle→setbe（残 6→16，劣化），
    // 回退 int；内层比较 mem 直比差仍为 N18b（见上）。
    int i = 0;
    int j = 0;
    int lenA;
    int lenB;

    unsigned int values[0x47];
    for (i = 0; i < 0x47; ++i)
        values[i] = 0x1ff;

    int tableA[37] = {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x0c, 0x0d, 0x0e, 0x11, 0x12,
        0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d,
        0x22, 0x24, 0x1f, 0x27, 0x25, 0x26, 0x2c, 0x2d, 0x2f, 0x37,
        0x41, 0x42, 0x43, 0x3a, 0x3b, 0x20, 0x0b };
    lenA = 37;
    for (i = 0; i < 0x47; ++i)
    {
        for (j = 0; j < lenA; ++j)
        {
            // [b28c-30] ORIG 0x082882b9 段：mov -0x28(%ebp),%edx（i 先载寄存器）
            // + cmp %eax,%edx ⇒ 原码左操作数是数组元素（tableA[j] == i）。
            if (tableA[j] == i)
                values[i] = ExceptionProtectionType(values[i], (SECURITY_PROTCTION_TYPE)0,
                    (SECURITY_PROTCTION_TYPE)0, (SECURITY_PROTCTION_TYPE)0,
                    (SECURITY_PROTCTION_TYPE)0, (SECURITY_PROTCTION_TYPE)0x10,
                    (SECURITY_PROTCTION_TYPE)0, (SECURITY_PROTCTION_TYPE)0,
                    (SECURITY_PROTCTION_TYPE)0x80, (SECURITY_PROTCTION_TYPE)0);
        }
    }

    // 第二轮（ORIG 在此把整表重置回 0x1ff，忠实保留）
    for (i = 0; i < 0x47; ++i)
        values[i] = 0x1ff;

    int tableB[29] = {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x0c, 0x0d, 0x0e, 0x11, 0x14,
        0x16, 0x17, 0x18, 0x1d, 0x22, 0x24, 0x2c, 0x2d, 0x1f, 0x20,
        0x2f, 0x41, 0x42, 0x43, 0x38, 0x39, 0x3a, 0x3b, 0x0b };
    lenB = 29;
    for (i = 0; i < 0x47; ++i)
    {
        for (j = 0; j < lenB; ++j)
        {
            if (tableB[j] == i)   // [b28c-30] 同 tableA：ORIG 寄存器比较方向
                values[i] = ExceptionProtectionType(values[i], (SECURITY_PROTCTION_TYPE)1,
                    (SECURITY_PROTCTION_TYPE)2, (SECURITY_PROTCTION_TYPE)4,
                    (SECURITY_PROTCTION_TYPE)8, (SECURITY_PROTCTION_TYPE)0x10,
                    (SECURITY_PROTCTION_TYPE)0x20, (SECURITY_PROTCTION_TYPE)0x40,
                    (SECURITY_PROTCTION_TYPE)0, (SECURITY_PROTCTION_TYPE)0x100);
        }
    }

    for (i = 0; i < 0x47; ++i)
        m_typeMap.insert(std::make_pair((SECURITY_PROTCTION)i, values[i]));
}

#include "CSecu_ProtectionField/CSecu_ProtectionField.inc"
{
    init();   // ORIG 0x82aa6d4：map 默认构造后调用 init()
}

// ORIG 0x0828873a（_ZN21CSecu_ProtectionField9CheckItemEP5CUser18SECURITY_PROTCTIONm）：
// 物品 index 命中受保护集合（0x1f70/0x1f71/0x989a13/0x292090e1）时委托 Check(user,type)，
// 否则直接 0。
#include "CSecu_ProtectionField/CheckItem.inc"
                                     unsigned long itemIndex)
{
    if (itemIndex == 0x1f70 || itemIndex == 0x1f71 ||
        itemIndex == 0x989a13 || itemIndex == 0x292090e1)
    {
        return Check(user, type);
    }
    return 0;
}

// ORIG 0x08288786（_ZN21CSecu_ProtectionField9CheckItemEP5CUser18SECURITY_PROTECTIONPK5CItem）：
// item/user 任一为空则 1，否则取 item->get_index() 走上面的重载。
#include "CSecu_ProtectionField/CheckItem_1.inc"
                                     const CItem* item)
{
    if (item == 0 || user == 0)
        return 1;
    unsigned long itemIndex = item->get_index();
    return CheckItem(user, type, itemIndex);
}

// ORIG T 0x08288bf2（报告 CheckRestoreType.md）：恢复惩罚位检查。
#include "CSecu_ProtectionField/CheckRestoreType.inc"
{
    if (user == 0)
        return 1;
    if ((protType & 0x80) != 0 && user->isTradePunishType(0x80))
        return 0xe5;
    return 0;
}

// ORIG T 0x08288c46（报告 CheckTradingType.md）：交易惩罚位逐位检查。
#include "CSecu_ProtectionField/CheckTradingType.inc"
                                             SECURITY_PROTCTION type)
{
    if (user == 0)
        return 1;
    if ((protType & 0x01) != 0 && user->isTradePunishType(0x01))
        return 0x72;
    if ((protType & 0x02) != 0 && user->isTradePunishType(0x02))
        return 0x72;
    if ((protType & 0x04) != 0 && user->isTradePunishType(0x04))
        return 0x72;
    if ((protType & 0x08) != 0 && user->isTradePunishType(0x08))
        return 0x72;
    if ((protType & 0x20) != 0 && user->isTradePunishType(0x20))
        return 0xe4;
    if ((protType & 0x40) != 0 && user->isTradePunishType(0x40))
        return 0x72;
    return 0;
}

// ORIG T 0x08288dbc（报告 CheckEtcType.md）：type∈{8,9} 且安全服务未启用 → 0xe9。
#include "CSecu_ProtectionField/CheckEtcType.inc"
{
    if (user == 0)
        return 1;
    if (user->isRestingUser())
        return 0xcf;
    if ((type == (SECURITY_PROTCTION)8 || type == (SECURITY_PROTCTION)9) &&
        !user->isSecurityServiceUsed())
        return 0xe9;
    return 0;
}

// ===========================================================================
// LinkMisc5 收口（ORIG 反汇编/报告逐条转写）
// ===========================================================================

// ORIG T 0x082886ae _ZN21CSecu_ProtectionField15GetInvenItemPtrEP5CUsercs：
// space==2 → 仓库槽引用；space∈{0,3,7} → 背包槽引用；其余返回空。
#include "CSecu_ProtectionField/GetInvenItemPtr.inc"
                                                   short slot)
{
    // ORIG T 0x082886ae（报告 GetInvenItemPtr.md）：switch 树状展开。
    switch (space)
    {
    case 0:
    case 3:
    case 7:
        return (Inven_Item*)user->getCurCharacInvenR()->GetInvenRef(
            GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)(int)space), (int)slot);
    case 2:
        return user->getCurCharacCargoR()->GetCargoRef((int)slot);
    }
    return 0;
}
// ORIG T 0x082884d6 _ZN21CSecu_ProtectionField10GetItemPtrEP5CUsercs：
// 取槽位物品副本后按 m_addInfo 查模板对象。
#include "CSecu_ProtectionField/GetItemPtr.inc"
{
    // ORIG T 0x082884d6（报告 GetItemPtr.md）：switch 树状展开，probe 整体拷贝。
    Inven_Item probe;
    switch (space)
    {
    case 0:
    case 3:
    case 7:
        probe = user->getCurCharacInvenR()->GetInvenSlot(
            GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)(int)space), (int)slot);
        break;
    case 2:
        probe = user->getCurCharacCargoR()->get_cargo_slot((int)slot);
        break;
    }
    CItem* result = G_CDataManager()->find_item(probe.m_addInfo);
    return result;
}

// ORIG T 0x08288ac8 _ZN21CSecu_ProtectionField16CheckAccountTypeEP5CUserm：
// 密保卡/密保 Pad 认证门 + 交易惩罚位检查。
#include "CSecu_ProtectionField/CheckAccountType.inc"
{
    // ORIG T 0x08288ac8（报告 CheckAccountType.md）：不缓存 card/pad 指针，
    // 每次属性查询均重新取对象。
    if (user == 0)
        return 1;

    if (user->getSecurityCard()->isActivate())
    {
        if ((unsigned int)user->getSecurityCard()->getFailCnt() > 2)
            return 0x7a;
        if (!user->getSecurityCard()->isCertified())
        {
            user->sendBackSecurityAuthRequest();
            return 0x7b;
        }
    }
    else
    {
        if (user->getPad()->isActivate())
        {
            if (user->getPad()->getFailCnt() > 2)
                return 0x88;
            if (!user->getPad()->isCertified())
            {
                user->sendBackSecurityAuthRequest();
                return 0x89;
            }
        }
    }

    if ((protType & 0x10) != 0 && user->isTradePunishType(0x10))
        return 0x72;
    return 0;
}

// ORIG T 0x08288994（报告 CheckOpposite.md）：先做自身 Check，再核对交易对手惩罚态。
#include "CSecu_ProtectionField/CheckOpposite.inc"
                                         CUser* opposite)
{
    if (user == 0 || opposite == 0)
        return 1;
    int result = Check(user, type);
    if (result == 0 && opposite->isTradePunishType(0))
        result = 0x73;
    return result;
}

// ORIG T 0x082887d0 _ZN21CSecu_ProtectionField15CheckItemRarityEP5CUser18SECURITY_PROTCTIONPK5CItemPK10Inven_Item
#include "CSecu_ProtectionField/CheckItemRarity.inc"
                                           const CItem* item,
                                           const Inven_Item* itemRef)
{
    // b28d-L3: DIFF/P209 — ORIG 0x082887d0 对 `mov -0x18(%ebp),%eax; cmp $0x2,%eax; jg`（rarity 局部装寄存器）
    if (item == 0 || user == 0)
        return 1;
    int rarity = 0;
    if (item != 0 && item->get_rarity() > 1)
        rarity = item->get_rarity();

    unsigned int protType = GetProtectionType(type);
    if (protType == 0)
        return 0;
    if (itemRef == 0)
        return 1;

    // ORIG 语义（报告 CheckItemRarity.md）：低风险物品仅做账户检查；
    // 高风险（rarity>=3 / 非揭封强化≥0xb 等）走完整检查链后共享出口。
    int accountErr;
    int result;
    if (rarity < 3 &&
        (item->get_rarity() != 2 || item->IsRandomOption()) &&
        itemRef->GetUpgrade() < 0xb)
    {
        accountErr = CheckAccountType(user, protType);
        if (accountErr != 0)
            return accountErr;
    }
    else
    {
        result = CheckAccountType(user, protType);
        if (result != 0)
            return result;
        if (type == 0x23)
        {
            result = CheckRestoreType(user, protType);
            if (result != 0)
                return result;
            result = CheckTradingType(user, protType, type);
            if (result != 0)
                return result;
        }
        if (type != 0x29)
            return 0;
        result = CheckRestoreType(user, protType);
        if (result != 0)
            return result;
    }
    return 0;
}
