bool CFairMatch::CheckRematch(PvP_Room* room)
{
    if (m_fieldB0 >= 3) return false;  // ORIG +0x0f cmp $0x2 立即数实证（P97）
    if (!_CheckRematchable(room)) return false;
    // ORIG +0x44..0x63：(this+t*0x48)+0x10 / disp 0xc = 全局 0x1c+0x48t
    // ⇒ m_teams[t].m_field00（“可重赛”标记；bool 取反 xor $1 实证，P22）。
    for (int t = 0; t < 2; ++t) {
        if (!m_teams[t].m_field00) return false;
    }
    return true;
