seriaRoom_AniDeco CUserCharacInfo::getSeriaRoomAniDecoInfo()
{
    if (m_selected)
        return m_selected->m_aniDeco;
    seriaRoom_AniDeco result;
    result.m_field2 = -1;
    return result;
}