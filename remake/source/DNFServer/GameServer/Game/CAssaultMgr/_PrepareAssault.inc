void CAssaulter::_PrepareAssault(CUser* user)     // ORIG T 0x82e6780
{
    if (user->getCurCharacR() != 0)
    {
        m_hp = (unsigned char)user->getCurCharacStamina();
        user->SetCurCharacLeaveTimeout(false);
        user->getCurCharacInvenW()->GetCreatureMgrW()->TurnStomach(true);
    }
