bool CStackableItem::verifyUsablePlace(CUser* user)
{
    if (m_usablePlaces.empty())
    {
        return 1;
    }
    StackableAction::UsablePlace place =
        static_cast<StackableAction::UsablePlace>(0);
    if (G_GameWorld()->IsPVPChannel())
    {
        place = StackableAction::USABLE_PLACE_PVP;
    }
    else
    {
        switch (user->get_state())
        {
        case 5:
            place = StackableAction::USABLE_PLACE_DUNGEON;
            break;
        case 3:
            place = StackableAction::USABLE_PLACE_VILLAGE;
            break;
        }
    }
    if (std::find(m_usablePlaces.begin(), m_usablePlaces.end(), place)
        == m_usablePlaces.end())
    {
        return 0;
    }
    if ((char)m_arad148.m_field4 < 0 ||
        ((char)m_arad148.m_field4 >= 0 &&
         ((CUserCharacInfo*)user)->getCurCharFirstGrowType() ==
             (char)m_arad148.m_field4))
    {
        return 1;
    }
    return 0;
