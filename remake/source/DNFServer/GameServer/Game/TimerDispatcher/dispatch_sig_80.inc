int TimerCheckHumanCertifyTimeout::dispatch_sig(int p1, int p2, unsigned int p3)
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
    // [A12-2026-09-10 实验·失败] 删调用点 (unsigned int) cast 实测：p2 形参为 **int**
    // （非 unsigned）⇒ 裸 `getKey() > p2` 落有符号 `setg`（语义改变，106→104 insns）
    // ⇒ cast 为语义必需，非冗余；已回滚。ORIG `cmp %edx,%eax; seta` 的 call 结果留
    // %eax 属 N22/N7 寄存器角色差（P248 一次性旋钮，赌输封顶）。
    if ((unsigned int)user->getHumanCertifyTimerKey() > (unsigned int)p2)
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
