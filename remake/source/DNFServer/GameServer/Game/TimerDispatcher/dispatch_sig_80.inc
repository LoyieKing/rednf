// [b28d-30] 复核 DIFF：槽位差 8（N82/N93 族），ORIG 0x086369xx 见 report。
// [b28d-solo-TimerDispatcher] 判词 N82+P292 封顶：槽平移 -0xc↔-0x14 + 块序差；
// isSendMailCertified 需 bool（CUser.h 冻结，已上报 Main）（105/113）。
// [TimerDispatcher-3 2026-09-10 判据①] 15 条 jcc 目标逐条对应（整体 +1 位移由一条 mov
// 中转引起）；唯一差 = call 结果 eax↔edx 中转（N22/N7；反形实测 116 insns 已回滚）。
bool TimerCheckHumanCertifyTimeout::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    CUser* user = G_CGameManager()->getUser(p1, 2);

    if (user == 0 || user->get_state() < 2)
    {
        return 0;
    }

    if (user->isHumanCertified() && user->isSendMailCertified())
    {
        return 1;
    }

    // [b28e-TimerDispatcher] 比较反形实测 116 insns（原 106）⇒ 回退；ORIG `cmp %edx,%eax`
    // 的 call 结果留 %eax 属 N22/N7 寄存器角色差（P248 一次性旋钮，赌输即回滚）。
    // [TD4 2026-09-10 判据①/P248 档③实测] 交换操作数书写序为 `(unsigned)p2 < key`
    // ⇒ 4.4 反劣（push %ebx + `cmp %eax,%ebx; setb`，帧 0x38→0x34、insns 106→107）；
    // ORIG `cmp %edx,%eax; seta`（call 结果留 %eax）属 N22/N7 寄存器角色差，回滚原形。
    // [TD-2026-09-11] H3CUser 已按 Main 核准把 `CUser::getHumanCertifyTimerKey()` 返回类型
    // int→unsigned（CUser.h:3405 + CUser/getHumanCertifyTimerKey.inc:1）⇒ 删左侧冗余 cast，
    // 落成探针 /tmp/td_p2.cpp f7 精确形（ORIG 086371cd `mov 0x10(%ebp),%edx; cmp %edx,%eax;
    // seta` = 2 条形）。p2 形参为 int ⇒ 右侧 cast 必须保留（A12 实验：裸 `key() > p2`
    // 落有符号 setg，语义改变 106→104 insns）。左侧 cast 变冗余也是 P41 变体诱因。
    if (user->getHumanCertifyTimerKey() > (unsigned int)p2)
    {
        return 1;
    }

    // P0：ORIG point 无槽（getPoint 结果 %eax 直入 0x4(%esp)）⇒ 实参内联。
    user->m_historyLog.RequestCleanPad(user->m_mcap.getPoint(), '\x03');

    // ORIG 08636xxx：5×`movl $0x0` 展开（20B 数组 = {0} 初始化，落在 RequestCleanPad
    // 之后）；旧循环形生 movw/addl/setle 循环体。
    unsigned short local_24[10] = {0};

    // P168a①：bool callee 一律裸形（技巧1：`!boolcall()` 直产 xor$1;test;je）
    if (!user->checkHumanCertify(1, local_24))
    {
        // ORIG 08636xxx：`call isSendMailCertified; xor $1; test; je <else>` ⇒
        // 原码为否定式 `if (!isSendMailCertified())`，正分支（reset 组）直落、else 后置。
        if (!user->isSendMailCertified())
        {
            user->setHumanCertified(true);
            user->SendMail(false);
            user->resetCleanpadFailCnt();
            user->resetHumanCertifyData();
        }
        else
        {
            user->DisConnSig((DISCONN_SIG)0x34, true, 0);
        }
    }

    return 1;
}