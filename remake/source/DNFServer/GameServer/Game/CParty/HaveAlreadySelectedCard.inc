// ============================================================================
// 卡片选择（ORIG 0x85b401c..0x85b494e）。m_field204[type][slot]：
// dungeon_reward_card 表，0xff=未选。
// ============================================================================
bool CParty::HaveAlreadySelectedCard(char a, eClearRewardCardType_t type)
{
    // ORIG 0x085b401c：guard 以 jle 直跳 body、假块(mov0;jmp ret)直落前（P59 双向布局），
    // 且与尾 return false 不合并（两处独立 mov0 块）。改 type>1 早返经 if-return 拆离。
    if ((int)type > 1)
        return false;
    {
        for (int i = 0; i <= 3; ++i)
        {
            if (m_field204[(int)type][i] == a)
                return true;
        }
    }
    return false;
}