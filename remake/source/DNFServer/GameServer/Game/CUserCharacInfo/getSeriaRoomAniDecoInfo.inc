seriaRoom_AniDeco CUserCharacInfo::getSeriaRoomAniDecoInfo()
{
    if (m_selected)
        return CUR->m_seriaRoomAniDeco;
    seriaRoom_AniDeco result;
    result.m_field2 = -1;
    return result;
