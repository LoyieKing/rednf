// ORIG T 0x08632a78 (_ZN17TimerAvatarExpire12dispatch_sigEiij)：恒返回 1（mov $1,%eax; ret）。
bool TimerAvatarExpire::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    return 1;
}