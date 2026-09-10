bool CEnchanter::IsEqualMonsterCad()
{
    CItem* card = FindMonsterCard(reinterpret_cast<CItem*>(m_field40));
    if (card == 0)
        return 0;
    if (card->get_index() != reinterpret_cast<CItem*>(m_field3c)->get_index())
        return 0;
    return 1;
