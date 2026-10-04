// ----------------------------------------------------------------------------
// ORIG T 0x08664c50 _ZN5CUser15calc_quest_statER15_Additioal_info：
// 精灵药水表→count_specific_items 统计持有；UserQuest+4 的 CQuestClear 经
// enumQuestClear(_fnStatQuestClear, this, ids, rewards) 收集可完成奖励；
// complete_reward_list 按持有补齐；逐项 apply_elixir_item 到 _Additioal_info。
// ----------------------------------------------------------------------------
// [b28d-27] DIFF N82：ORIG 0x08664c50；p167 CLEAR（18/18），残差为帧/槽分配，
// 语义闭合。
// [b28d-solo-CUserMissingA2] 修复⇒AE：qc 表达式内联进 enumQuestClear 实参
// （ORIG 0x08664c9e `lea 0x4(%eax),%edx`，无具名局部）⇒ 消 -0x14 槽、全帧槽号
// 与 ORIG 对齐，DIFF→AE（p167 CLEAR 18/18）。
void CUser::calc_quest_stat(_Additioal_info& ai)
{
    std::vector<int> elixirIds;
    for (int i = 0; i <= 7; ++i)
        elixirIds.push_back(elixir_item_id[i]);

    std::vector<std::pair<int, int> > ownCounts;
    count_specific_items(elixirIds, ownCounts);

    std::vector<std::pair<int, int> > rewards;
    // ORIG 0x08664c9e：`call getCurCharacQuestR; lea 0x4(%eax),%edx` 直接内联进
    // 实参装载（无 qc 具名局部）⇒ 表达式内联，避免多占 -0x14 槽使全帧深 4。
    getCurCharacQuestR()->m_cleared.enumQuestClear(_fnStatQuestClear, this, &elixirIds, &rewards);

    complete_reward_list(rewards, ownCounts);

    for (size_t i = 0; i < rewards.size(); ++i)
    {
        apply_elixir_item(ai, rewards[i]);
    }
}