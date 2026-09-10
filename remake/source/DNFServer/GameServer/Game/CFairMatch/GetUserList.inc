void CFairMatch::GetUserList(void* out)
{
    *reinterpret_cast<TeamInfoThird*>(out) = m_teams[0];
