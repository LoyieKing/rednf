// ============================================================================
// ORIG 0x08284d08（报告 isCardSelectComplete.md）：members 空 → 0。
// type==2（双卡型）：2x2 全部槽的 m_flag08（char 引用，std::find<char> 实证
// _ZSt4findI...cET_S8_S8_RKT0_）都必须出现在 members 中；否则查该行两槽。
// 全命中 → 1。
// [b28d-N82] fresh 复验 137/139：残差 = 帧/槽平移 + find 块尾 jmp 岛（N104）。
// [b28d-solo-CTournamentDungeonReward] DIFF 137/139：修具名 found 迭代器后求值序
// 与槽位（end -0x2c / begin -0x28 / find -0x30 / end -0x24；else 支 -0x20/-0x1c/
// -0x34/-0x18）与 ORIG 逐槽全等；残 = N18a×2；p167 11=11 CLEAR。
// [SweepCTournament 2026-09-09] 复核 137/139：残 = N18a 中转×2；p167 11=11 CLEAR
//（empty/find/end/begin/eq 全命中）⇒ N 封顶。
// ============================================================================
bool CTournamentDungeonReward::isCardSelectComplete(
    ENUM_TOURNAMENT_REWARD_CARD_TYPE type, const std::vector<int>& members)
{
    if (members.empty())
        return false;

    if ((int)type == 2)
    {
        for (int i = 0; i < 2; ++i)
        {
            for (int j = 0; j < 2; ++j)
            {
                // ORIG 求值序 = &flag → end() → begin() → find → 比较侧 end()；
                // 具名 found 使 4.4 的 find 结果先落槽再调 end（P293 同族）。
                std::vector<int>::const_iterator found =
                    std::find(members.begin(), members.end(),
                              m_rewards[i][j].m_flag08);
                if (found == members.end())
                    return false;
            }
        }
    }
    else
    {
        for (int j = 0; j < 2; ++j)
        {
            std::vector<int>::const_iterator found =
                std::find(members.begin(), members.end(),
                          m_rewards[(int)type][j].m_flag08);
            if (found == members.end())
                return false;
        }
    }
    return true;
}