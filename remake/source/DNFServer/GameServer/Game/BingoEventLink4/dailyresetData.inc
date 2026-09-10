void BingoEvent::dailyresetData()
{
    int hourLimit = G_CEnvironment()->m_serverEnvir.m_fatigueTime;  // env+0x37c
    long curSec = GlobalData::s_systemTime_.getCurSec();
    int day = m_bingo->getToday(curSec, hourLimit);
    if (day > 0 && day < 0x20)
    {
        m_bingo->resetBoard(day);
        m_bingo->resetQuiz(day);
        cMyTrace trace(__PRETTY_FUNCTION__, 0x50, 0);
        trace("BingoEvent Day(%d)", day);
    }

    // [b28d-D9] ORIG 0x080c9d82 同样为 BOOST_FOREACH 展开（deref 后取 pair.second
    // 落 -0xc 槽，判空后三步操作）。
    typedef std::map<unsigned int, CUser*>::value_type UserPair;
    BOOST_FOREACH(UserPair& pr, G_CGameManager()->m_userByAccId)
    {
        CUser* user = pr.second;
        if (user != 0)
        {
            static_cast<BingoData*>(user->getBingoData())->clear();
            user->sendBingoData();
            user->sendDecorationObjectCnt(0);
        }
    }
