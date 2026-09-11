bool CMission_winning_count::_canUpdate(
    const MissionClearCondition_Parameter& param) const
{
    std::bitset<32> bits(param.m_victory);
    // ORIG：`call; test %al,%al; je; mov$1; jmp; mov$0` 分支形态（P56），
    // 直 return 表达式被 4.4 折叠删分支。改 if/return true;false 保留 ORIG CFG。
    if (is_Victory_in_a_row(bits, 1))
        return true;
    return false;
}