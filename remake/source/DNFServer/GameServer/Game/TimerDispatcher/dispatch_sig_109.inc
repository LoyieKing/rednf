// [b28d-30] 复核 DIFF：同 _NS_NonClient_CheckResponse——N18b 同型折 mem；异型化旋钮跨泳道。
// [b28d-solo-TimerDispatcher] 已 AE：Main 代落 getCurCharacNo→unsigned int 后两段
// 物化对齐（42/42）。
bool TimerRequestExit::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    CUser* user = G_CGameManager()->getUser(p1, 2);   // ORIG 第二实参为常量 2
    // [b28d-30] 非成员比较对象核对：本函数为成员，this@0x8 ⇒ 0x10(%ebp)=p2
    // （旧注释正确，b28d-30 一度误改 p3/0x14 已回滚）。ORIG `mov 0x10(%ebp),%edx;
    // cmp %edx,%eax` 两段物化 = callee 返回 unsigned 与 int 形参异型（P248 档①，
    // 探针 /tmp/probe30_e.cpp d5/d6 实证）。
    // ORIG 0x08638f6a：三段独立早退；characR 判空 sete 物化（c1 探针形）；
    // characNo 比较对象 p2（0x10(%ebp)），混型走 mov edx;cmp 形态（c3 探针形）。
    if (user == 0)
        return 0;
    if (user->getCurCharacR() == 0)
        return 0;
    // ORIG 0x08638f3b：`mov 0x10(%ebp),%edx; cmp %edx,%eax`（call 结果 vs arg 两步形）。
    if (user->getCurCharacNo() != p2)
        return 0;
    user->DisConnSig(static_cast<DISCONN_SIG>(0x2f), 1, 0);   // ORIG 0x8638f6a
    return 1;
}