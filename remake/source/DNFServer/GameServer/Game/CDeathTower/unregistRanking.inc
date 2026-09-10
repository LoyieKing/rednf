void CDeathTowerRanking::unregistRanking(unsigned int characNo)
{
    // ORIG 08468552 T：for i in 0..4: _getRankingTable(i).erase(characNo)。
    // ORIG 0x8468561：体内 call _getRankingTable@0x8469d90（跨 TU W 符号）；
    // 循环变量 int（0x8468583 cmpl $0x4 + setle 有符号实证）。
    for (int i = 0; i <= 4; ++i)
        _getRankingTable(i)->erase(characNo);
