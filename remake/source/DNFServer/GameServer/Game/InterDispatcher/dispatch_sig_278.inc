int Inter_GoldControlMoneyActionCancel::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_GoldControlMoneyActionCancelPktView* pPkt = reinterpret_cast<Inter_GoldControlMoneyActionCancelPktView*>(data);
    if (user != 0)
    {
        // ORIG 直接 user+0x8e980 成员访问（无 GetGoldControl() 调用）。
        if (pPkt->m_0 != 0)
            user->m_goldControl.MailSameMid();
        else
            user->m_goldControl.MailComplete(
                pPkt->m_1);
    }
    return 0;
}