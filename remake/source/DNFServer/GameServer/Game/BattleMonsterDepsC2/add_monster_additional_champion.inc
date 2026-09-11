// [b28d-solo-BattleMonsterDepsC2] P197④/P225：ORIG `call size; test %eax; sete %al; test %al;
// jne` ⇒ 源写 `size() == 0`（empty() 直跳少 2 条）；`begin()+slot` 不套 static_cast（ORIG
// 直接把 slot 槽址绑 operator+ 第二引用参，cast 会多一条 -0x1c 临时）。
void CBattle_Field::add_monster_additional_champion(std::vector<int>& championList,
                                                    int count)
{
    // ORIG 每轮循环首查 size==0 即退出（首轮亦然，避免空表除零）；empty 检查
    // 必须置于循环体首，不能并入 for 条件（NEW 二进制曾因此首轮漏检）。
    for (int i = 0; i < count; ++i)
    {
        if (championList.size() == 0)
            break;
        const int slot = rand() % championList.size();                     // ORIG 单次 rand
        const int picked = championList[slot];
        championList.erase(championList.begin() + slot);
        map_monster& mob = GetCurrentMapInfo()->m_monsterMap[picked];
        if (mob.m_field13 == 0)                                            // +0x13
        {
            mob.m_roleType = 1;
        }
        else if (mob.m_roleType == 1)
        {
            LogManager::logFormat(
                1, "battle_field.cpp",
                __PRETTY_FUNCTION__,
                0x6bf, "Box Monster is Champion! id(%d)", picked);
        }
    }
}