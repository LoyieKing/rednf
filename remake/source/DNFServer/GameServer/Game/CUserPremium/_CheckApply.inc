bool CUserPremium::_CheckApply(int type, tm t)
{
    // ORIG 0x86ae834：结果落 bool 槽 local_9（命中不早退，继续迭代）；
    // mask 每判定独立全链（无 def 缓存）；+0x18 即 tm_wday。
    bool ok = false;
    if (g_SPremiumInfo[type].m_applyDayMask[0] == -1)
        return true;
    int i = 0;
    while (i < 7)
    {
        if (g_SPremiumInfo[type].m_applyDayMask[i] == -1)
            break;
        if (g_SPremiumInfo[type].m_applyDayMask[i] == t.tm_wday)
            ok = true;
        ++i;
    }
    return ok;
