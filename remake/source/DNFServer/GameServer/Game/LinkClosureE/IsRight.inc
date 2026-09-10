char CBuffGroup::IsRight(CUser* user)
{
    if (user->get_charac_level() > 0x45)
        return 0;
    if (__builtin_abs((int)(unsigned short)user->get_posX() -
                      (int)(unsigned short)m_user->get_posX()) > 0x12c
        || __builtin_abs((int)(unsigned short)user->get_posY() -
                         (int)(unsigned short)m_user->get_posY()) > 0x12c)
        return 0;
    return 1;
