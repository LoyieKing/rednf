void ProcStage::ProcSummonObjectMs::reset()
{
    m_currentMs = 0;
    m_deadCount = 0;
    m_aliveDelta = 0;
    // b28d-52：ORIG 0x0812d478 begin 结果经 const_iterator 转换构造（P96/P270：
    // `__normal_iteratorIPKP...` 转换 ctor），增量段为后置 ppEi（P28）。
    for (std::vector<_SummonObjectMs*>::const_iterator it = m_summons.begin();
         it != m_summons.end(); it++)
    {
        delete *it;
    }
    m_summonMap.clear();
    m_summons.clear();
