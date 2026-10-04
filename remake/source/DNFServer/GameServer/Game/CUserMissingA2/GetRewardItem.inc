// ----------------------------------------------------------------------------
// 22. ORIG T 0x081b3276 sz 0x59d。物品奖励（长流程）：
// 重置 item、msg 置兜底邮政消息；area 校验；m_areaLevel==0 归零 m_field30；
// itemDropDice=randInt(10000)（GM 用户强制 0）；>=dropRate 失败；
// groupDice=randInt(m_groupIdSum)；GM 打印；按组累加选组（越界回绕 0）；
// msg=组名；mobList 非空走 mob 分支（map24 查独立掉落表），否则 item 分支
// （m_itemList 累加）；命中写 item+create_item+result.m_8=1。
// ----------------------------------------------------------------------------
// [b28d-27] DIFF（cv 旋钮待修）：ORIG 0x081b3276；p167 差集显示 ORIG 用 const
// 迭代器/const operator[]（_ZNKSt6vector...ixEj ×4、_ZNKSt17_Rb_tree_iterator...ptEv ×3、
// _ZNKSt6vector...emptyEv），我方用非 const 版（+4/+3/+2）⇒ N89/P96 封顶。
// [b28d-solo-CUserMissingA2] 修复（真语义 bug）：m_itemList 的 pair 语义为
// (概率, 道具id)——`sum2 += .first`、命中 `item.m_addInfo = .second`
// （ORIG 0x081b376c `mov (%eax),%eax` / 0x081b3790 `mov 0x4(%eax),%eax` 实证；
// 原实现两处互换 ⇒ 概率累加错、道具 id 取错）。另：pArea 表达式内联（消 -0x48
// 具名槽）、`itemDropDice >= dropRate` 比较方向翻正（ORIG `cmp dropRate,%eax; jl`）。
// [b28d-solo-CUserMissingA2②] 本波补齐两处真缺口 ⇒ **p167 50/50 CLEAR**：
// ① item 分支正向 empty 包裹 `if (!group.m_itemList.empty())`（ORIG 0x081b3786
//    `mov eax,[ebp-0x38]; add eax,0x14; call _ZNKSt6vectorISt4pairIiiESaIS1_EE5emptyEv;
//    xor $1; test; je <return-0>` 实证——原实现直接进 for 少一次 const empty）；
// ② dropLists 越界守卫改早返形 `if ((int)dropLists.size() <= it->second.first)
//    return false;`（ORIG 0x081b3636 `call size; mov ebx,eax; call ptEv; mov
//    eax,[eax+0x4]; cmp ebx,eax; setle; test; je <主流程>` 实证）。
// [b28d-solo-CUserMissingA2③] 本轮重核（dual_rank gap 6）⇒ 修真差异 + gap 6→1：
//   ① **dropDice 误建为函数级 static**：ORIG 0x081b3648 `mov %eax,-0x2c(%ebp)` /
//      0x081b3673 `mov -0x2c(%ebp),%eax` 是栈槽，且 nm 的 6 个 .bss 局部符号
//      （itemDropDice/groupDice/mobDice/itemDice/buffer/probabilityRewardItem）**不含**
//      dropDice ⇒ 原码是普通局部 int（原实现多写一个 .bss 符号且读写形态不同）。
//   ② groupSum/groupIdx 声明序翻正（ORIG 0x081b346e 先清 -0x40=groupSum、后清
//      -0x3c=groupIdx；原实现槽互换）。
//   ③ 复现 ORIG 形态：groupIdx 重复清零（0x081b347c）、mob/item 两分支在循环后补
//      显式 `return false` 使 4.4 分出独立 return-false 岛（ORIG 0x081b36f6/0x081b3700
//      /0x081b37fa/0x081b3803 四岛；原实现共享单一出口，少 5 条）。
//   残差 = 1 条 `jmp` 中继岛 + 尾部对齐 `nop`（§6.1 豁免）⇒ N112 封顶；
//   p167 50/50（MISSING/EXTRA 仅 CMTRand C1↔C2 同址别名，P262）。
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
    if (user->m_forceItemDrop != 0)
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
}