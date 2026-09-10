void _check_disjoint_need_slot(CUser* user, CItem* item, Inven_Item& invenItem,
                               expert_job::CDisjointer* disjointer)
{
    // [b28d-solo-ExpertJobLink4 P214③/P312]：ORIG 0x81f8fef..0x81f902c 三个
    // 11-dword `rep stos`（清零序 total -0x60 → perType -0x8c → emptySlots
    // -0xb8）⇒ 三个 `= {0}` 显式零初始化数组，非循环赋值。
    int total[11] = {0};
    int perType[11] = {0};
    int emptySlots[11] = {0};

    std::vector<int>* predicted =
        GetPredictDisjointItems(user, item, invenItem, disjointer);

    for (unsigned int i = 0; i < predicted->size(); ++i)
    {
        // [b28d-solo-ExpertJobLink4 P157/P183]：ORIG 对 (*predicted)[i] 共三次
        // 独立下标调用（判零 / find_item / probe.m_addInfo），无 itemIndex 槽。
        if ((*predicted)[i] == 0)
            continue;

        CItem* templateItem = G_CDataManager()->find_item((*predicted)[i]);
        if (templateItem == 0)
            continue;

        Inven_Item probe;                       // ORIG 局部 ctor 清零
        probe.m_addInfo = (*predicted)[i];      // 直接写 +0x02
        probe.set_add_info(1);                  // ORIG：m_addInfo2 = 1（set_add_info 语义）
        templateItem->make_item(probe);         // ORIG 0x81f90f3..0x81f9108：vptr+8
                                                // 虚槽 make_item 填充 type 等（CItem.h vtable 槽 2）
        // ORIG 用立即数 `cmpl $0xb,-0x24`（kInvalidSlot 无具名槽，P0）。
        int needSlot = Inven_Item::ITEM_TYPE_INVALID;
        // ORIG 每次调用前重新 getCurCharacInvenR()，itemIndex 取 probe.m_addInfo
        // （+0x02），probeType 取 probe.m_field1（+0x01）逐次重读（无具名局部）。
        if (user->getCurCharacInvenR()->IsExistStackable(
                probe.m_addInfo, (Inven_Item::ITEM_TYPE)probe.m_field1))
        {
            needSlot = (int)probe.m_field1;
        }
        else if (user->getCurCharacInvenR()->IsExistStackable(
                     probe.m_addInfo, (Inven_Item::ITEM_TYPE)0))
        {
            needSlot = 0;
        }

        // ORIG 0x81f9183 `cmpl $0xb,-0x24; je <total[probeType] 远端块>`：
        // 反写条件使 invalid 分支落远端（P47/P222）。
        if (needSlot != Inven_Item::ITEM_TYPE_INVALID)
        {
            // ORIG 单次索引读（`mov needSlot,%eax; mov arr(%ebp,%eax,4),%edx`）；
            // `+= 1` 在 4.4 恒多读一次索引槽（+1 条/处）。
            ++perType[needSlot];
            ++total[needSlot];
        }
        else
        {
            ++total[probe.m_field1];
        }
    }

    for (int i = 0; i < 11; ++i)
        emptySlots[i] = user->getCurCharacInvenR()->InvenEmptycount(
            (Inven_Item::ITEM_TYPE)i);

    int deficit = 0;
    for (int i = 0; i < 11; ++i)
    {
        if (emptySlots[i] + perType[i] < total[i])
            deficit += total[i] - (emptySlots[i] + perType[i]);
    }

    if (deficit > emptySlots[0])
        throw static_cast<ENUM_ERROR>(4);
