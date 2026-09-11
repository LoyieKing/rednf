void CParty::CompleteCardSelect()
{
    for (int i = 0; i <= 3; ++i)                        // -0x14
    {
        if ((unsigned char)m_field204[0][i] == 0xff)
        {
            for (int z = 0; z <= 3; ++z)                // -0x10
            {
// b28d-solo-CParty: IDENTICAL —— P223 否定 goto 尾空标签复现 +0x97 nop 岛；58=58
                // b28d-49 @0x85b485x：ORIG `call _checkValidUser; xor $0x1,%eax;
                // test %al,%al; jne <continue>` ⇒ 负极性早退（P168a 裸形）；正极性包裹
                // 产 `test;je` 且缺 xor（ndiff 1）。
                if (!_checkValidUser(z))
                    continue;
                if (!IsSelectCard((char)z, eClearRewardCardType_t_0))
                {
                    m_field204[0][i] = (char)z;
                    if ((unsigned char)m_field204[0][i] == 0xff)
                    {
                        LogManager::logFormat(
                            1, "party.cpp",
                            __PRETTY_FUNCTION__, 0x36f8,
                            "INIT_CARD_NUM != dungeon_reward_card_[i]===>> "
                            "dungeon_reward_card_[i] = %d, i = %d, z = %d",
                            (int)(char)m_field204[0][i], i, z);
                    }
                    break;
                }
            }
        }
    }
}