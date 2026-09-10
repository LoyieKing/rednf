void ProcStage::addCreatedMapMonster(_MapMonsterSummary& summary)
{
    // P0：ORIG 无 seq 槽（0x812e177 movzwl 后寄存器直存/lea 自增），++ 后缀单读双用。
    summary.m_seq = m_field14++;
    m_createdMapMonsters.insert(std::make_pair(summary.m_seq, summary));
