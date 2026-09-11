int BlueMarble::setDiceNumber()
{
    m_diceNumber = (unsigned int)m_pRands[0].randInt() %
                    (unsigned int)m_pScript->m_info.m_diceMax +
                m_pScript->m_info.m_diceMin;
}