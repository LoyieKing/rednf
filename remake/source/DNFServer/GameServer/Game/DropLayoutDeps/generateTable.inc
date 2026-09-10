void CItemGenRateTable::generateTable(stGenItemRange_t* range, bool keepRandomOption)
{
    // [b28d-solo-DropLayoutDeps] 203/202 条；已修：删 pairBytes/maxOffset/target
    // 局部（ORIG 全程表达式重算，P0/P183）、end() 不缓存（ORIG end 在条件块）、
    // insert 直接构造 value_type（非 make_pair，P192）、grades 用 const 指针
    //（ORIG 调 ZNK…5beginEv，消迭代器转换 ctor）、m_slots 索引用 level（ORIG 原码
    // 行为，P171 照抄）。残差=帧/槽号（N82）+ 虚调 IsRandomOption 寄存器角色（N7）。
    // 报告 generateTable.md（T 0x0853466c）逐循环转写。槽址 =
    // type*0x7110+rarity*0x12d8+level*24 ⇒ m_slots[type][rarity][level]。
    const __gnu_cxx::hash_map<int, CItem*>* grades =
        G_CDataManager()->m_itemList->getItemGradeList();
    for (int type = 1; type <= 4; ++type)
    {
        for (int rarity = 0; rarity <= 5; ++rarity)
        {
            for (int level = 1; level <= 200; ++level)
            {
                unsigned int cumulative = 0;
                // ORIG：无 pairBytes/maxOffset/target 局部（每次重算表达式）。
                int offset = -(int)(*reinterpret_cast<const unsigned char(*)[2]>(
                                  &range->m_range[level]))[0];   // -(u8)min
                for (; offset < (int)(*reinterpret_cast<const unsigned char(*)[2]>(
                                     &range->m_range[level]))[1]; ++offset)
                {
                    if (level + offset > 200)         // cmp $0xc8 jg → 终止整行
                        break;
                    for (__gnu_cxx::hash_map<int, CItem*>::const_iterator
                             it = grades[level + offset].begin();
                         it != grades[level + offset].end(); ++it)
                    {
                        CItem* item = it->second;
                        if (item->get_gen_rate() == 0)
                            continue;
                        if (item->get_rarity() != rarity)
                            continue;
                        if (!_itemTypeVerify(type, item))
                            continue;
                        if (item->isSpecialMonsterDropItem())
                            continue;
                        // param_2 假时剔除随机选项物品（虚槽 0x4c =
                        // CItem::IsRandomOption；df_game_r vtable 0x8c96da0+0x4c 实证）。
                        if (!keepRandomOption && item->IsRandomOption())
                            continue;
                        cumulative += (unsigned int)item->get_gen_rate();
                        m_slots[type][rarity][level].insert(   // ORIG 用 level（非 target）
                            std::pair<const unsigned int, int>(
                                cumulative, item->get_index()));
                    }
                }
            }
        }
    }
