// [b28d-solo-EmblemCompoundServer] GetEmblemRandomIndex 判词（ORIG 0x081919e8，77 insn）：
// IDENTICAL/AE。修法：listIdx 改 `int listIdx = 0`（ORIG movl 清零 ⇒ P10）；
// get_rand_int 结果具名 int 局部（槽 -0x10）；empty 后重发 m_result[grade]
// （P157 逐使用点重读）+ `if(!m_selectable)` 取反形（P168a/P292，xor $1）。
int EmblemCompoundServer::GetEmblemRandomIndex(int grade,
                                               unsigned short selectIdx)
{
    ARAD::SCRIPT::EmblemCompound::Result* result = m_result[grade];
    // ORIG：rand(1000) < elem+2(m_itemIndex) → 取 +0(m_listIdx)，否则 +4(m_itemCount)
    int listIdx = 0;
    int randValue = get_rand_int(1000);
    if (randValue < (int)(short)result->m_itemIndex)
        listIdx = result->m_listIdx;
    else
        listIdx = result->m_itemCount;

    std::vector<unsigned long>& list = m_list[listIdx];
    if (list.empty())
        return 0;
    // ORIG +0x8f 重发 m_result[grade]（P157 逐使用点重读），取反形 xor $1。
    if (!m_result[grade]->m_selectable)
        return (int)list[get_rand_int((int)list.size())];
    return (int)list[selectIdx];
}