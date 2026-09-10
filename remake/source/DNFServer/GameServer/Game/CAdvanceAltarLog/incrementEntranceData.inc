void CAdvanceAltarLog::incrementEntranceData(unsigned int stageIndex,
                                             unsigned int enterDelta,
                                             unsigned int successDelta,
                                             unsigned int starDelta)
{
    Guard<Mutex> guard(&m_mutex);
    std::map<unsigned int, STAdvanceAltarEntranceData>::iterator it =
        m_entrances.find(stageIndex);
    // [b28d-E03 2026-09-10] 块序对齐（ORIG 0x08156c95 `je +0xc1`：未命中装配块
    //   fall-through 在前、命中累加块 jump 在后）⇒ 条件按 `it == end` 正极性书写，
    //   装配块为 then、累加块为 else（P59/P222 双向布局）。
    if (it == m_entrances.end())
    {
        STAdvanceAltarEntranceData data;                // 栈装配（ORIG local_28..）
        data.m_stageIndex = stageIndex;
        data.m_enterCount += enterDelta;
        data.m_successCount += successDelta;
        data.m_obtainStar += starDelta;
        // ORIG operator[] 逐 uint 写回（data[0]=键重复，等价整结构赋值）。
        m_entrances[stageIndex] = data;
    }
    else
    {
        it->second.m_enterCount += enterDelta;
        it->second.m_successCount += successDelta;
        it->second.m_obtainStar += starDelta;
    }
