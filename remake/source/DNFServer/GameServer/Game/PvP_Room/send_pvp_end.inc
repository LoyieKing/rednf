int PvP_Room::send_pvp_end()
{
    m_isEndPvpBattle = 1;
    if (G_GameWorld()->GetChannelType() != 6)
    {
        update_pvp_point();
    }
    pvp_reward();
    return 1;
}