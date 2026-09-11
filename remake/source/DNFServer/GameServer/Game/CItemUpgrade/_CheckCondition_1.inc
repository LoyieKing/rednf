// ============================================================================
// _CheckCondition(ENCHANT 5 参) @ 0x085497ea（_CheckCondition_085497ea.md）
//   三代强化条件校验。返回错误码，0=通过。
// [b28d-solo-CItemUpgrade 判词] AE（位域视图复现 `and $1; test %al` 与
//   `shr $3,%al`/`shr %al; and $3`；need 改 unsigned 复现 `cmp mem,%eax; jae`
//   单段装载；`ret != 0` 用 P225 while 形复现 setne;test;je 四段物化）。
// ============================================================================
int CItemUpgrade::_CheckCondition(const CUser* user, const Inven_Item& item,
                                  const CItem* itemScript, const Inven_Item& ticket,
                                  unsigned char param)
{
    // ORIG 0x085497ea 逐段转写（_CheckCondition_085497ea.md）。
    if (!item.m_amp.isIdentified())
        return 0xae;
    if (item.m_field1 != 1)
        return 0x13;
    if (itemScript->GetItemType() == 0xb)
        return 0x13;
    // P48 无 CSE：两臂各独立调用 GetItemType。
    if (itemScript->GetItemType() < 10 || itemScript->GetItemType() > 0x15)
        return 0x13;
    if (item.m_fieldb !=
        ((const CEquipItem*)itemScript)->get_endurance())
        return 7;

    // 空间检查：预测分解结果物品数 + 1，增幅再 +1。
    // ORIG 0x08549938 `cmp -0x14(%ebp),%eax; jae`（mem 直比 + 无符号序）⇒ need
    // 是 unsigned 槽（int 槽时 4.4 恒先 `mov %eax,%edx; mov -0x14,%eax` 两段，
    // P209）。
    unsigned int need =
        (unsigned int)G_CDataManager()->m_disJoint->getPredictResultItemTypeCount(
            itemScript->get_rarity());
    ++need;
    if (item.m_amp.hasAbility())
        ++need;
    // [b28d-solo-CItemUpgrade] ORIG 0x08549944 条件直接内联进 if（无 bool 局部）：
    //   cmpl $0,-0x14; je skip; …lea (%ebx,%eax,1); cmp -0x14(%ebp),%eax; jae skip;
    //   mov $1; jmp; skip: mov $0; test %al,%al; je。写具名 bool 会多
    //   `mov %al,-0x15; cmpb $0,-0x15` 两段槽物化（P133/P214①）。
    // P48：InvenEmptycount/QuickEmptyCount 各自 getCurCharacInvenR。
    if (need != 0 &&
        (unsigned int)(user->getCurCharacInvenR()->InvenEmptycount(
                           Inven_Item::ITEM_TYPE_3) +
                       user->getCurCharacInvenR()->QuickEmptyCount()) <
            (unsigned)need)
        return 0x15;

    const CItem* materialScript =
        G_CDataManager()->find_item(ticket.m_addInfo);
    if (itemScript == 0)
        return 0x15;

    // 附魔槽（item+0x1f/0x20 = m_random.m_skill0/1.m_field）：双槽就绪或
    // 首槽就绪且类型/技能匹配材料脚本 +0x30c（P48：逐次重新读字段）。
    // [b28d-solo-CItemUpgrade] 位域视图：ORIG 0x08549986/0x08549994 `movzbl;
    // and $0x1; test %al`（无 uchar→int 加宽）与 `shr $3,%al`（8 位无符号移位）
    // 只能由真实位域成员复现（`m_field & 1` 多 `movzbl %al,%eax`；`m_field >> 3`
    // 产 32 位 sar，探针 c1/c2/c4/c5 实证）。
    if (item.m_random.m_skill0.m_bits.m_flag &&
        item.m_random.m_skill1.m_bits.m_flag)
        return 0x13;
    if (item.m_random.m_skill0.m_bits.m_flag &&
        item.m_random.m_skill0.m_bits.m_skillId == param &&
        item.m_random.m_skill0.m_bits.m_type ==
            (int)((const CStackableItem*)materialScript)->m_char30c)
        return 0x13;

    // 遍历装备附魔表匹配 job+growType（迭代器直构 P50，end 缓存 P13）。
    // [b28d-solo-CItemUpgrade] ORIG 0x08549a0f 无 equip 具名局部（P0）：
    //   每次 `mov itemScript,%eax; mov %eax,(%esp); call getEnchantSystemData`，
    //   写具名指针会多 `mov %eax,-0x10; mov -0x10,%eax` 两段并顶浅迭代器槽。
    std::vector<STEnchantSystemData>::const_iterator it =
        ((const CEquipItem*)itemScript)->getEnchantSystemData().begin();
    std::vector<STEnchantSystemData>::const_iterator end =
        ((const CEquipItem*)itemScript)->getEnchantSystemData().end();
    bool matched = false;
    for (; it != end; ++it)
    {
        if (user->get_charac_job() == it->m_a &&
            (unsigned char)user->getCurCharFirstGrowType() == it->m_b)
        {
            matched = true;
            break;
        }
    }
    if (!matched)
        return 1;
    return 0;
}