// ORIG T 0x08697892 GetTeamId() const（W）：mov 0x4(%eax)。
// 【推断修正：objdump——旧桩误返 m_field00，ORIG 为 +0x4】
int COnlinePreliminaryTeam::GetTeamId() const
{
    return m_field04;
}