bool Quest::check_power_side(char side)
{
    if (m_powerSide == 0xff)
    {
        return 1;
    }
    char winner = GlobalData::s_power_manager->GetWinnerSide();
    if (m_powerSide == 0)
    {
        if (winner == side)
        {
            return 1;
        }
    }
    else
    {
        if (m_powerSide == 1)
        {
            char winner2 = GlobalData::s_power_manager->GetWinnerSide();
            if (winner2 != side)
            {
                return 1;
            }
        }
        else
        {
            LogManager::logFormat(1, "data_manager.cpp", __PRETTY_FUNCTION__,
                                  0x1161, "Quest Script Error q_index(%d)", m_index);
            return 0;
        }
    }
    return 0;
