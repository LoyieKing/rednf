bool CParty::IsCompleteCardSelectOfMembers(eClearRewardCardType_t type)
{
    // b28d-solo-CParty：ORIG @0x085b489c 的 +0x97 为 4.1 空 then 占位 nop 岛
    // （je 落 nop、body 尾 jmp 跳 +0x98），P223 复现形 = 否定条件 goto 尾空标签。
    for (int i = 0; i <= 3; ++i)
    {
        if (m_member[i].m_pUser == 0)
            goto L_next;
        if ((int)type == 2)
        {
            for (int j = 0; j < 2; ++j)
            {
                if ((unsigned char)m_field204[j][i] == 0xff)
                    return false;
            }
        }
        else if ((unsigned char)m_field204[(int)type][i] == 0xff)
        {
            return false;
        }
L_next:;
    }
    return true;
