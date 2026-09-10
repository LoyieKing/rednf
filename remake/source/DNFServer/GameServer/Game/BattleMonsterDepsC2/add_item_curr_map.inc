void CBattle_Field::add_item_curr_map(int& itemUid, int& itemIdx)
{
    if (m_pDungeon == NULL || m_pMap == NULL)
        return;

    GetCurrentMapInfo()->m_assignItemIdx = itemIdx;                       // MapInfo+0x3c

    std::list<_mapItem> itemList = m_pMap->get_item_list();
    std::list<_mapItem>::iterator it = itemList.begin();
    // ORIG +0x077 `call GetCurrentMapInfo; mov 0x4(%eax),%eax; mov %eax,-0x24` ⇒
    // 循环前缓存 m_mapIndex 到具名局部（p167 MISSING GetCurrentMapInfo×1）。
    const int curMapIndex = GetCurrentMapInfo()->m_mapIndex;
    (void)curMapIndex;
    _mapItem item;                     // ORIG @0x08302c0e 循环前 C1（-0x44）
    // [b28d-D9] P167 判据②（重载选错）：ORIG 0x08302bc6 调 const 版
    // `_ZNKSt6vectorI21stDungeonAssignItem_tSaIS0_EE2atEj`，我方非 const 版
    // `_ZNSt6vector...2atEj`（MISSING/EXTRA 各 1）⇒ 容器按 const 指针访问。
    const std::vector<stDungeonAssignItem_t>* specialItems =
        m_pDungeon->getSpecialPassiveObjectItems();

    stGenerateRefData_t refRandom;                                        // A @-0xc4
    stGenerateRefData_t refSpecific;                                      // B @-0x10c
    stGenerateRefData_t refDrop;                                          // C @-0x154
    refRandom.m_field35 = static_cast<char>(get_dungeon_diff());
    refRandom.m_rateSlot = 1;
    // [b28d-solo-BattleMonsterDepsC2] §4.3：S290 已在 stGenerateRefData_t +0x34 建具名
    // union 视图（P235）⇒ 改用 m_field34v（vector<pair<int,int>> const*），消除裸偏移。
    refDrop.m_field34v = m_pDungeon->getObjectDropItems();

    while (it != itemList.end() && m_bloodState != 1)                     // +0x1b4
    {
        // [b28d-D9] P167：ORIG 0x08302c46 段用 `item = *it`（List_iterator::de
        // + 4 dword 整块拷贝）而非 it-> 四字段（ptEv×4）；日志段 curMapIndex 亦
        // 无具名槽，重调 GetCurrentMapInfo()->m_mapIndex（P48 无 CSE）。
        item = *it;

        // [b28d-Lane27] P0：ORIG 无 passiveIdx/field04/field08/field0c 具名槽——四者
        // 在使用点直接重读 item 槽（-0x44/-0x40/-0x3c/-0x38），删局部消 4 槽。
        // [b28d-Lane27] ORIG 0x08302cf2 段：先 `cmp size,idx; jbe`（size<=idx）再
        // `test idx; jns`（有符号 >=0）⇒ 按 ORIG 原子序书写（交换后 4.4 产 415 形）。
        if (specialItems->size() <= static_cast<unsigned int>(item.m_objectIdx) ||
            item.m_objectIdx < 0)
        {
            LogManager::logFormat(
                1, "battle_field.cpp",
                __PRETTY_FUNCTION__, 0x7a5,
                "Dungeon passive object index error! Map id:%d, DungeonAssignItem "
                "size:%d, Passive object id:%d",
                GetCurrentMapInfo()->m_mapIndex,
                static_cast<int>(specialItems->size()), item.m_objectIdx);
            return;
        }

        const stDungeonAssignItem_t& assign = specialItems->at(item.m_objectIdx);
        if (assign.m_assignItemIdx != -1)
        {
            refRandom.m_field34 = static_cast<char>(assign.m_assignItemIdx);
            // ORIG byte 写 +0x38 = m_rankBonusIndex 低字节（int 槽内字节写）
            // [b28d-Lane27] 权威头 +0x38 union 已有具名字节视图 m_field38/39 ⇒ 直用，
            // 取址形 `*(uchar*)&m_rankBonusIndex` 4.4 恒产 `lea 0x38; mov %al,(%edx)`（+2/处）。
            refSpecific.m_field38 = static_cast<unsigned char>(assign.m_assignItemIdx);
        }
        else
        {
            // [b28d-D9] P48：ORIG 0x08302d80 段两次独立 get_standard_level()
            // （无具名 stdLvl），且两个赋值目标各自重取源表达式。
            // [b28d-Lane27] ORIG 0x08302dbd 段：`test;jle TRUE(坏)` / `cmp $0xc8; jle FALSE`
            // ⇒ 物化「坏」= lvl<=0 || lvl>200 并早 continue（原 && 正向产 jg/je 反向）。
            if (m_pDungeon->get_standard_level() <= 0 ||
                m_pDungeon->get_standard_level() > 200)
            {
                continue;          // ORIG 直跳循环头（不推进迭代器，见函数头注释）
            }
            // [b28d-Lane27] ORIG 0x08302dfc `test %al; setg %al; test; je`（有符号
            // 「>0」物化）⇒ 写正极性 `> 0` 并交换两臂（then=dimension 分支在前）。
            // [b28d-Lane27] P292 探针 /tmp/l27_probe.cpp g2：ORIG 0x08302dfc
            // `test %al; setg %al` 需**有符号**比较 ⇒ 加 (char) cast（无 cast 生 setne）。
            if (static_cast<char>(m_pDungeon->get_dimension_possible()) > 0)
            {
                refRandom.m_field34 = static_cast<char>(m_standardDimensionLevel);
                refSpecific.m_field38 =
                    static_cast<unsigned char>(m_standardDimensionLevel);
            }
            else
            {
                refRandom.m_field34 =
                    static_cast<char>(m_pDungeon->get_standard_level());
                refSpecific.m_field38 = static_cast<unsigned char>(
                    m_pDungeon->get_standard_level());
            }
        }

        // [b28d-Lane27] ORIG 0x08302e53 段序：result C1 → m_pad37(field08) →
        // +0x39(field04) → +0x34(&assign.m_list)——照 ORIG 语句序（原 result
        // 声明在三写之后、字段写序亦反）。
        stGenerateResult_t result;
        refRandom.m_pad37 = static_cast<char>(item.m_field08);
        // ORIG byte 写 +0x39 = m_rankBonusIndex 第 2 字节（保留 0/2/3 字节）
        // [b28d-Lane27] ORIG 0x08302e67 为**单字节直写**（`mov -0x40,%eax; mov %al,-0xd3`）；
        // 读改写 `(x & 0xffff00ff) | (v<<8)` 生 5 条 → 改字节视图 +1。
        refSpecific.m_field39 = static_cast<unsigned char>(item.m_field04);
        // [b28d-solo-BattleMonsterDepsC2] §4.3：+0x34 = assign.m_list 指针（assign+0x04），
        // 用 S290 具名视图 m_field34l（list<pair<int,int>> const*）替代裸偏移。
        refSpecific.m_field34l = &assign.m_list;                          // assign+0x04

        // [b28d-Lane27] P0：ORIG 无 gen 具名槽（0x08302e99 段接收者留 %eax 直到
        // `call *%ecx`）⇒ 三处链式内联，删局部消 1 槽。
        G_CDataManager()->getItemGenerator()->getGenerator(
            WongWork::CItemGeneratorMgr::eGenerateType_1)
            ->generateRandomItem(refRandom, result);
        G_CDataManager()->getItemGenerator()->getGenerator(
            WongWork::CItemGeneratorMgr::eGenerateType_1)
            ->generateSpecificItem(refSpecific, result);
        G_CDataManager()->getItemGenerator()->getGenerator(
            WongWork::CItemGeneratorMgr::eGenerateType_4)
            ->generateSpecificItem(refDrop, result);

        // [b28d-Lane27] P13：ORIG 0x08302f2a/0x08302f3f 把 begin/end 各求值一次缓存成
        // 两个具名迭代器（-0x48/-0x4c），条件段只做 ne ⇒ 缓存 end（原 for 每轮重调）。
        std::vector<Inven_Item>::iterator dropIt = result.m_dropItems.begin();
        std::vector<Inven_Item>::iterator dropEnd = result.m_dropItems.end();
        for (; dropIt != dropEnd; ++dropIt)
        {
            map_item mi;
            mi.m_count = 1;
            // [b28d-Lane27] ORIG 0x08302f73 段：`mov itemUid,%eax; mov %eax,mi+4;
            // lea 0x1(%eax),%edx; mov %edx,itemUid` ⇒ 后缀自增单次读取（原写成
            // 两句 `mi.m_itemIndex = itemUid; itemUid = mi.m_itemIndex + 1;`
            // 4.4 会重读成员槽 +1 条）。
            mi.m_itemIndex = itemUid++;
            mi.m_standardLevel = static_cast<char>(item.m_field0c);
            mi.m_item = *dropIt;
            GetCurrentMapInfo()->Add_Item(mi);
        }
        ++it;
    }
