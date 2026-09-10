void CActionPointManager::GM_Set(CUser& user, unsigned int value)
{
    // b28d-L3: DIFF/N82 — ORIG 0x082... push ebx + sub 0x44 vs 我方 sub 0x58（帧差/callee-saved 缺失，N82）
    m_flag00 = 1;
    m_sig.m_apSum = 0;
    m_prevApSum = 0;
    for (int row = 0; row <= 299; ++row)
    {
        _ActionPoint& rec = m_sig.m_actionPoints[row];
        for (int slot = 0; slot <= 4; ++slot)
        {
            const signed char entry =
                static_cast<signed char>(rec.m_days[slot]);
            if (entry > 0)
            {
                rec.m_days[slot] = static_cast<unsigned char>(-entry);
            }
        }
    }
    for (int row = 0;
         row <= 299 && m_sig.m_apSum < value; ++row)
    {
        _ActionPoint& rec = m_sig.m_actionPoints[row];
        for (int slot = 0;
             slot <= 4 && m_sig.m_apSum < value; ++slot)
        {
            const signed char entry =
                static_cast<signed char>(rec.m_days[slot]);
            if (entry < 0)
            {
                m_prevApSum = m_sig.m_apSum;
                const unsigned char restored =
                    static_cast<unsigned char>(-entry);
                rec.m_days[slot] = restored;
                m_sig.m_apSum += static_cast<signed char>(restored);
                const _ActionGroupIndex group =
                    static_cast<_ActionGroupIndex>(rec.m_groupIndex);
                CUserProc::SendActionClear(user, rec.m_actionNo, group, true,
                                           ENUM_PACKETCLASS_0);
                CUserProc::CheckAndSendToMessageAllUser(
                    m_prevApSum, m_sig.m_apSum, user.getCurCharacName());
            }
        }
    }
