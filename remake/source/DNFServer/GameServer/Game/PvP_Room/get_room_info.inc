void PvP_Room::get_room_info(PVP_ROOM_INFO* out)
{
    out->title = m_roomName;
    out->byte4 = m_mode8;
    out->short6 = m_requestedMapNo;
    out->byte8 = m_isExistPassword;
    out->passwd = m_password;
}