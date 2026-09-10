bool CParty::IsExistInvisible()
{
    for (int i = 0; i <= 3; ++i)
    {
        if (_checkValidUser(i))
            // ORIG 0x085b641f：嵌套 if 免 && 物化岛（平铺形在 4.4 产
            // mov$0/test/je/mov$1 五行岛，ORIG 直 jcc）
            if (!m_member[i].m_pUser->IsCurCharacVisible())
                return true;
    }
    return false;
