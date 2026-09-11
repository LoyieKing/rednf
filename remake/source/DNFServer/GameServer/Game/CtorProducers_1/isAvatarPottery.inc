bool AvatarRouletteServer::isAvatarPottery(unsigned long itemIdx)
{
    // ORIG 0x0817fb0a：**(ulong**)(this+0x18)==itemIdx —— 即
    // m_pRoulette->m_avatarCoinNo（+0x18 指向 roulette 首 8 字节）。
    if (static_cast<unsigned long>(m_pRoulette->m_avatarCoinInfo) == itemIdx)
        return true;
    return false;
    // [推断] ORIG 为 8 字节 ulong 比对；共享头现以 int 建模 coin info 字段。
}