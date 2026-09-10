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
    ((WongWork::CQuestClear*)((char*)getCurCharacQuestR() + 4))
        ->enumQuestClear(_fnStatQuestClear, this, &elixirIds, &rewards);

    complete_reward_list(rewards, ownCounts);

    for (size_t i = 0; i < rewards.size(); ++i)
    {
        apply_elixir_item(ai, rewards[i]);
    }
