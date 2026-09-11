// ORIG T 0x081440b2：
//   m_endTick(+0x9ec) = CSystemTime::getCurTickCount(s_systemTime_)；
//   playSec = (m_endTick - m_startTick(+0x9e8)) / 1000（ORIG imul 0x10624dd3
//   有符号除法）；对槽位 0..3：user=get_user(i)，user!=0 且 checkValidUser(i)==1
//   且 isMemberAlive(i) 时 m_dungeonLog.IncrementBossStageEnd(*user,
//   (uchar)m_stageCur, playSec, (uchar)m_difficulty)；恒返 1。
// [b28d-54 N] ORIG 0x081440b2：帧槽整体平移 4（N82/N95）+ bool 槽物化（P197④）；
//   控制流/调用序列全等。
// [b28d-solo-CBossTower] AE（85:85，本波翻正）：CBossPlay.h 头内 inline
//   `isMemberAlive` unsigned char→bool（P292 调用点判据：ORIG 0x0814414d
//   `call; test %al,%al; je` 无 setne ⇒ bool；body 单 movzbl 直返不变）
//   + 去具名 invalid（裸短路条件）⇒ setne 两段消除、DIFF→AE。三 TU
//   （CBossTower/CBossPlay/CBossStage）fresh 复验零回归。
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
}