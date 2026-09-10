int CDeathTowerRanking::checkRenewMyRecord(unsigned int towerIdx,
                                           unsigned int key,
                                           unsigned int stage,
                                           unsigned int playTime)
{
    // ORIG 084682fe T：m_bestTables[towerIdx].find(key)；无记录 → -1；
    // 否则构造临时纪录(stage,playTime)，临时 > 已有 → 1，否则 0。
    // [b28d-D13] 逐条对齐 ORIG：①开头 call _getBestRecordTable(towerIdx)（0x084682fe
    // 首块 `mov 0xc(%ebp),%eax;mov %eax,0x4(%esp);mov 0x8(%ebp),%eax;call`，结果存
    // -0xc）⇒ 删除原 towerIdx>=5 越界保护（P140/P171：ORIG 无检查）与 m_bestTables
    // 直接寻址；②tmp 声明在 find/比较之后（ORIG 构造在 0x08468390 后）；③赋值序
    // playTime(0x18)→-0x38 先于 stage(0x14)→-0x3c。
    BestMap* table = _getBestRecordTable(towerIdx);
    BestMap::const_iterator it(table->find(key));
    if (it == BestMap::const_iterator(table->end()))
        return -1;
    stBestRecord_t tmp;
    tmp.m_playTime = playTime;
    tmp.m_stage = stage;
    return tmp > it->second ? 1 : 0;  // ORIG: stBestRecord_t::operator>(tmp, existing)
