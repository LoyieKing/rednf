void SendingAPC_Info::Send_APC_Info(TOD_Layer const& layer, CUser* user)
{
    Set_APC_Info(layer, m_apcInfo);

    PacketGuard guard(&m_buf);
    user->Send(guard);

    char msg[256];
    sprintf(msg,
            "TOD : setting layer:%d, name:%s, lv:%d, job:%d, grow_type:%d, "
            "guild_name:%s, pvp_grade:%d, creature_name:%s, creature_id:%d",
            (unsigned int)(layer.GetLayer() & 0xffff),
            m_apcInfo.m_userCharacName, m_apcInfo.m_lv, m_apcInfo.m_job,
            m_apcInfo.m_growType, m_apcInfo.m_guildName, m_apcInfo.m_pvpGrade,
            m_apcInfo.m_creatureName, m_apcInfo.m_creatureId);
    TowerOfDespairMgr::SendMessageToCUser(user, msg);
