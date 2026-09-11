void CPartyTelePort::set_teleport_data(unsigned char village, unsigned char area,
                                       short posx, short posy,
                                       unsigned char direction, char state)
{
    m_tp.reset();
    m_tp.m_village = village;
    m_tp.m_areaIndex = area;
    m_tp.m_posx = posx;
    m_tp.m_posy = posy;
    m_tp.m_direction = direction;
    m_tp.m_state = state;
    for (int i = 0; i <= 3; ++i)
    {
        set_teleport_member_state(i, 2);
    }
}