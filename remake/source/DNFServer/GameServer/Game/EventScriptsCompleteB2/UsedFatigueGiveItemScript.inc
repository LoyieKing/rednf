UsedFatigueGiveItemScript::UsedFatigueGiveItemScript()
    : m_userType(0),
      m_usedFatigue(0),
      m_charcEventFlag(0),
      m_jobType(-1),
      m_fatigueLimit(-1)
{
    m_dbTableName.clear();
    m_mail.reset();
