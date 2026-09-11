// ORIG T 0x0812daec（compare.md 全文）：return lhs->m_ms < rhs->m_ms。
bool ProcStage::ProcSummonObjectMs::compare(const _SummonObjectMs* lhs,
                                            const _SummonObjectMs* rhs)
{
    return lhs->m_ms < rhs->m_ms;
}