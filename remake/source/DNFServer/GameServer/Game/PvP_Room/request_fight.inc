bool PvP_Room::request_fight(CUser* user)
{
    if ((unsigned char)m_battleEndFlag != 0)
    {
        return 1;
    }
    int iVar2 = get_user_seat(user);
    if (iVar2 < 0)
    {
        return 0;
    }
    m_relayMgr.OnRequestFight(iVar2);
    return 1;
}