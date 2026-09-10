bool GameWorld::leave_game_world(CUser* user)
{
    if (GlobalData::s_GM_Manager->GetCurrentGmMode(user->get_acc_id()))
    {
        user->SetGameMasterMode(false);
        GlobalData::s_GM_Manager->TurnGmMode(user->get_acc_id());
    }

    // P50 复活：ORIG 迭代器 C1Ev 在 key 求值之前发射（声明点即发射点）。
    std::map<unsigned short, CUser*>::iterator userIt;
    unsigned short uniqueId = (unsigned short)user->get_unique_id();
    userIt = m_UserInWorld.find(uniqueId);
    if (userIt == m_UserInWorld.end())
    {
        LogManager::logFormat(1, "world.cpp",
                              __PRETTY_FUNCTION__, 0x56a,
                              "m_UsersInWorld.find(%d)",
                              (unsigned short)user->get_unique_id());
        return false;
    }

    // ORIG 0x086c539c 再次 get_unique_id 存 -0x1e 后 erase(&key)（非复用 uniqueId）
    m_UserInWorld.erase((unsigned short)user->get_unique_id());

    if (check_valid_area(user->getCurCharacVill(), user->get_area(false)))
    {
        Village* village = getUserVillage(user);
        if (village != NULL)
        {
            Area* area = village->getArea(user->get_area(false));
            if (area != NULL)
            {
                area->erase_user(user);
            }
        }
    }

    PacketGuard packet;
    packet.put_header(0, 6);
    packet.put_short((unsigned short)user->get_unique_id());
    packet.finalize(true);
    send_all(packet);

    std::map<unsigned int, unsigned short>::iterator sessionIt;
    unsigned int accId = user->get_acc_id();
    sessionIt = m_sessionList.find(accId);
    if (sessionIt == m_sessionList.end())
    {
        LogManager::logFormat(1, "world.cpp",
                              __PRETTY_FUNCTION__, 0x588,
                              "session_list.find(%s)",
                              NumberToString(user->get_acc_id(), 0));
        return false;
    }

    m_sessionList.erase(user->get_acc_id());
    // ORIG 0x086c568f..0x086c56d8 真 try/catch（[EXCEPTION Leave GameWorld]）。
    try
    {
        g_schoolMgr.DelUser(user);
        if (user->get_charac_guildkey() != 0)
        {
            g_guildAgitMgr.ReleaseGuildAgitArea(user->get_charac_guildkey());
        }
        if (user->isJoinPowerWar())
        {
            GlobalData::s_power_manager->DecreasePowerUserCount((char)user->getPowerSide());
        }
        if (user->isHangameUser())
        {
            m_hangameUserCount--;
        }
        else
        {
            m_userCount--;
        }
    }
    catch (...)
    {
        LogManager::logFormat(1, "world.cpp", __PRETTY_FUNCTION__, 0x5a7,
                              "[EXCEPTION Leave GameWorld] - code = %s",
                              strerror(errno));
        return false;
    }
    return true;
