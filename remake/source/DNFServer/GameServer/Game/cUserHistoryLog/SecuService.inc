void cUserHistoryLog::SecuService(ENUM_SECU_SERVICE_TYPE type, ENUM_SECU_SERVICE_ACTION action)
{
    if (type == ENUM_SECU_SERVICE_TYPE_GOBLIN)
        m_user->LogHistory("s_gob,%d", (int)action);
    else if (type == ENUM_SECU_SERVICE_TYPE_CARD)
        m_user->LogHistory("s_card,%d", (int)action);
