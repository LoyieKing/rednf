// ORIG 仅清 +0x00/+0x04(1B)/+0x08/+0x0c；+0x05 的 CharacterKind m_kind 不初始化。
void PlayerInfoThird::reset()
{
    m_user = 0;
    m_flag = 0;
    m_pvpGrade = 0;
    m_rating = 0.0f;
}