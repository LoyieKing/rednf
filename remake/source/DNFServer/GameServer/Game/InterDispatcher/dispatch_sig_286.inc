// [b28d-solo-InterDispatcher] P226 语句表达式复现 ORIG 0x084e6170 的短路 al 物化
//   （mov$1/mov$0/test al je tail；具名 bool 局部 4.4 恒落槽）。
int Inter_SelectCharacMid::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_SelectCharacMidPktView* pPkt = reinterpret_cast<Inter_SelectCharacMidPktView*>(data);
    // ORIG 0x084e6170：user&&characR 以 al 物化（mov$1/mov$0/test al je tail，技巧22）；
    // m_0 检查独立正向（jne tail）；fatigue 实参免槽（call→movzwl %ax,%edi 跨双 call，R2L）。
    if (({ user != 0 && user->getCurCharacR() != 0; }))
    {
        if (pPkt->m_ok == 0)
        {
            user->getHackAnalyzer()->addServerHackCnt(
                user,
                (WongWork::ENUM_HACKTYPE)pPkt->m_hackType, 1,
                pPkt->m_arg10,
                pPkt->m_arg8);
            if (pPkt->m_hackType == 0x2ce)
            {
                reinterpret_cast<XNuclear::CHades*>(user->getHades())->GoldTracking(
                    2,
                    pPkt->m_arg10, "",
                    pPkt->m_arg8,
                    user->getCurCharacUsedFatigue() & 0xffff);
            }
        }
    }
    return 0;
}