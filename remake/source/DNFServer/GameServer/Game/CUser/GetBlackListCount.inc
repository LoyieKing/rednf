int CUser::GetBlackListCount(SIG_REQ_BLACK_COUNT* sig) const
{
    sig->m_count = 0;
    // ORIG 0x867f1ef-0x867f1f4：call;mov %eax,%edx; mov 0xc(%ebp),%eax; mov %edx,0x8(%eax)。
    // b28d-solo-CUser 试形：P211/P274 字段侧异型化——字段 int→unsigned int（SigTypes_C.h:391，
    //   同宽同偏移）+ 删冗余 (unsigned) cast（cast 被 4.4 折叠回同型）⇒ RHS-first + %edx 中转。
    sig->m_blackCount = getCurrCharacBlackCount();
    return 1;
