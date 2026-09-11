// ORIG T _ZN15Inter_LoadInven19_CheckOverEquipItemEP5CUser @0x084c4d38
// （报告 docs/class_func_reports/Inter_LoadInven/_CheckOverEquipItem.md 两段全读）
// [b28d-lane2] verdict=DIFF ndiff=216（ORIG 0x084c4d38）：槽序/帧差(N82/P212/P221)+混合残差；gap-13。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=157（ORIG 0x84c4d38）：N18a 单段 regmove+N82/P112 帧槽平移。
void Inter_LoadInven::_CheckOverEquipItem(CUser* user)
{
    // [Q5-InterDisp 2026-09-11 已结案] ORIG 084c4d3f `call <T>; xor $0x1,%eax; test %al,%al;
    //   jne <T>` 的 xor 只在 gcc 已知值域 {0,1} 时发出。三条路径实测：
    //   ① 只把**返回类型** uchar→bool（未同步字段）：调用点 xor 复现 ✓、本函数 gap -1→+0，
    //      但定义侧 `return (unsigned char)CUR->m_overEquipFlag;`（字段当时为 char）
    //      需 uchar→bool 归一化 ⇒ CUserCharacInfo 251/391/1/3 → 251/390/1/4 反劣 ⇒ 回滚。
    //   ② 调用点写 `f() ^ 1`：4.4 发 `cmp $0x1,%al; setne %al; test;jne`（多 1 条）⇒ 回滚。
    //   ③ **字段 `m_overEquipFlag` 与返回类型同改 bool**（Main 授权，写点唯一且只写 0
    //      ⇒ 语义零风险，布局 1 字节不变）：本函数 gap **-1→+0**（221=221），
    //      CUserCharacInfo 复编回 **251/391/1/3**（不劣化）、CUser 809/1719/5/66/586
    //      （不劣化）⇒ **已落地**（CUserCharacInfo.h:462/1079 + .inc:1 + .cpp:1162）。
    //   本函数残差自此仅剩 N82/P112 槽平移族（-0x1c/-0x18 vs -0x20/-0x1c）。
    if (!user->needCurCharacCheckOverEquip())
        return;
    if (user->getCurCharacR() == 0)
        return;

    CInventory* inven = user->getCurCharacInvenW();
    for (int slot = 0; slot <= 0x15; ++slot)
    {
        Inven_Item item = inven->GetInvenSlot(0, slot);
        if (item.m_addInfo == 0)
            continue;
        CItem* ci = G_CDataManager()->find_item(item.m_addInfo);
        if (ci == 0 || ci->is_stackable())
            continue;

        int overLevel = user->GetPremiumInfo()->GetOverEquipableLevel(
            (ENUM_EQUIPMENTTYPE)slot);   // ORIG 传槽号（0..0x15）
        const int maxEquip = user->GetCurCharacMaxEquipLevel();
        const int levelGap = maxEquip - user->get_charac_level();
        overLevel = std::max(overLevel, levelGap);       // ORIG +0x0f5 call std::max<int>(int&,int&)

        // ORIG 084c4e44 mov -0x14(%ebp),%eax; mov %eax,-0x10(%ebp)：static_cast<CEquipItem*>
        //   的副本在 check_low_level 之前落地（GetUsableMaxLevel 用该副本 -0x10，GetAttachType
        //   用 ci -0x14）。
        CEquipItem* cequip = static_cast<CEquipItem*>(ci);
        // P183：ORIG 无 needMove 具名槽（084c4e9c mov $0x1,%eax / 084c4ea3 mov $0x0,%eax）
        //   ⇒ 原码是单个短路 if；ORIG 3 次 get_charac_level（O0 不 CSE）：levelGap +
        //   check_low_level + GetUsableMaxLevel。
        // 语义轴：ORIG 084c4e69 `xor $0x1,%eax; test %al,%al; jne 084c4e9c` ⇒ 原码是
        //   `!check_low_level(...)`（CItem::check_low_level = m_usableLevel <= level，即等级
        //   不够才移走）；旧稿写成正向条件，判定反了。
        if (!ci->check_low_level(user->get_charac_level() + overLevel)
            || (ci->GetAttachType() == 5
                && cequip->GetUsableMaxLevel() < user->get_charac_level()))
        {
            if (ci->IsAvatarItem())                     // vtable+0x10 槽
            {   // 化身位 → 移入 avatar 仓空槽
                int emptySlot = -1;                     // 084c4e8e movl $0xffffffff,-0xc(%ebp)
                // [G18 T14 2026-09-11 · P-G7b 负结论] 改赋值内嵌条件实测**反劣**：
                // our 221→224（+3 超额物化），ORIG 该处 `sete;test;jne` 不可由内嵌复现
                // ⇒ 已还原原形（两句写）。
                emptySlot =
                    inven->get_empty_slot((INVEN_TYPE)2, (Inven_Item::ITEM_TYPE)8);
                if (emptySlot != -1)
                    inven->move_item((INVEN_TYPE)0, slot, (INVEN_TYPE)2, emptySlot);
            }
            else
            {
                inven->ResetSlot(0, slot);              // 仓库重置 → 回背包并邮寄兜底
                // [K3 2026-09-11] P226：ORIG 084c4e 段直接对返回值测高位，无 bool 物化
                //   （旧稿 `(r & 0x80000000) == 0` + `!` 多 5 条）；语义等价（高位非零 ≡ 失败）。
                if (inven->insertItemIntoInventory(item, (eItemAddReason)0xe, false, false)
                    & 0x80000000)
                {
                    WongWork::CMailBoxHelper::ReqDBSendNewMail(
                        user, item, 0,
                        user->getCurCharacNo(), true, 0, 0);
                }
            }
        }
    }
    user->resetCurCharacOverEquipFlag();
}