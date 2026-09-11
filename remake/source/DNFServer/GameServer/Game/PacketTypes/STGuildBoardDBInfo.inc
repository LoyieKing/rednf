// ORIG 0x822b83c：成员 C1（m_members@0x84）→ ints +0x78/+0x7c/+0x80 清零 →
// memset(this,0,0x78)（+0..+0x77 全为 m_pad78，与 memset 首参等价）。
// 唯一定义处归本 TU：消除 MSG_GUILD_BOARD 构造对 _ZN18STGuildBoardDBInfoC1Ev 的 undefined。
STGuildBoardDBInfo::STGuildBoardDBInfo()
    : m_members()
{
    m_78 = 0;
    m_7c = 0;
    m_80 = 0;
    memset(m_pad78, 0, 0x78);
}