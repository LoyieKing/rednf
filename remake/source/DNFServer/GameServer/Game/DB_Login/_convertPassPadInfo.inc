// ============================================================================
// _convertPassPadInfo（ORIG T 0x08411af0）
// 【推断】返回值：ORIG 尾部无显式 ret 赋值，eax 残留自尾调的
// DB_MouseRegister::convertPassPadInfo（其真实源码应返回错误码 int，
// dispatch 以 0x48/0xa6 分支消费）；此处按该语义透传。
// ============================================================================

// ORIG T 0x08411af0：lea 0x390d/0x3910/0x3905/0x3914/0x390c/0x3918/0x38e0/
// 0x38e1/0x390e/0x3a3a 直取地址压栈调 convertPassPadInfo，尾部无回写。
// mousepass 实参为 m_passPadPassword+0x24（0x3905），ip 实参为 m_passPadPassword。
int DB_Login::_convertPassPadInfo(SIG_LOGIN_DATA* sig)
{
    return DB_MouseRegister::convertPassPadInfo(
        sig->m_accId,                                  // mid
        sig->m_passPadPassword + 0x24,                 // mousepass（0x3905）
        sig->m_passPadFailCnt,                         // fail_cnt(int&)
        sig->m_passPadVersion,                         // cancel_cnt(char&)
        sig->m_passPadCancelCnt,                       // cancel_cnt2(int&)
        sig->m_passPadEnable,                          // validityExpired(bool&)
        sig->m_passPadPassword,                        // ip（0x38e1）
        sig->m_passPadCertified,                       // flag2(bool&)
        sig->m_webAddress,                             // buf3（0x3a3a）
        sig->m_passPadCheckTime,                       // validity_time(long&)
        sig->m_passPadAdvideChangePwd);                // flag3(bool&)
}
