// ----------------------------------------------------------------------------
// ORIG T 0x0812d540（insertSummonObject.md 全文）：push_back(&so) +
//   map.insert(make_pair(so.m_seq, &so))。
// ----------------------------------------------------------------------------
void ProcStage::ProcSummonObjectMs::insertSummonObject(_SummonObjectMs& so)
{
    m_summons.push_back(&so);
    m_summonMap.insert(std::make_pair(so.m_seq, &so));
}