bool CMission_hereafter_winning_count::_canUpdate(
    const MissionClearCondition_Parameter& param) const
{
    std::bitset<32> bits(param.m_victory);
    if (is_Victory_in_a_row(bits, 1))
        return true;
    return false;
