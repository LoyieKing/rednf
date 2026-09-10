int CBossTower::onFinishStage()
{
    int playSec = 0;
    m_endTick = GlobalData::s_systemTime_.getCurTickCount();
    playSec = (m_endTick - m_startTick) / 1000;

    for (int i = 0; i <= 3; ++i)
    {
        CUser* member = m_party->get_user(i);
        // ORIG 0x0814414d..0x08144159：短路物化岛无 bool 槽；isMemberAlive
        // 独立 `test %al,%al;je` ⇒ 裸条件两段（P0/P226）。
        if (member == 0 || m_party->checkValidUser(i) != 1)
            continue;
        // P47/P59：ORIG 为 `call; test %al,%al; je <循环尾>`（正向包裹、直跳）
        // ⇒ 用 if(f){body}，勿用 if(!f)continue（uchar callee 恒多 sete+test）。
        if (isMemberAlive(i))
        {
            m_dungeonLog.IncrementBossStageEnd(
                *member,
                static_cast<unsigned char>(m_stageCur),
                static_cast<unsigned int>(playSec),
                static_cast<unsigned char>(m_difficulty));
        }
    }
    return 1;
