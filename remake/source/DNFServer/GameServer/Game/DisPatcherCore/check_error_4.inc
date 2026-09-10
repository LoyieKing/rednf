int DisPatcher_RecoverStamina::check_error(CUser* user, MSG_BASE& msg)
{
    if (user->get_state() != 3)
    {
        return 0x63;
    }
    int secu = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x2f);
    if (secu != 0)
    {
        return secu;
    }
    if (user->getCurCharacR() == 0)
    {
        return 0xffffffff;
    }
    return 0;
