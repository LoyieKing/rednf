// ============================================================================
// clearScript —— ORIG T 0x088b1926：清 cost map → begin/end 各取一次后遍历
// delete Result* → 清 result map。不清理 m_list（ORIG 如此）。
// ============================================================================
void EmblemCompound::clearScript()
{
    m_cost.clear();
    std::map<int, Result*>::iterator it = m_result.begin();
    std::map<int, Result*>::iterator endIt = m_result.end();
    for (; it != endIt; ++it)
        ::operator delete(it->second);
    m_result.clear();
}