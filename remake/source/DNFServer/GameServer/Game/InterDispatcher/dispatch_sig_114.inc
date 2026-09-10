int Inter_MonitorDeathTowerRanking::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    // ORIG 084d3331/084d3320：循环界 `cmp $0x4/%eax; setbe` ⇒ 原码 unsigned `<=` 形。
    // ORIG 084d32c6/084d32e0：条件与 registRanking 第 3 实参各自重物化取值（value 无槽，
    //   留 %edi 跨 getCurCharacNo call）⇒ §P0 内联，不设具名 value。
    // ORIG 084d3299 `mov 0x10(%ebp),%eax; mov %eax,-0x24` ⇒ 包基址具名局部。
    Inter_MonitorDeathTowerRankingPktView* pPkt =
        reinterpret_cast<Inter_MonitorDeathTowerRankingPktView*>(data);
    for (unsigned int i = 0; i <= 4; ++i)
    {
        for (unsigned int j = 0; j <= 3; ++j)
        {
            if (pPkt->m_ranks[i * 4 + j + 4] != 0)
            {
                static_cast<WongWork::CDeathTowerRanking*>(
                    WongWork::CDeathTowerRanking::getRankTable(j + 1))
                    ->registRanking(
                        i, user->getCurCharacNo(),
                        pPkt->m_ranks[i * 4 + j + 4]);
            }
        }
    }
    return 0;
