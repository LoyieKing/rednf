bool CDataManager::Verify() const
{
    // [ORIG +0x0008..+0x0027] 空循环照抄（ORIG 保留 i∈[1,0xc6) 死循环体）
    for (int k = 1; k < 0xc6; ++k)
    {
    }

    // [ORIG +0x0027..+0x00d7] mob reward 单调性（i ∈ [0,200)）
    for (int i = 0; i < 200; ++i)
    {
        if ((unsigned int)m_mobReward[i + 1] <= (unsigned int)m_mobReward[i])
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x2723, 9, true, true);
            trace("Warning: Unusual mob reward(%d) on mob grade(%d)",
                  (int)(unsigned int)m_mobReward[i], i);
        }
    }

    // [ORIG +0x00d7..+0x0247] 11 个职业的技能 required-level-range tag 检查
    bool skillFail = false;
    for (int job = 0; job <= 0xa; ++job)
    {
        for (__gnu_cxx::hash_map<int, CSkill*>::const_iterator
                 it(m_skillList->getSkillList(job)->begin()),
                 e(m_skillList->getSkillList(job)->end()); it != e; ++it)
        {
            // 【ORIG asm 08360f12/08360f15: mov 0x4(%eax),%eax; mov 0xc8(%eax),%eax】
            // ORIG 无 skill/sp 具名局部（P0：无槽往返 ⇒ 表达式直读；P335 消 P 对）。
            if (*(const int*)((const char*)it->second + 0xc8) == 0)
            {
                cMyTrace trace(__PRETTY_FUNCTION__, 0x272e, 9, true, true);
                trace("Skill [required level range] tag value error "
                      "(Job: %d)(Skill: %d)", job, it->second->get_index());
                skillFail = true;
            }
        }
    }
    if (skillFail)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x2735, 9, true, true);
        trace("Verifying Skills fail");
        return false;
    }

    // [ORIG +0x02a3..+0x0363] 主地图校验（逐张覆盖失败标志，不提前 break；
    // ORIG 同样每次覆写 local_69 后统一判定）
    bool mapFail = false;
    {
        // asm 0836103b: this+0x04 → hash_map 直接作用于该对象首部
        // （CMapList::m_mainMap +0x00）
        const CMapList* ml = m_mapList;
        for (__gnu_cxx::hash_map<int, CMap*>::const_iterator
                 it(ml->m_mainMap.begin());
             it != ml->m_mainMap.end(); it++)
        {
            // ORIG 形态：VerifyMap 返回值 xor 1 存入标志位，逐张覆盖不提前 break
            mapFail = ((VerifyMap(*it->second, -1) ^ 1) & 0xff) != 0;
        }
    }
    if (mapFail)
    {
        return false;
    }

    // [ORIG +0x0363..+0x03f8] 全副本 VerifyDungeon（this+0x00 直接作用 =
    // CDungeonList::m_dungeonMap +0x00）
    for (__gnu_cxx::hash_map<int, CDungeon*>::const_iterator
             it(m_dungeonList->m_dungeonMap.begin());
         it != m_dungeonList->m_dungeonMap.end(); it++)
    {
        VerifyDungeon(*it->second);
    }

    // [ORIG +0x03f8..+0x0994] 道具表 booster/package 引用完整性
    bool itemFail = false;
    {
        // asm 083611c8: this+0x0c(m_itemList) + lea 0x4 → CItemList::m_itemMap
        for (__gnu_cxx::hash_map<int, CItem*>::const_iterator
                 it(m_itemList->m_itemMap.begin());
             it != m_itemList->m_itemMap.end(); ++it)
        {
            CItem* item = it->second;
            // ORIG 正极性守卫（asm 08361222 `test %al,%al; je 083617d5`，无 xor $1）：
            // 4.4 对 `if (!f())` 恒发 xor $1（P168a），故改写为 if (f()) 包裹 body
            // （下列 body 缩进未调整，纯机器码形态对齐）。
            if (item->is_stackable())
            {
            // ORIG 逐点重读 GetItemType()（P48/P157）：|| 短路两臂各一次虚调用
            // （ORIG 08361240 cmp $0xf / 08361257 cmp $0x10），不可缓存到局部。
            if (item->GetItemType() == 0xf || item->GetItemType() == 0x10)
            {
                // booster：CStackableItem+0x240 stStackableBooster_t.m_elements
                CStackableItem* sitem = (CStackableItem*)item;
                for (std::vector<stStackableBoosterElement_t>::const_iterator
                         el = sitem->m_booster.m_elements.begin(),
                         elEnd = sitem->m_booster.m_elements.end();
                     el != elEnd; ++el)
                {
                    for (std::vector<stStackableBoosterItemInfo_t>::const_iterator
                             info = el->m_items.begin(),
                             infoEnd = el->m_items.end();
                         info != infoEnd; ++info)
                    {
                        if (info->m_itemIdx == 1)
                        {
                            continue;
                        }
                        if (G_CDataManager()->find_item(info->m_itemIdx) == 0)
                        {
                            cMyTrace trace(__PRETTY_FUNCTION__, 0x275a, 9, true, true);
                            trace("Booster item element not found (%d)->(%d)",
                                  item->get_index(), info->m_itemIdx);
                            itemFail = true;
                        }
                    }
                }
            }
            else if (item->GetItemType() == 0xe)  // ORIG 08361467 cmp $0xe
            {
                // package：包列表/选择列表逐元素 find_item 校验。
                // 共享头 m_pkg21c/m_pkg234 元素类型暂为 vector<char> 占位
                // （STStackableScript.h 循环依赖无法具名化），沿用
                // CCeraShop_Special.cpp:409 已确立的 reinterpret_cast 模式。
                CStackableItem* sitem = (CStackableItem*)item;
                std::vector<STStackablePackageData>& pkgs =
                    *reinterpret_cast<std::vector<STStackablePackageData>*>(
                        &sitem->m_pkg21c);  // 【CStackableItem+0x21c】
                for (std::vector<STStackablePackageData>::const_iterator p =
                         pkgs.begin(),
                         pEnd = pkgs.end();
                     p != pEnd; ++p)
                {
                    if (p->m_field0 == 1)
                    {
                        continue;
                    }
                    if (G_CDataManager()->find_item(p->m_field0) == 0)
                    {
                        cMyTrace trace(__PRETTY_FUNCTION__, 0x276b, 9, true, true);
                        trace("Package item element not found (%d)->(%d)",
                              item->get_index(), p->m_field0);
                        itemFail = true;
                    }
                }

                const std::vector<std::vector<STStackablePackageData> >& sel =
                    *reinterpret_cast<
                        std::vector<std::vector<STStackablePackageData> >*>(
                        &sitem->m_pkg234);  // 【CStackableItem+0x234】
                for (std::vector<std::vector<STStackablePackageData> >::
                         const_iterator group = sel.begin();
                     group != sel.end(); ++group)
                {
                    const std::vector<STStackablePackageData> copy(*group);
                    for (std::vector<STStackablePackageData>::const_iterator p =
                             copy.begin(), pEnd = copy.end();
                         p != pEnd; ++p)
                    {
                        if (p->m_field0 == 1)
                        {
                            continue;
                        }
                        if (G_CDataManager()->find_item(p->m_field0) == 0)
                        {
                            cMyTrace trace(__PRETTY_FUNCTION__, 0x277b, 9, true, true);
                            trace("Package item selection element not found "
                                  "(%d)->(%d)",
                                  item->get_index(), p->m_field0);
                            itemFail = true;
                        }
                    }
                }
            }
            else
            {
                // ORIG 083617d0 虚调用后 `cmp $0x22,%eax` 无消费者（P171 死比较照抄）
                (void)(item->GetItemType() == 0x22);
            }
            }
        }
    }

    if (!itemFail)
    {
        // [ORIG +0x0994..+0x1149] Quest 表奖励引用检查
        bool questFail = false;
        const QuestList* ql = m_questList->getQuestList();
        for (__gnu_cxx::hash_map<int, Quest*>::const_iterator
                 qit(ql->m_questMap.begin()),
                 qe(ql->m_questMap.end()); qit != qe; ++qit)
        {
            Quest* quest = qit->second;

            // 【ORIG asm 083618aa: mov 0xb8(%eax)】quest 类型/tag 字段
            // ORIG 无 qp 具名局部（P0）：按 quest 指针直接偏移访问。
            const int state = *(const int*)((const char*)quest + 0xb8);
            if (state != 0 && state != 0x19)
            {
                continue;
            }

            // 【ORIG +0xc4/+0xd0】两段 RewardItemInfo 奖励向量（当前 Quest.h
            // 未建模此区布局，按 ORIG 偏移访问）
            {
                std::vector<RewardItemInfo>& v =
                    *reinterpret_cast<std::vector<RewardItemInfo>*>(reinterpret_cast<char*>(quest) + 0xc4);
                for (std::vector<RewardItemInfo>::const_iterator it = v.begin();
                     it != v.end(); ++it)
                {
                    const int itemIdx = it->m_itemIdx;
                    if (itemIdx != 0 && itemIdx != 1 && itemIdx != 2 &&
                        itemIdx != 999 &&
                        this->find_item(itemIdx) == 0)
                    {
                        cMyTrace trace(__PRETTY_FUNCTION__, 0x27a1, 5);
                        trace("Quest reward item element not found "
                              "(quest: %d)(item: %d)",
                              *(const int*)((const char*)quest + 0x04), it->m_itemIdx);  // 【ORIG asm 08361b7e: quest+0x04】
                        questFail = true;
                    }
                }
            }
            {
                std::vector<RewardItemInfo>& v =
                    *reinterpret_cast<std::vector<RewardItemInfo>*>(reinterpret_cast<char*>(quest) + 0xd0);
                for (std::vector<RewardItemInfo>::const_iterator it = v.begin();
                     it != v.end(); ++it)
                {
                    const int itemIdx = it->m_itemIdx;
                    if (itemIdx != 0 && itemIdx != 1 && itemIdx != 2 &&
                        itemIdx != 999 &&
                        this->find_item(itemIdx) == 0)
                    {
                        cMyTrace trace(__PRETTY_FUNCTION__, 0x27ac, 5);
                        trace("Quest reward select item element not found "
                              "(quest: %d)(item: %d)",
                              *(const int*)((const char*)quest + 0x04), it->m_itemIdx);  // 【ORIG asm 08361b7e: quest+0x04】
                        questFail = true;
                    }
                }
            }
            // 【ORIG +0xdc】collision quest 列表（vector<int>）
            {
                std::vector<int>& v =
                    *reinterpret_cast<std::vector<int>*>(reinterpret_cast<char*>(quest) + 0xdc);
                for (std::vector<int>::const_iterator it = v.begin();
                     it != v.end(); ++it)
                {
                    if (find_quest(*it) == 0)
                    {
                        cMyTrace trace(__PRETTY_FUNCTION__, 0x27cd, 5);
                        trace("Quest reward collision quest element now found "
                              "(quest: %d)(collision quest: %d)",
                              *(const int*)((const char*)quest + 0x04), *it);  // 【ORIG asm 08361b7e: quest+0x04】
                        questFail = true;
                    }
                }
            }
            // 【ORIG +0xec】collision item 向量（RewardItemInfo）
            {
                std::vector<RewardItemInfo>& v =
                    *reinterpret_cast<std::vector<RewardItemInfo>*>(reinterpret_cast<char*>(quest) + 0xec);
                for (std::vector<RewardItemInfo>::const_iterator it = v.begin();
                     it != v.end(); ++it)
                {
                    const int itemIdx = (*it).m_itemIdx;
                    if (itemIdx != 0 && itemIdx != 1 && itemIdx != 2 &&
                        itemIdx != 999 &&
                        this->find_item(itemIdx) == 0)
                    {
                        cMyTrace trace(__PRETTY_FUNCTION__, 0x27da, 5);
                        trace("Quest reward collision item element not found "
                              "(quest: %d)(item: %d)",
                              *(const int*)((const char*)quest + 0x04), (*it).m_itemIdx);  // 【ORIG asm 08361b7e: quest+0x04】
                        questFail = true;
                    }
                }
            }
        }

        if (!questFail)
        {
            // [ORIG +0x1187..] 怪物等级区间检查（CDataManager+0x08 管理器 +
            // 0x18 monsterMap；CMonster+0x58/+0x5c level min/max）
            for (__gnu_cxx::hash_map<int, CMonster*>::const_iterator
                     it(m_monsterManager->m_monsterMap.begin());
                 it != m_monsterManager->m_monsterMap.end(); it++)
            {
                CMonster* mon = it->second;
                // ORIG 无 mp 具名局部（P0）：按 mon 指针直接偏移访问。
                const int lvA = *(const int*)((const char*)mon + 0x58);  // 【ORIG asm +0x10de】
                const int lvB = *(const int*)((const char*)mon + 0x5c);  // 【ORIG asm +0x10db】
                if (lvA < 1 || 200 < lvA || lvB < 1 || 200 < lvB)
                {
                    cMyTrace trace(__PRETTY_FUNCTION__, 0x2808, 9, true, true);
                    trace("Error: Invalid monster(%d) level(%d-%d)",
                          mon->get_index(), lvA, lvB);
                }
            }

            // [ORIG +0x11a0..+0x1313] 全副本通关模拟（WongWork::CDungeonClear）
            WongWork::CDungeonClear clearSim;
            for (__gnu_cxx::hash_map<int, CDungeon*>::const_iterator
                     it(m_dungeonList->m_dungeonMap.begin());
                 it != m_dungeonList->m_dungeonMap.end(); ++it)
            {
                // ORIG 08361fab: mov %eax,-0x1c(%ebp); mov -0x1c(%ebp),%eax
                // ⇒ 原码有具名局部（P0：有槽往返）；08361fb9 cwtl = (short) 转换。
                CDungeon* dungeon = it->second;
                clearSim.addClearedDungeon((short)dungeon->get_index(), '\x04');
            }
            char buf[0x1800];
            if (clearSim.toString(buf, 0x1800) < 0)
            {
                cMyTrace trace(__PRETTY_FUNCTION__, 0x2815, 9, true, true);
                trace("Simulate dungeon clear data fail!!!!");
            }
        }
    }

    return true;
