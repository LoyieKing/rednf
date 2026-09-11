// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x08209c8c orig/ours=291/251 ndiff-blocks=29
//   帧差 sub $18c vs $180 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %edi | sub    $0x18c,%esp | movl   $&_ZZN28Dispatcher_SecurityCardIssue12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp)
// [SweepDispatchers 2026-09-09] 形态收敛（251→290 = ORIG 291；p167 仅 C1/C2 别名）：
//   ① certKey 前 6 字节 ORIG 0x08209f7d 为 `movl $0 + movw $0` 两写（P202①），旧码 memset call；
//   ② phone 缓冲 0xc 字节 ORIG 0x08209cdf/e6/ed 为三个 movl（4.1 内联），旧码 memset call；
//   ③ serverKey[0x21]/cardData[0x27] 改块内 `= {0}` 复现 ORIG 0x08209f8a/0x08209fe2 慢路
//      （旧码两处 memset call ⇒ p167 EXTRA memset）。
int Dispatcher_SecurityCardIssue::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x08209c8c（docs/class_func_reports/Dispatcher_SecurityCardIssue/dispatch_sig.md）
    // [G2-91 2026-09-11 · P331] ORIG 0x08209f8a 先 `lea -0x9f(%ebp); call <memset>`
    //   （sha1Hex 清 0x29），**其后**才 `lea -0x164(%ebp); call <CSHA1 C1>` ⇒ 原码 ctor
    //   发射点在 memset 之后；旧形声明在函数头致 4.4 入口先发 ctor（多 4 条）且槽 -0x170。
    char sha1Hex[0x29];                       // local_a3 -0x9f
    char certKeyPhone[0x12];                  // local_32 -0x2e：[0..5]=certKey、[6..]=phone(0xc)
    unsigned int phoneLen = 0;                // local_20 -0x1c
                                              // [G6-91 负结论] P328⑤ 试去 `= 0` 预清并依赖
                                              // 守卫后 `phoneLen = 0;`：实测 291/290 不变
                                              // （-0x3c 清零点位置未变，差仍在 [6]-[12] 段
                                              //  sha1 ctor/守卫装载序）⇒ 已回滚，N 封顶。
    bool fail;                                // bVar11

    // +0x00：state != 3 → 拒绝
    if (user->get_state() != 3)
        return LineFunc(0xa596,
                        __PRETTY_FUNCTION__,
                        0, 0);

    // +0x4c：清零局部
    phoneLen = 0;
    // ORIG 0x08209cdf/0x08209ce6/0x08209ced：phone 缓冲 0xc 字节为三个 movl
    // （4.1 内联展开，非 memset call）。
    *reinterpret_cast<int*>(certKeyPhone + 6) = 0;
    *reinterpret_cast<int*>(certKeyPhone + 10) = 0;
    *reinterpret_cast<int*>(certKeyPhone + 14) = 0;

    // +0x6f：读 phone 长度
    if (!packet.get_int((unsigned int&)phoneLen))
        return LineFunc(0xa59f,
                        __PRETTY_FUNCTION__,
                        0, 0);
    // +0xac：读 phone
    if (!packet.get_str(certKeyPhone + 6, 0xc, phoneLen))
        return LineFunc(0xa5a0,
                        __PRETTY_FUNCTION__,
                        0, 0);
    // +0xff：号码格式检查
    if (!GlobalData::s_securityCardCenter->checkPhoneNumber(certKeyPhone + 6))
        return LineFunc(0xa5a3,
                        __PRETTY_FUNCTION__,
                        0, 0);

    // +0x145：SHA1(phone) → 40 字符十六进制
    memset(sha1Hex, 0, 0x29);                 // 0807dcc0
    CSHA1 sha1;                               // local_168 -0x164（ORIG ctor 在 memset 之后）
    sha1.Update((unsigned char const*)certKeyPhone + 6, phoneLen);
    sha1.Final();
    sha1.GetHexHash(sha1Hex);

    // +0x1b3：安全卡状态检查
    if (user->getSecurityCard()->isActivate())
    {
        if (!user->getSecurityCard()->isCertified())
            return LineFunc(0xa5b3,
                            __PRETTY_FUNCTION__,
                            0, 0);
        // [R4BigTUDispatchers 2026-09-11 · 符号性轴] ORIG 0x08209eb2 `cmp $0x2,%eax;
        //   seta %al`（**unsigned** 比较）—— 而同函数下方 CPad 分支 ORIG 0x08209f0f 为
        //   `setg`（signed）⇒ ORIG 两处**故意不同**（CSecurityCard::getFailCnt 返回
        //   unsigned）。我方原写 `2 < getFailCnt()`（int）产 `setg` ⇒ 与 ORIG 不符。
        //   同点 CPad 分支保持 signed（0821 行 `(int)` cast 即为此）。
        if (2u < (unsigned int)user->getSecurityCard()->getFailCnt())
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xa4, 0x7a);
            return 0;
        }
    }
    // +0x2f1：Pad 状态检查
    if (user->getPad()->isActivate())
    {
        if (2 < user->getPad()->getFailCnt())
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xa4, 0x88);
            return 0;
        }
        if (!user->getPad()->isCertified())
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xa4, 0x86);
            return 0;
        }
    }

    // +0x2f1：ORIG 对 certKey 前 6 字节发 movl+movw 两写（P202①），
    // serverKey/cardData 为 4.1 内联对齐剥离慢路（ORIG 0x08209f8a / 0x08209fe2）。
    *reinterpret_cast<int*>(certKeyPhone) = 0;
    *reinterpret_cast<short*>(certKeyPhone + 4) = 0;
    char serverKey[0x21] = {0};               // local_53/local_52 -0x4f
    char cardData[0x27] = {0};                // local_7a -0x76
    // +0x39d：生成卡数据/服务器密钥/证书密钥
    char* pCardData = GlobalData::s_securityCardCenter->generateCardData(cardData);
    char* pServerKey = GlobalData::s_securityCardCenter->generateServerKey(serverKey);
    char* pCertKey = GlobalData::s_securityCardCenter->generateCertKey(certKeyPhone);
    // +0x3e3：DB 请求
    DB_SecurityCardIssue::makeRequest((unsigned int)user->GetUID(),
                                      user->get_acc_id(),
                                      pCertKey, pServerKey,
                                      certKeyPhone + 6, sha1Hex, pCardData);
    return 0;
}