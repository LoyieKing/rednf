bool CPCRoomFatigueEvent::IsEventing(CUser* user) const
{
    // ORIG 0x08267720：先以 NULL 调基类 CEventBase::IsEventing 判总开关，
    // 再单独检查 user 非空；两者皆真时返回 isAffectedPremium(user, 0xc)。
    if (!CEventBase::IsEventing(0) || user == 0)
        return false;
    if (!user->isAffectedPremium((ENUM_PREMIUM_TYPE)0xc))
        return false;
    return true;
}