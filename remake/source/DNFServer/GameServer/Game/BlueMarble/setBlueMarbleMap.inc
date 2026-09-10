int BlueMarble::setBlueMarbleMap()
{
    m_blueMarbleMap = (unsigned int)m_pRands[1].randInt() %
                    (unsigned int)m_pScript->getMapTotalCount() +
                1;
