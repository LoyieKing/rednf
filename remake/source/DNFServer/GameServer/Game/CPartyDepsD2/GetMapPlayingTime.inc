unsigned int CParty::GetMapPlayingTime()
{
    // ORIG 0x085b6768：getCurTickCount 后连减 m_enterTick(+0x344/48)、
    // m_baseTick(+0x33c/40)（双 sub/sbb），结果写 m_playedTime(+0x334/338)，
    // 再读回 lo/hi（edx=0）。P180 union u64 别名使直减形编译为 sub/sbb。
    bd(this).m_playedTime =
        (unsigned long long)GlobalData::s_systemTime_.getCurTickCount() -
        bd(this).m_enterTick - bd(this).m_baseTick;
    return (unsigned int)bd(this).m_playedTime;
}