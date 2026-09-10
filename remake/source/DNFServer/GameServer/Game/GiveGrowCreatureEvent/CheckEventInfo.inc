int GiveGrowCreatureEvent::CheckEventInfo(CUser& user)  // 0x081b6e7a
{
    int job = user.get_charac_job();
    int now = GlobalData::s_systemTime_.getCurSec();
    if (!G_CDataManager()->isAvailableGiveGrowCreatureEventJob(job, now))
        return 3;

    std::vector<GrowthCreatureEvent::eventCharacInfo>* mine =
        static_cast<std::vector<GrowthCreatureEvent::eventCharacInfo>*>(
            user.getGrowthCreatureEventdata());
    // P171：ORIG 无 `mine == 0` 守卫（只判 empty）。
    if (mine->empty())
        return 0;

    // P13/P28：ORIG 循环前一次求值 end() 存具名局部、增量段用后置 `it++`（ppEi）。
    std::vector<GrowthCreatureEvent::eventCharacInfo>::iterator it = mine->begin();
    std::vector<GrowthCreatureEvent::eventCharacInfo>::iterator itEnd = mine->end();
    while (it != itEnd)
    {
        if ((*it).m_job == job)
            return 3;                                   // 该职业已领过
        it++;
    }
    return 0;
