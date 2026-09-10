int Dispatcher_SDC_Damage_Check::dispatch_sig(CUser* user, PacketBuf& packet)
{
    unsigned char count = 0;                            // -0x2d
    int party = 0;                                      // -0x2c 初值 0 后被 var 指针复用（ORIG 同槽）
    if (!packet.get_byte(count))
    {
        return LineFunc(0x6ab5,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    int capA = 500;                                     // -0x24（ORIG 赋 500，后未再读）
    int capB = 500;                                     // -0x20（同上）
    int srcLen = 500;                                   // -0x1c（循环内被长度覆盖）
    // ORIG [21:26]：`movl $0x1f4,-0x24/-0x20/-0x1c` 三条**连续**槽 ⇒ capA/capB/srcLen 相邻
    //   （destLen 不得插在中间，否则三槽被拉开、帧 0x440→0x450）。
    unsigned long destLen = 500;                        // -0x34
    // [Lane1Dispatchers 2026-09-10] N78/P214③：ORIG 0x081e9e84 段对两缓冲各发**内联**
    //   rep stos（我方 `memset` 在 4.4 -O0 恒退化为 call，rep-stos 计 0/2）⇒ 改聚合
    //   初始化 `= {0}`（4.4 对常量零初始化内联 rep stos，P214③ 实证）。
    char compBuf[500] = {0};                            // -0x22c 压缩数据缓冲
    long varBuf[125] = {0};                             // -0x420 解压目标（500 字节）
    STComputeDamageVariable* var =
        reinterpret_cast<STComputeDamageVariable*>(varBuf);   // -0x2c
    unsigned char loopIdx = 0;                          // -0x25
    for (; loopIdx < count; loopIdx += 1)               // setb：unsigned byte 比较
    {
        int len = 0;                                    // -0x38
        if (!packet.get_int(len))
        {
            return LineFunc(0x6ac9,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (len > 500)
        {
            return LineFunc(0x6acb,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (!packet.get_binary(compBuf, len))
        {
            return LineFunc(0x6ace,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        srcLen = len;                                   // -0x1c ← len
        int uncompressRet = uncompress(
            (unsigned char*)varBuf, &destLen,
            (const unsigned char*)compBuf, (unsigned long)srcLen);   // -0x18
        if (uncompressRet != 0)
        {
            return LineFunc(0x6ad8,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        int hackGate = 0;                               // -0x14
        int expectedHp = var->m_expectedHp;             // -0x10 ← +0x1e6
        hackGate = computeHpDamageSDC(var, 0, 0);
        if (expectedHp < hackGate - 1 || hackGate + 1 < expectedHp)
        {
            user->getHackAnalyzer()->addServerHackCnt(
                user, (WongWork::ENUM_HACKTYPE)0x25d, 1, 0, 0);
        }
        // ORIG [133]：`movswl %ax,%ebx` 直传（无 partyIdx 槽）。
        CParty* party2 =
            G_CGameManager()->GetParty((int)user->GetPartyIndex());   // -0xc
        if (party2 != 0)
        {
            // ORIG [150]：`lea 0xb24(%edx),%ecx` 寄存器直用（无 field 具名指针槽）。
            party2->m_battleField.SetDamagedPacketCnt(var->m_uid);   // +0x1ca
            party2->m_battleField.SetDamageValue(var->m_uid, hackGate);
        }
    }
    return 0;
