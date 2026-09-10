void CParty::changePartyMemberInfoOnMoveMap(MSG_MOVE_MAP& info, bool firstVisit)
{
    if (firstVisit)                                      // cmpb $0 jne 结束
        return;

    // N96 清绑：ORIG 判空短路 + IsEnterEachMap 结果 mov$1/mov$0 物化岛
    // （%al 直落 test；4.4 恒折 test;jcc 无中转——P168a③b/N96 判词）。
    if (m_battleField.m_pDungeon != 0 &&
        m_battleField.m_pDungeon->IsEnterEachMap())
    {
        changePartyMemberInfo(info, (char)info.m_39);    // 座位号字节 +0x39
    }
    else
    {
        for (int i = 0; i < 4; ++i)                      // cmpl $3/setle
            changePartyMemberInfo(info, (char)i);
    }
