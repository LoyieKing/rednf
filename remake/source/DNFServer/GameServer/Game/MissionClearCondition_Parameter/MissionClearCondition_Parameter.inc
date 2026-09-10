MissionClearCondition_Parameter::MissionClearCondition_Parameter(PVP_BATTLE_MODE mode)
{
    m_mode = 0xa;
    if (mode == (PVP_BATTLE_MODE)2)
        m_mode = 1;
    else if (mode == (PVP_BATTLE_MODE)3)
        m_mode = 2;
