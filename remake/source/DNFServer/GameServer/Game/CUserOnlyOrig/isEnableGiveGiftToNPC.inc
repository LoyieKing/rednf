int CUser::isEnableGiveGiftToNPC()
{
    int r = GlobalData::s_pSecuProtectionField->Check(
        this, (SECURITY_PROTCTION)0x15);
    if (r != 0)
        return r;
    if (private_store::GetInstancePrivateStoreMgr()->IsBusyPrivateStore(this))
        return 0x13;
    if (IsCurCharacGhost())          // CUserCharacInfo 继承成员（真名按头取齐）
        return 0x13;
    if (CheckInTrade())
        return 0x13;
    if (GetAssaultPlace() != 0)
        return 0x13;
    if (G_GameWorld()->IsPVPChannel())
        return 0x13;
    return 0;
