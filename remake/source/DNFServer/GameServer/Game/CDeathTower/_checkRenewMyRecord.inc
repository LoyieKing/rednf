int CDeathTower::_checkRenewMyRecord(unsigned int dungeonIdx, unsigned int memberCnt, unsigned int a, unsigned int b, unsigned int c)
{
    // [b28d-D13] P0 清绑（N96）：ORIG 0x846806d mov %eax,%ebx 为纯 register 存储建议
    // 可复现的 callee-saved 升格（P231），无 asm 绑定。
    register unsigned int towerIdx =
        WongWork::CDeathTowerRanking::makeDungeonIdx2TowerIdx(dungeonIdx);
    return ((WongWork::CDeathTowerRanking*)WongWork::CDeathTowerRanking::getRankTable(
                memberCnt))->checkRenewMyRecord(towerIdx, a, b, c);
