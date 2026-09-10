void CLeagueMatch::GetUserList(void* out)     // ORIG T 0x0855e642（拷贝第一队 0x70 字节，P83 rep movsl）
{
    *reinterpret_cast<TeamInfo*>(out) = m_teamA;
