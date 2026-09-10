void CVillageMonster::OnRunVillageMonster(std::vector<MapArea>* areas, int idx)
{
    m_field10 = 0;
    if (m_state != 0)
        return;
    if (idx % 3 != m_field0c)
        return;
    int oldX = m_field04;
    int oldY = m_field08;
    int offset = get_rand_int(MONSTER_MOVE_PIXEL) - 0x96;
    m_field04 = m_field04 + offset;
    offset = get_rand_int(MONSTER_MOVE_PIXEL) - 0x96;
    m_field08 = m_field08 + offset;
    pvp_assault::IsIncludeVirtualMoveAreas(areas, m_field04, m_field08,
                                           m_field04, m_field08, 0x14);
    if (abs(m_field04 - oldX) + abs(m_field08 - oldY) <= 0x31)
    {
        m_field04 = oldX;
        m_field08 = oldY;
    }
    else
        m_field10 = 1;
