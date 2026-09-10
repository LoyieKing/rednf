unsigned char CUserCharacInfo::GetCurCharacSuperState()
{
    return m_selected ? CUR->m_superState : 0;
