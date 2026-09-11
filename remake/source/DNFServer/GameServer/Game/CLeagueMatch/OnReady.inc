// ORIG T 0x0855e52a。ORIG 返回 bool（!ready && state==4 → 0）。
bool CLeagueMatch::OnReady(CUser* /*user*/, bool ready)
{
    if (!ready && m_state == 4)
        return false;
    return true;
}