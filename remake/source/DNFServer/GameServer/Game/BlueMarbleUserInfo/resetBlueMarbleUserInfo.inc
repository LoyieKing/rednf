void BlueMarbleUserInfo::resetBlueMarbleUserInfo()
{
    m_user = 0;
    m_userState = (BlueMarbleUserState::T)0;
    m_boardZone = 1;
    m_grade = (BlueMarbleUserGrade::T)4;
    resetBuff();   // ORIG BuffInfo::reset（m_exp=m_gold=m_drop=1）
    resetIndex();
}