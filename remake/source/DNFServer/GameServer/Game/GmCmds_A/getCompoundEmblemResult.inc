bool CBoosterTest::getCompoundEmblemResult(CItem const* item,
                                           std::map<int, int>& result)
{
    // [b28d-solo-GmCmds_A] ORIG +0x00e..+0x039：两个具名 item 副本
    // （-0x28/-0x24）+ 死槽 count=2（-0x20）+ jewelType 预置 -1（-0x1c）。
    // [round5-GmCmds_A] P212/判据⑩：ORIG sockets 槽 -0x48 **深于** p1(-0x28)
    // ⇒ 4.1「先声明者深」⇒ 原码 sockets 声明在 p1/p2 之前（仅赋值在后）。
    unsigned short sockets[2];
    CItem const* p1 = item;
    CItem const* p2 = item;
    int count = 2;
    sockets[0] = static_cast<CStackableItem const*>(p1)->getJewelTargetSocket();
    sockets[1] = static_cast<CStackableItem const*>(p2)->getJewelTargetSocket();

    int jewelType = -1;
    jewelType = this->_selectJewelTypeIndex(2, sockets);
    (void)count;
    // [b28d-solo-2] P295/P225：ORIG +0x05d `cmpl $-1,槽; sete; test; je` 为
    // 4.1 槽比较 bool 物化形，4.4 对 if 恒折 `jne` 直跳 ⇒ while+return 复现。
    while (jewelType == -1)
    {
        return false;
    }

    // 徽章映射表：ORIG = G_CDataManager() + 0xc + (0x208 + grade*6 + jewelType)*4
    // （P0：无 grade 具名槽，get_grade 内联进下标表达式；P291 具名数组视角）。
    // [round5-GmCmds_A] 判据⑩/帧对齐：ORIG 帧 0x70 与 p1/p2/count/jewelType
    // 槽（-0x28/-0x24/-0x20/-0x1c）要求本处存在一个具名 grade 槽（去掉后帧塌
    // 0x60 且全组浅移 4）；get_grade 实参用 p1（ORIG 0x084af155 `mov -0x28(%ebp)`)
    int grade = p1->get_grade();
    int emblemId =
        reinterpret_cast<const CBoosterEmblemIdTableView*>(G_CDataManager())
            ->m_ids[grade * 6 + jewelType + 0x208];

    CStackableItem const* emblem =
        static_cast<CStackableItem const*>(G_CDataManager()->find_item(emblemId));
    if (emblem == 0 || emblem->GetItemType() != 0x10)
    {
        return false;
    }

    for (int i = 0; i < m_count; ++i)
    {
        std::vector<stStackableBoosterItemInfo_t> items;
        emblem->getBoosterItem(items);
        // [b28d-solo-GmCmds_A] P167/P28：同 getBoosterResult，两个具名
        // const_iterator（begin/end 各一次 C1<iterator> 转换构造）+ while 形。
        std::vector<stStackableBoosterItemInfo_t>::const_iterator it = items.begin();
        std::vector<stStackableBoosterItemInfo_t>::const_iterator end = items.end();
        while (it != end)
        {
            // [b28d-solo-GmCmds_A] P167：本件 ORIG 用 ptEv（08111bc0），与
            // getBoosterResult 的 deEv（084b4a16）相反；条件取反避免第二个
            // ppEv 调用点（ORIG 仅 +0x267 一处 ppEv）。
            // [round5-A20 C05④ 归因核 · 实测回退] ORIG 0x084af25f `mov %eax,-0xc(%ebp);
            //   cmpl $0x0,-0xc(%ebp); je`（find_item 结果有槽再比较，P0/P115）。
            //   试补具名局部 `CItem* foundInner = …` 实测：setne 消除 ✓，但 4.4 把该槽
            //   分配到**浅区 -0x2c**，挤出 p1/p2/count/sockets 整组错位（p1 多一份 -0x2c
            //   副本）、指令数 217→**216**（gap 0→−1）、归一 ndiff 191 vs 旧 254。
            //   与 A28/P0「我方多槽不可控」同族：**净账不达标（gap 由 0 退化 −1 +
            //   头部槽集新增 ORIG 无的 -0x2c）⇒ 已回退**，保留 gap 0 与头/尾槽集全等的旧形。
            if (G_CDataManager()->find_item(it->m_itemIdx) != 0)
            {
                int key = it->m_itemIdx;
                std::map<int, int>::iterator pos = result.find(key);
                // [b28d-solo-GmCmds_A] P220：ORIG 用 ne（`call ne; test; je <else>`）；
                // operator[] 实参重读 it->m_itemIdx（p167 ptEv×6）。
                if (pos != result.end())
                {
                    result[it->m_itemIdx] += it->m_addInfo;
                }
                else
                {
                    result[it->m_itemIdx] = it->m_addInfo;
                }
            }
            ++it;
        }
    }
    return true;
