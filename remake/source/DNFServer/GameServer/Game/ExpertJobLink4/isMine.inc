bool CDisjointer::isMine(CUser* user)
{
    return m_machine.get_user() == user;
