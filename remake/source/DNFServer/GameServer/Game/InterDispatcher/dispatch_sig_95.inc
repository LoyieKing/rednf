int Inter_MemberExpLevelUp::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    // ORIG：门1 = get_state()==0 早退（`test %eax,%eax`+`je`，无 sete 物化）；
    // 包基址槽在门1 之后才落 -0xc；门2 用 (bool) 显式物化（`setne`+`test`）。
    if (user->get_state() == 0)
        return 0;
    Inter_MemberExpLevelUpPktView* pPkt = reinterpret_cast<Inter_MemberExpLevelUpPktView*>(data);
    if ((bool)(user->getCurCharacNo() != pPkt->m_characNo))
        return 0;
    user->set_uppermember_exp_level(
        pPkt->m_uppermemberExpLevel);
    return 0;
}