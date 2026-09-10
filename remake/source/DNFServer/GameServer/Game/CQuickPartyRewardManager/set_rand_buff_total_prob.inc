void CQuickPartyRewardManager::set_rand_buff_total_prob()
{
    // 声明序=槽序（col=-0x10/total=-0xc/row=-0x8）；row 泄漏值 11 后写回
    int col;
    int total;
    int row;
    for (col = 0; col < 7; ++col)
    {
        total = 0;
        for (row = 0; row < 11; ++row)
            total += m_probs[row][col];
        m_probs[row][col] = total;
    }
