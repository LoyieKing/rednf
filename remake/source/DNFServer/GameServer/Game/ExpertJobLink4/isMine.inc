// isMine（ORIG W 0x08234854）：m_machine.get_user() == user
bool CDisjointer::isMine(CUser* user)
{
    return m_machine.get_user() == user;
}