// ORIG 0x0826d91a：按列求和后写回 m_probs[row][col]——row 声明在外层、循环后泄漏
// 值 11（ORIG 写回段载 -0x8 运行时形 shl3/sub/add，非 0x61 常量折叠，P171 照抄）。
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
}