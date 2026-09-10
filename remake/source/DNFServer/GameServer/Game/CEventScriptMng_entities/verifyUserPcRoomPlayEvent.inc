int CPcRoomPlayTime::verifyUserPcRoomPlayEvent(CUser* user)
{
    // ORIG 0x8110a38：未开事件 → 0xb2；非 0xc 类 premium → 7；
    // 领奖次数达到 m_numberOfTime（先判 >=）→ 0x12，否则 0。
    // 末段比较已是 ORIG 形（cmp %edx,%eax; setae）：callee 返回 unsigned int 后
    // 4.4 不再插 `mov %eax,%edx` 中转；调用点 (unsigned int) cast 冗余无害
    // （S12 探针 x1≡x2）。
    if (!is_eventing())
        return 0xb2;
    if (!user->isAffectedPremium((ENUM_PREMIUM_TYPE)0xc))
        return 7;
    if ((unsigned int)user->getPcRoomPlayTimeRewardCnt() >=
        (unsigned int)static_cast<PcBangPlayTimeScript*>(m_script1c)->m_numberOfTime)
        return 0x12;
    return 0;
