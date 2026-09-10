int Dispatcher_SecurityCardCertKey::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x0820a6bc（docs/class_func_reports/Dispatcher_SecurityCardCertKey/dispatch_sig.md）
    char certKey[6];                          // local_1a -0x16
    unsigned int keyLen = 0;                  // local_14 -0x10
    // [round5 A01 · C05④ 第二步判定（未落地，留痕）] ORIG 0x0820a847/0x0820a88c 尾测为
    //   `mov -0xc,%eax; cmp $0x4,%eax; setbe %al; test %al,%al; jne`（P51 setbe 守卫）。
    //   按 Main 修正后的步骤 2 前置「body 必须以 throw/return 终止才试 P225 原形」：
    //   本处循环 body 正常路径以 `idx = idx + 1` 结束（仅**条件** return）⇒ **无终止语句**
    //   情形，按判据直接转步骤 4（N-88）**不试 P225/do-while**，避免 +2 回边劣化（A07/A03 反例）。
    int idx;                                  // local_10 -0xc

    // +0x00：state < 3 → 拒绝
    if (user->get_state() < 3)
        return LineFunc(0xa675,
                        __PRETTY_FUNCTION__,
                        0, 0);

    // +0x46：清零
    keyLen = 0;
    memset(certKey, 0, 6);

    // +0x5a：读长度与 6 位认证码
    if (!packet.get_int((unsigned int&)keyLen))
        return LineFunc(0xa681,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_str(certKey, 6, keyLen))
        return LineFunc(0xa683,
                        __PRETTY_FUNCTION__,
                        0, 0);

    // +0xed：已激活且认证标志已开 → 拒绝
    if (user->getSecurityCard()->isActivate()
        && user->getSecurityCard()->isCertFlagOn())
        return LineFunc(0xa688,
                        __PRETTY_FUNCTION__,
                        0, 0);
    // +0x144：已认证 → 拒绝
    if (user->getSecurityCard()->isCertified())
        return LineFunc(0xa68c,
                        __PRETTY_FUNCTION__,
                        0, 0);

    // +0x184：前 5 位必须为数字（'0'..'9'；ORIG 为 movsbl 减 0x30 后无符号 ≤9 判定）
    for (idx = 0; idx <= 4; idx = idx + 1)
    {
        if ((unsigned int)((int)(signed char)certKey[idx] - 0x30) > 9)
            return LineFunc(0xa691,
                            __PRETTY_FUNCTION__,
                            0, 0);
    }

    // +0x1da：行为日志（action：未激活=2、已激活=3）
    if (!user->getSecurityCard()->isActivate())
        user->m_historyLog.SecuService((ENUM_SECU_SERVICE_TYPE)1, (ENUM_SECU_SERVICE_ACTION)2);
    else
        user->m_historyLog.SecuService((ENUM_SECU_SERVICE_TYPE)1, (ENUM_SECU_SERVICE_ACTION)3);

    // +0x233：Hades 安全等级切换（1=ENUM_HADES_SECURITY；value：未激活=0x1a、已激活=0x1b）
    if (!user->getSecurityCard()->isActivate())
        ((XNuclear::CHades*)user->getHades())->ChangeSecurity((XNuclear::ENUM_HADES_SECURITY)1, 0x1a);
    else
        ((XNuclear::CHades*)user->getHades())->ChangeSecurity((XNuclear::ENUM_HADES_SECURITY)1, 0x1b);

    // +0x292：DB 激活请求
    DB_SecurityCardActivate::makeRequest((unsigned int)user->GetUID(),
                                         user->get_acc_id(),
                                         certKey,
                                         user->getSecurityCard()->isActivate());
    return 0;
