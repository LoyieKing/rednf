// [b28d-solo-CItemAmplifier] verdict=NEAR@4.4.7（139/139）/ **AE@c6446r**
//   （compare_tu 考古的失传老版本口径：identical=92 ae=193 near=2 diff=2；本件与
//   generateAmplifyItem 两件由 NEAR→AE，零回归）。残差仅 N7：4.4.7 间接 call
//   目标恒 %eax 而 ORIG/4.4.6 用 %edx（3 处 GetItemType 虚调 + 1 处
//   isAmplifyPollutionItem）。路由需求已附净效果证据上报 Main。
//   本轮真语义修复：rarity 守卫极性反转（ORIG 0x08234a19 物化 `rarity==0||
//   rarity==1` → return false；旧源码 `!=0 && !=1` 判反）+ flag 分支合并 &&
//   （ORIG bool 物化且 flag==0 也走 getUsableLevel 臂）+ grade/rarity/base 去具名
//   局部（P0）。
bool CItemAmplifier::checkInvestableItem(const CItem* item,
                                         const Inven_Item& inven,
                                         int flag) const
{
    if (item->is_stackable())
    {
        return false;
    }

    // [b28d-34] P282：vptr+0xc 槽 = _ZTV5CItem+0x14 = word[5] = CItem::GetItemType()
    // （CItem.h:45 纯虚；基类 _ZTV5CItem word[5] 是 __cxa_pure_virtual 占位，真身在
    // 派生类槽：_ZTV14CStackableItem word[5]=0x08514a84=CStackableItem::GetItemType）。
    // ORIG 三处 `mov (%eax),%eax; add $0xc; mov (%eax),%edx; call *%edx`
    // （0x082349a5/0x082349bc/0x082349ed）；旧函数指针形把目标缓存进栈槽并多 2 insn。
    // [b28d-solo-CItemAmplifier2] 收口 P282 复核：我方 .o 三处立即数同为 0xc，
    // generateAmplifyItem 一处同为 0x58（=isAmplifyPollutionItem）⇒ 槽号全对齐。
    // [b28d-34] 修后 138/139，残差 N7（间接 call 目标 %eax vs %edx）+ 帧 0x24/0x34。
    if (item->GetItemType() <= 9 || item->GetItemType() > 0x15)
    {
        return false;
    }
    if (item->GetItemType() == 0xb)
    {
        return false;
    }

    // [b28d-solo-CItemAmplifier] 语义反转修复：ORIG 0x08234a19 `test%eax; je
    // 8234a2d`，8234a2d = `mov $0x1` 岛 ⇒ 物化的是 **rarity==0 || rarity==1**
    // （两臂各自重调 get_rarity），命中即 return false；旧源码写成
    // `!=0 && !=1` 恰把可强化稀有度判反（0/1 放行、≥2 拒绝）。
    if (item->get_rarity() == 0 || item->get_rarity() == 1)
    {
        return false;
    }

    CEquipItem* equip = (CEquipItem*)item;
    if (!equip)
    {
        return false;
    }
    if ((int)inven.m_endurance < equip->get_endurance())
    {
        return false;
    }

    // [b28d-solo-CItemAmplifier] ORIG 0x08234980 实证：`cmpl $0,flag; je <bool 物化>`
    // 后 mov$1/mov$0 + test ⇒ 源码是**合并 && 条件**（flag==0 也落 getUsableLevel
    // 臂），旧嵌套 if 形既少一条语义检查又无 bool 物化段（P214①/P133）。
    if (flag != 0 && equip->isExistUpgradableLevelTag())
    {
        if (flag <= 0x36)
        {
            return false;
        }
    }
    else if (item->getUsableLevel() <= 0x36)
    {
        return false;
    }

    // [b28d-solo-CItemAmplifier] P0：ORIG grade 在 %ebx、rarity 在 %edx、base 在
    // %eax（均无槽往返）⇒ 原码无这三个具名局部，表达式内联；仅 diff 占 -0x14。
    // 表址形 `add $0xd4,%edx; mov 0xc(%eax,%edx,4)` = this+0x100(m_upgrade)+0x25c。
    int diff = item->get_grade() -
               m_upgrade.m_bodyView.m_named.m_baseGrade[item->get_rarity()];
    // 钳零保持 std::max 形：ORIG 0x08234b0a 实参序 `lea -0x14(diff)→0x4(%esp);
    // lea -0x10(0)→(%esp)` ⇒ 原码是 std::max(**0, diff**)（arg0 在 (%esp)）。
    return std::max(0, diff) >= m_script.m_field3c;
}