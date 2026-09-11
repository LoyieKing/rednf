// ORIG T 0x08586124 / 0x08586146 / 0x08586168：改字段后 alter() 标脏。
void COnlinePreliminaryMember::IncreaseWinCount()
{
    ++m_winCount;
    alter();
}