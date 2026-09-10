int TimerAvatarExpire::CheckExpiredAvatar(CUser* user)
{
    PacketGuard guard;
    if (user->get_state() == 0)
    {
        return 1;
    }
    if (user->getCurCharacR() == 0)
    {
        return 0;
    }
    if (user->getCurCharacInvenR()->isAnyExpiredAvatarItem())
    {
        user->getCurCharacInvenW()->ExpiredAvatarItem(&guard);
    }
    return 1;
