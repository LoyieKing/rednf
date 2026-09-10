int DisPatcher_DeleteItem::restricted_check(CUser* user, MSG_BASE& /*msg*/)
{
    if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
            user, (RestrictCategory::Enum)1, 1))
    {
        return 0xd1;
    }
    // ORIG 0x081c1cf6：Check 结果落 -0xc 槽，`cmpl $0` 判后回读返回（帧 0x28）。
    int nRet = (int)GlobalData::s_pSecuProtectionField->Check(
        user, (SECURITY_PROTCTION)0x1d);
    if (nRet != 0)
        return nRet;
    return 0;
