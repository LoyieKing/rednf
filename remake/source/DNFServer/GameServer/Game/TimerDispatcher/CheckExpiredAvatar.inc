// ORIG T 0x08632a82 (_ZN17TimerAvatarExpire18CheckExpiredAvatarEP5CUser)
// PacketGuard 局部 + get_state==0 → 1；getCurCharacR==0 → 0；
// 有到期头像时经 getCurCharacInvenW 发包；恒 1。
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
}