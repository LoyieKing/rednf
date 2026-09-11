// idx∈[0,4)：ORIG 反汇编 0x856515c/0x856517d 实证槽基址 = this + 0x24 + idx*0x10
// （movzbl 0x18+(idx+1)*16 读 m_flag、+(idx+1)*16+0x14 读 m_user），即
// m_teams[0].m_players[idx]（m_teams[0] @ +0x1c，m_players @ +0x08）。
CUser* CFairMatch::GetUser(unsigned int idx)
{
    if (idx > 3u) return 0;
    if (!m_teams[0].m_players[idx].m_flag) return 0;
    return m_teams[0].m_players[idx].m_user;
}