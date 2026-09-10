void CDeathTowerRanking::unregistBestRecord(unsigned int characNo)
{
    // ORIG 08468b4c T：for i in 0..4: _getBestRecordTable(i).erase(characNo)。
    // ORIG 0x8468b4c：体内 call _getBestRecordTable@0x8469dae（跨 TU W）；int 循环（setle）。
    for (int i = 0; i <= 4; ++i)
        _getBestRecordTable(i)->erase(characNo);
