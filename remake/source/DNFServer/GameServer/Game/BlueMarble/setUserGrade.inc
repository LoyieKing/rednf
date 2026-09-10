int BlueMarble::setUserGrade(int grade)
{
    std::multimap<int, int, std::greater<int> > mm;
    m_users[grade].setGrade(BlueMarbleUserGrade::T_0);
    for (int i = 0; i < m_pScript->m_info.m_playerMax; ++i)
    {
        // ORIG 0x080d7b5d/0x080d7b87：==0 物化 sete %al 后 test;jne 出口（body fall，
        // 范式23 goto）。b28c-1：P0/N96 清绑，P168a② 具名 bool + 正向 goto；
        // 声明前置（P198① 两句式）避免 goto 跨初始化。
        if (m_users[i].getUser() == 0)
            goto NEXT0;
        if (m_users[i].getGrade() == 0)
            goto NEXT0;
        // ORIG 0x080d7bb0..：zone 槽 -0x1c；Ghidra `make_pair<int,int&>` +
        // 无 out-of-line call ⇒ 原码是左值 make_pair（P61/P234），
        // `std::move(zone)` 会多发一次 `call std::move<int>`（+5 条）。
        mm.insert(std::make_pair(m_users[i].getBoardZone(), i));
    NEXT0:
        ;
    }
    std::multimap<int, int, std::greater<int> >::iterator it = mm.begin();
    int curGrade = 0;
    int nextGrade = 1;
    int lastZone = 0;
    while (it != mm.end())
    {
        if (it->first == lastZone)
        {
            lastZone = it->first;
            m_users[it->second].setGrade((BlueMarbleUserGrade::T)curGrade);
        }
        else
        {
            lastZone = it->first;
            m_users[it->second].setGrade((BlueMarbleUserGrade::T)nextGrade);
            curGrade = nextGrade;
            ++nextGrade;
        }
        ++it;
    }
