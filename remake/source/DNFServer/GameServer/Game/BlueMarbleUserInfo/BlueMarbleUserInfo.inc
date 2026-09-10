BlueMarbleUserInfo::BlueMarbleUserInfo()
    : m_user(0),
      m_userState((BlueMarbleUserState::T)0),
      m_boardZone(1),
      m_grade((BlueMarbleUserGrade::T)4),
      m_buff(),
      m_tileIndex(),
      m_pScript(0)
{
    m_buff.reset();
    m_tileIndex.reset();
