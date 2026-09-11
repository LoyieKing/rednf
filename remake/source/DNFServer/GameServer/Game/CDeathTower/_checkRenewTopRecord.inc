bool CDeathTower::_checkRenewTopRecord(unsigned int dungeonIdx, unsigned int memberCnt, unsigned int a, unsigned int b)
{
    // [b28d-D13] P0 清绑（N96）：ORIG 0x8468034 mov %eax,%ebx，同 _checkRenewMyRecord。
    register unsigned int towerIdx =
        WongWork::CDeathTowerRanking::makeDungeonIdx2TowerIdx(dungeonIdx);
    return ((WongWork::CDeathTowerRanking*)WongWork::CDeathTowerRanking::getRankTable(
                memberCnt))->checkRenewTopRecord(towerIdx, a, b);
}