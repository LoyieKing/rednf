// ORIG T 0x084601da：排行榜展示包。个人最优/榜首记录段委托给
// CDeathTowerRanking 的两个 static 表访问 + 记录序列化函数。
void CDeathTower::CPacketHandler::makeDisplayRangking(unsigned int tableType,
                                                      unsigned int key,
                                                      unsigned int stage,
                                                      unsigned int clearTime,
                                                      bool flag,
                                                      unsigned int dungeonIdx,
                                                      unsigned int playTime)
{
    put_header(0, 0x90);
    put_int((int)clearTime);                       // ebp+0x18 → param_4
    put_int((int)stage);                           // ebp+0x14 → param_3
    put_byte(static_cast<unsigned int>(flag));     // ORIG movzbl -0xc 直零扩（bool slot 信任）
    put_int((int)playTime);                        // ebp+0x24 → param_7
    // ORIG 0x084601da：两记录段各自重调 makeDungeonIdx2TowerIdx +
    // getRankTable（P48 无跨句缓存；this 经 %esi、towerIdx 经 %ebx 跨 call）。
    {
        // P179：ORIG towerIdx 跨 getRankTable call 经 %ebx（push %ebx 由此）。
        // N96 清绑后按 P226 裸语句表达式升格试形。
        // getRankTable 结果 %eax 直传 this 实参槽（命名指针会强制落槽回读）。
        ((CDeathTowerRanking*)CDeathTowerRanking::getRankTable(tableType))
            ->makeMyBestRecordPacket(
                ({ CDeathTowerRanking::makeDungeonIdx2TowerIdx(dungeonIdx); }),
                key, *this);                                     // ORIG 0x08468b8e
    }
    {
        ((CDeathTowerRanking*)CDeathTowerRanking::getRankTable(tableType))
            ->makeTopRankerRecordPacket(
                ({ CDeathTowerRanking::makeDungeonIdx2TowerIdx(dungeonIdx); }),
                *this);                                          // ORIG 0x08468c68
    }
    finalize(true);
}