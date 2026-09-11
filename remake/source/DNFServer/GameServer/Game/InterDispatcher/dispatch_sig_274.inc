int Inter_UpdateSecuServiceState::dispatch_sig(CUser* user, char* data, int len)
{
    // ORIG +0x06：包基址先无条件落 -0xc，其后才是 user 判空门。
    Inter_UpdateSecuServiceStateMsgWinView* pWin_msg =
        reinterpret_cast<Inter_UpdateSecuServiceStateMsgWinView*>(data);
    // ORIG +0x0c 判的是 0xc(%ebp)＝user 形参（非 data）。
    if (user != 0)
    {
        pWin_msg->m_14 = 0;
        GlobalData::s_pSecuDataControl->UpdateSecurityServiceState(
            user,                                      // 0xc(%ebp)
            (unsigned char)pWin_msg->m_e,
            (unsigned char)pWin_msg->m_f,
            pWin_msg->m_pad10);   // §4.3 具名数组基址替代裸偏移 pWin_msg+0x10
    }
    return 0;
}