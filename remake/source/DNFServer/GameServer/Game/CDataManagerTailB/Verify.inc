// ---------------------------------------------------------------------------
// Verify() const —— ORIG T 0x08360d94（0x1359 字节）
// 结构（报告 ## 反编译 C 全文对照）：
//   1) mob reward 单调性检查（m_mobReward[i+1] <= m_mobReward[i] → trace 0x2723）；
//   2) 11 个职业技能表 CSkill+0xc8（required level range tag）==0 → 报错任一命中
//      则 trace 0x2735 并返回 false；
//   3) 全部主地图 VerifyMap(map, -1)，失败返回 false；
//   4) 全部副本 VerifyDungeon(dungeon)；
//   5) 道具表遍历：stackable 的 booster/package 引用完整性检查（缺失即置错）；
//   6) Quest 表奖励引用检查（reward/collision item & collision quest）；
//   7) 怪物等级区间合法性检查；
//   8) WongWork::CDungeonClear 全副本模拟（diff=4）输出二进制串，失败打
//      "Simulate dungeon clear data fail!!!!"；全部通过返回 true。
// 注：CSkill+0xc8、quest+0xb8/+0xc4/+0xd0/+0xdc/+0xec、CMonster+0x58/+0x5c 在
// 当前权威头尚未具名化，按 ORIG 偏移访问并标注【ORIG asm 证据】；待对应头批次
// 收口后替换为具名字段。
// ---------------------------------------------------------------------------
// [b28d-solo-CDataManagerTailB] N：N82 —— §6.2 三件套：p167 166=166 CLEAR
//   （MISSING/EXTRA 归零）、gap -49 条（4.4 多具名槽往返）、帧 0x1ac0 vs
//   0x1abc（-4）。语义完成项：空循环照抄（ORIG i∈[1,0xc6) 死体）、
//   __PRETTY_FUNCTION__（12 处）、booster/package 经 G_CDataManager()->find_item、
//   选择列表 vector 拷贝构造+析构、getSkillList 双调用、getQuestList 具名、
//   collision item 用 (*it)、toString 返回值判失败（删 memset 近似）。
//   残余为 4.1/4.4 槽复用策略（N82）与 STL 内联/转换构造发射差（N89 族）。
//   ORIG 0x08360d94。
// [SweepCDataManagerTailB 2026-09-10] 修真缺口（判据㉑ 缺调用 + 判据② call * 虚槽）：
//   ① ORIG 道具循环对 `item->GetItemType()`（vtable+0xc，_ZTV14CStackableItem
//      vptr+0xc=CStackableItem::GetItemType）共 **4 次虚调用**（08361240/08361257/
//      08361467/083617d0，Ghidra C 亦示 || 两臂各一次 + else 一次 + 死比较一次）；
//      旧码把结果缓存进 `const int type` 只调 1 次 ⇒ 缺 2 次 call *。已改逐点重读。
//   ② ORIG is_stackable 守卫为 08361222 `test %al,%al; je 083617d5`（正极性，
//      无 xor $1）；旧码 `if(!is_stackable()) continue;` 4.4 恒发 xor $1（P168a），
//      已改 `if (is_stackable()) { body }` 包裹。
//   读数：raw insns 1154→1172→1167（ORIG 1174）。二轮按 P335/P0 对齐：删 4 个
//   ORIG 无的具名局部（skill/sp、qp、mp；P0 无槽往返 ⇒ 表达式直读），补 ORIG
//   有的 dungeon 局部（08361fab mov %eax,-0x1c; mov -0x1c,%eax）⇒ fn_diff_tu
//   bad 1068→989、store-reload 成对 P 7→3；call * 2→4=ORIG 4；
//   p167 166=166 CLEAR（MISSING 全为 C1/C2、D1/D2 同址别名，P93 豁免）。
//   残余：4.1 多路径 jmp+nop 汇合块（ORIG jcc 103 vs 我方 95）、P197④ setcc 物化、
//   N18a lea/add、N122 bool 取反 xor（我方 2 处 vs ORIG 1 处）、else 分支
//   `cmp $0x22` 被 4.4 DCE（P171 死比较跨版本前端差）。
// [l5f 2026-09-11 · 二轮] ①§4.3 裸偏移具名化：CSkill::m_fieldC8（+0xc8）、
//   Quest::{m_fieldB8(+0xb8), m_index(+0x04)×4, m_titleReward(+0xc4),
//   m_selectReward(+0xd0), m_fieldDC(+0xdc), m_rewardItems(+0xec)} —— ORIG disp
//   折叠形对齐（raw 1167→1161）。CMonster+0x58/+0x5c 的 m_independentDropCount/
//   m_catchItemCount 虽已具名（CMonster.h:50/51）但为 **private**（编译实证
//   02:45 报错）⇒ 本 TU 无法具名访问；.h 项按纪律只报告：建议属主提 public 或
//   加 const getter（零布局影响），届时本处两行可同批具名化。
//   ②槽集合对比（报告级）：ORIG 91 槽 / 我方 96 槽，主体为**恒定 0xc 平移**
//   （ORIG -0x54/-0x58/-0x5c ↔ 我方 -0x60/-0x64/-0x68，bool 汇聚槽 -0x6a↔-0x6b），
//   帧 ORIG `sub $0x1abc`+push edi/esi/ebx vs 我方 `sub $0x1ab0`+push esi/ebx
//   （T=sub+4×push 差 0x10=4 具名对象；push edi 属 4.4 寄存器压力，P332 源级不可
//   诱导）⇒ 按 §6.2 三件套判 N82/N126 封顶：gap 13、p167 166=166 CLEAR、槽位集恒差；
//   勿用 pad/volatile 强凑（P182/P332 反例）。
//   ③负结论（GI 假缺口，Main 2026-09-11 广播）：本 TU 的
//   `_Z41__static_initialization_and_destruction_0ii` 读数为工具**按名错配**
//   （ORIG 493 个实例，工具取首个 0x0807ee76 属 PacketTypes TU）⇒ 逐 TU 该件
//   读数不可比；我方 16 条 `std::ios_base::Init`+`__cxa_atexit` 形正确，零改动。
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
            // [l5f 2026-09-11 · §4.3] CSkill+0xc8 已在权威头具名（CSkill.h:121
            // m_fieldC8），裸偏移转具名成员（ORIG disp 折叠形）。
            if (it->second->m_fieldC8 == 0)
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
            // [l5f 2026-09-11 · §4.3] Quest+0xb8 已在权威头具名（Quest.h:444
            // m_fieldB8）；ORIG 无 qp 具名局部（P0）⇒ 表达式直读。
            const int state = quest->m_fieldB8;
            if (state != 0 && state != 0x19)
            {
                continue;
            }

            // 【ORIG +0xc4/+0xd0】两段 RewardItemInfo 奖励向量（当前 Quest.h
            // [l5f 2026-09-11 · §4.3] 已具名：m_titleReward(+0xc4)/
            // m_selectReward(+0xd0)/m_fieldDC(+0xdc)/m_rewardItems(+0xec)。
            {
                std::vector<RewardItemInfo>& v =
                    quest->m_titleReward;
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
                              quest->m_index, it->m_itemIdx);  // 【ORIG asm 08361b7e: quest+0x04】
                        questFail = true;
                    }
                }
            }
            {
                std::vector<RewardItemInfo>& v =
                    quest->m_selectReward;
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
                              quest->m_index, it->m_itemIdx);  // 【ORIG asm 08361b7e: quest+0x04】
                        questFail = true;
                    }
                }
            }
            // 【ORIG +0xdc】collision quest 列表（vector<int>）
            {
                std::vector<int>& v =
                    quest->m_fieldDC;
                for (std::vector<int>::const_iterator it = v.begin();
                     it != v.end(); ++it)
                {
                    if (find_quest(*it) == 0)
                    {
                        cMyTrace trace(__PRETTY_FUNCTION__, 0x27cd, 5);
                        trace("Quest reward collision quest element now found "
                              "(quest: %d)(collision quest: %d)",
                              quest->m_index, *it);  // 【ORIG asm 08361b7e: quest+0x04】
                        questFail = true;
                    }
                }
            }
            // 【ORIG +0xec】collision item 向量（RewardItemInfo）
            {
                std::vector<RewardItemInfo>& v =
                    quest->m_rewardItems;
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
                              quest->m_index, (*it).m_itemIdx);  // 【ORIG asm 08361b7e: quest+0x04】
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
                // [l5f 2026-09-11 · §4.3 报告项] CMonster+0x58/+0x5c 虽已具名
                // （CMonster.h:50/51 m_independentDropCount/m_catchItemCount）但为
                // private：本 TU 无法具名访问（编译实证 2026-09-11 02:45）。
                // 建议属主把二者提为 public 或加 const getter（零布局影响），
                // 本件在此之前保持按 ORIG 偏移直读（ORIG 无 mp 具名局部，P0）。
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
}