void CAvatarItemMgr::AddEmblemEndurance(int key, const Avatar_Item* item)
{
    // [sweep-CAvatarItemMgr] P235：cast 留在表达式内时 4.4 恒 base 先载
    // （`mov -0x14(%ebp),%ecx` 在 `mov -0x10(%ebp),%edx` 之前），ORIG 是
    // 完整类型成员直访问 ⇒ i 先、base 后。把 cast 移到赋值点，局部即完整类型。
    const JewelSocketData* jewelData =
        (const JewelSocketData*)item->getJewelSocketData();
    short totalEndurance = 0;
    for (int i = 0; i <= 4; i++)
    {
        // [b28d-38] P0/P48：ORIG 对同一表达式读两次（test 与 find_item 实参各一次），
        // 无 itemId 具名槽；grade 无具名槽（P52 链式实参跨 receiver call 保活 ebx）。
        // [b28d-solo-CAvatarItemMgr] P223：ORIG 空 then 占位岛
        // （`je <nop>; body; jmp <incr>; nop; incr:`）⇒ 取反 goto 尾空标签。
        if (jewelData->m_slots[i].m_itemId == 0)
            goto next_slot;
        {
            // [sweep-CAvatarItemMgr] P254：enum 字段使 4.4 保留 `mov mem,%eax;
            // mov %eax,%ebx` 两段中转（int 字段恒折单段）。p167 12/12 CLEAR。
            CItem* citem = G_CDataManager()->find_item(
                jewelData->m_slots[i].m_itemId);
            if (citem == 0)
                return;
            totalEndurance += (short)ARAD::Singleton<AvatarRechargeServer>::Get()
                                  ->GetRateFromEmblemGrade(citem->get_grade());
        }
    next_slot:;
    }
    std::map<int, int>::iterator it = m_emblemEnduranceMap.find(key);
    // [b28d-38] P167：ORIG 调 operator==（非 !=）、insert 实参构造成员模板为
    // pair<int&,short&>（totalEndurance 为 short 左值、pair 直构非 make_pair）。
    if (it == m_emblemEnduranceMap.end())
    {
        m_emblemEnduranceMap.insert(std::pair<const int, int>(key, totalEndurance));
    }
    else
    {
        it->second = (int)totalEndurance;
    }
