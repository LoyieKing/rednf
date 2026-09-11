// ORIG 0x080d6a90：标量按声明序 init-list 直写 → 成员 C1 call(+0x10/+0x1c)
// → +0x24 直写 → 显式 reset() 两 call（C1 后二次 reset 语义照抄）。
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
}