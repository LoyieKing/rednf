void ProcStage::ProcSummonObjectMs::insertSummonObject(_SummonObjectMs& so)
{
    m_summons.push_back(&so);
    m_summonMap.insert(std::make_pair(so.m_seq, &so));
