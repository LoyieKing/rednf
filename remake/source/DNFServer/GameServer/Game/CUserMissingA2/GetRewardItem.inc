bool CHARAC_LOAD_MERCENARY::GetRewardItem(
    CUser* user, competition_reward_result& result, Inven_Item& item,
    std::string& msg)
{
    if (user == 0)
        return false;

    item.reset();
    msg = MercenaryHardCodingSettingValue::MERCENARY_BASE_POSTAL_MESSAGE;
    if (!CheckCompetitionAreaIndex(static_cast<char>(m_field30), m_level))
        return false;

    // ORIG 0x081b32d8：`call operator[]; movzwl 0x14(%eax),%eax` 无 pArea 具名槽
    // （P157/P183）⇒ 内联表达式直读。
    if (G_CDataManager()->m_mercenarySystem
            .m_vec40[static_cast<int>(static_cast<char>(m_field30))]
            .m_areaLevel == 0)
        m_field30 = 0;

    static CMTRand probabilityRewardItem;
    int dropRate = CalcItemDropRate(user);
    unsigned long range = 10000;
    static int itemDropDice;
    itemDropDice = probabilityRewardItem.randInt(range);
    if (user->m_field6ef68 != 0)
        itemDropDice = 0;
    // ORIG 0x081b33c6：`mov itemDropDice,%eax; cmp dropRate,%eax; jl <继续>`
    // ⇒ 源码比较方向为 itemDropDice < dropRate（P196 折痕：jg↔jl 同义）。
    if (itemDropDice >= dropRate)
        return false;

    const stMercenaryCompetitionArea* pArea2 = &G_CDataManager()
        ->m_mercenarySystem.m_vec40[static_cast<int>(static_cast<char>(m_field30))];
    unsigned long groupRange =
        static_cast<unsigned long>(pArea2->m_groupIdSum);
    static int groupDice;
    groupDice = probabilityRewardItem.randInt(groupRange);
    if (user->isGMUser())
    {
        static char buffer[0x100];
        sprintf(buffer, "dice: %d/%d\n", groupDice, pArea2->m_groupIdSum);
        SendMessageToCUser(user, buffer);
    }

    // ORIG 0x081b346e/0x081b3475/0x081b347c：groupSum 占 -0x40、groupIdx 占 -0x3c，
    // 且 groupIdx 连续清零两次 ⇒ 声明序 groupSum 在前 + 循环前一次显式重清（P198① 同族）。
    int groupSum = 0;
    unsigned int groupIdx = 0;
    groupIdx = 0;
    while (groupIdx < pArea2->m_groups.size())
    {
        groupSum += pArea2->m_groups[groupIdx].m_groupId;
        if (groupDice <= groupSum)
            break;
        ++groupIdx;
    }
    if (pArea2->m_groups.size() <= groupIdx)
        groupIdx = 0;

    const stMercenaryRewardGroup& group = pArea2->m_groups[groupIdx];
    msg = group.m_name;

    if (!group.m_mobList.empty())
    {
        unsigned long mobRange =
            static_cast<unsigned long>(group.m_field20);
        static int mobDice;
        mobDice = probabilityRewardItem.randInt(mobRange);
        int mobIdx = FindRelativeProb<int>(group.m_mobList, mobDice);

        std::map<int, std::pair<int, int> >::iterator it =
            G_CDataManager()->m_independentDrop.m_map24.find(mobIdx);
        if (it != G_CDataManager()->m_independentDrop.m_map24.end())
        {
            const std::vector<IndependentDropListInfo>& dropLists =
                G_CDataManager()->m_independentDrop.m_vec0;
            // [b28d-solo-CUserMissingA2②] ORIG 0x081b3636 段 `call size; mov
            // ebx,eax; call ptEv; mov eax,[eax+0x4]; cmp ebx,eax; setle; test;
            // je <主流程>`（有符号 size<=first 早返 return-0 岛）⇒ 反向早返形。
            if ((int)dropLists.size() <= it->second.first)
                return false;
            const std::vector<std::pair<int, int> >& list =
                dropLists[it->second.first].m_list04;
            unsigned long dropRange =
                static_cast<unsigned long>(it->second.second);
            // ORIG 0x081b3648/0x081b3673：dropDice 落栈槽 -0x2c（`mov %eax,-0x2c(%ebp)`
            // / `mov -0x2c(%ebp),%eax`），且 nm 无 .bss 符号 ⇒ 原码是**普通局部
            // int**（其余 itemDropDice/groupDice/mobDice/itemDice 才是函数级 static）。
            int dropDice = probabilityRewardItem.randInt(dropRange);
            int sum = 0;
            for (unsigned int j = 0; j < list.size(); ++j)
            {
                sum += list[j].second;
                // [b28d-solo-CUserMissingA2②] 复验：写 `sum >= dropDice` 反致块序
                // 错位（mnem ratio 0.9605→0.7289）⇒ 维持 ORIG 语义等价形。
                if (dropDice <= sum)
                {
                    item.m_addInfo = list[j].first;
                    item.m_addInfo2 = 1;
                    G_CDataManager()->m_itemList->create_item(
                        ENUM_ITEM_CREATE_TYPE_1, item, 1);
                    result.m_8 = 1;
                    return true;
                }
            }
            // ORIG 0x081b36f6 独立 return-false 岛（mob 循环未命中）与 0x081b3700
            // （it==end）分列两岛；显式 return 阻止 4.4 共享出口合并。
            return false;
        }
        return false;
    }

    // [b28d-solo-CUserMissingA2②] ORIG 0x081b3786 `mov eax,[ebp-0x38]; add
    // eax,0x14; call _ZNKSt6vectorISt4pairIiiESaIS1_EE5emptyEv; xor $1; test;
    // je <return-0>` = item 分支的正向 empty 包裹（p167 MISSING 该 const empty
    // 调用的根因；原实现直接进 for，语义上空表时同样返回 false 但少调用点）。
    if (!group.m_itemList.empty())
    {
        unsigned long itemRange =
            static_cast<unsigned long>(group.m_field24);
        static int itemDice;
        itemDice = probabilityRewardItem.randInt(itemRange);
        int sum2 = 0;
        for (unsigned int k = 0; k < group.m_itemList.size(); ++k)
        {
            // [b28d-solo-CUserMissingA2 真 bug 修正] ORIG 0x081b376c/0x081b3790：
            // m_itemList 的 pair 语义为 (概率, 道具id)——累加读 +0（first），命中取
            // +4（second）赋 m_addInfo（与 m_mobList 的 (id, prob) 相反）。原实现
            // 两处互换 ⇒ 概率累加错、道具 id 取错。
            sum2 += group.m_itemList[k].first;
            if (itemDice <= sum2)
            {
                item.m_addInfo = group.m_itemList[k].second;
                item.m_addInfo2 = 1;
                G_CDataManager()->m_itemList->create_item(
                    ENUM_ITEM_CREATE_TYPE_1, item, 1);
                result.m_8 = 1;
                return true;
            }
        }
        // ORIG 0x081b37fa `jmp <0x081b3803>` + 0x081b3803 岛：循环未命中与 empty
        // 失败各落独立 return 岛。4.4 尾块合并使循环退出直落外层 return-false 块
        // （少 1 条 jmp）。goto 尾标签变体实测可补该 jmp 但整帧槽号平移 8
        // （归一化 diff 5→19 处，净负，已回滚）⇒ N112/N121 封顶。
        return false;
    }
    return false;
