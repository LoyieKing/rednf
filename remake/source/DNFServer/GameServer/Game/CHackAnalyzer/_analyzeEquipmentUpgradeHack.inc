void CHackAnalyzer::_analyzeEquipmentUpgradeHack(unsigned short const* table)
{
    // ORIG 0x080f7e18（_analyzeEquipmentUpgradeHack.md）：当前角色背包装备栏
    // 前 0x16 格 Inven_Item::GetUpgrade 低字节须逐一等于客户端上报表值。
    if (checkCollectedHack(REPORT_4_HACK_FLAG_EQUIPMENT_UPGRADE))
        return;

    // [hc 收] ORIG 0x080f7e18 寻址链五条 = imul $0x3d;add $0x10;add inv(-0x10 槽);
    // add $0xc;mov %eax,(%esp)（两常量分列、i 侧先算）。旧裸偏移链 4.4 恒折
    // inv-first + 0x1c 单常量（判 N1+N18a 已作废）。视觉具名化 = 槽序列视图：
    // CInventory 起始 +0x10 为 0x3d 步长槽数组、槽内 +0xc 为 Inven_Item 区；
    // 探针 /tmp/hc_probe.cpp fF/fG 与 ORIG 逐条同形（元素内 char[0x31] 数组退化）。
    // 语义承载 = 槽序列（未建模子对象占位，§5.3），GetUpgrade 具名访问槽内项。
    struct InvenSlotView
    {
        char m_head[0xc];        // +0x00..+0x0b（本函数不读）
        char m_item[0x31];       // +0x0c：Inven_Item 区（0x3d - 0xc）
    };
    struct InvenSlotsView
    {
        char m_head[0x10];       // +0x00..+0x0f
        InvenSlotView m_slots[0x16];
    };
    static_assert(sizeof(InvenSlotView) == 0x3d, "InvenSlotView stride drift");
    static_assert(offsetof(InvenSlotView, m_item) == 0xc, "slot item offset drift");
    static_assert(offsetof(InvenSlotsView, m_slots) == 0x10, "slots base drift");

    const InvenSlotsView* inv = reinterpret_cast<const InvenSlotsView*>(
        ((CUser*)m_baseHackType)->getCurCharacInvenR());
    for (int i = 0; i < 0x16; ++i)
    {
        if (((const Inven_Item*)inv->m_slots[i].m_item)->GetUpgrade() != table[i])
        {
            _addHackInfo(REPORT_4_HACK_FLAG_EQUIPMENT_UPGRADE,
                         (ENUM_HACKTYPE)0x3c3, 0);
            return;
        }
    }
