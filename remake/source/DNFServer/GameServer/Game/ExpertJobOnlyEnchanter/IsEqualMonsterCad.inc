// ---------------------------------------------------------------------------
// IsEqualMonsterCad（报告 IsEqualMonsterCad.md，ORIG T 0x849ca6a sz 0x5b）：
// FindMonsterCard(m_field40) 结果的 get_index 与 m_field3c 的 get_index 相等。
// ---------------------------------------------------------------------------
bool CEnchanter::IsEqualMonsterCad()
{
    CItem* card = FindMonsterCard(reinterpret_cast<CItem*>(m_field40));
    if (card == 0)
        return 0;
    if (card->get_index() != reinterpret_cast<CItem*>(m_field3c)->get_index())
        return 0;
    return 1;
}