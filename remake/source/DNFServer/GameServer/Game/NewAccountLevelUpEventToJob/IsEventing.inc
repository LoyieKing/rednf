// ORIG 0x081b8816：直调 CEventBase::IsEventing(this, user)（vtable+0x3c 覆盖）。
bool NewAccountLevelUpEventToJob::IsEventing(CUser* user) const
{
    return CEventBase::IsEventing(user);
}