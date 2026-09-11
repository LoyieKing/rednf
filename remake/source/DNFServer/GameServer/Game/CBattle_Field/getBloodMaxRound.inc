int CBattle_Field::getBloodMaxRound()
{
    if (m_pDungeon != 0)
    {
        return ((CDungeon_8a0_proxy*)m_pDungeon)->m_8a0;
    }
    return 0;
}