int Arad_INTER_kEmblemCompound::dispatch_sig(CUser* user, char* body, int len)
{
    // [b28d-solo-Arad_InternalDispatcher_INTER 判词] AE（ORIG 0x0819a0c0）：90=90、
    // push edi/esi/ebx 组与槽位全等。解法 = P227 全表达式临时链（见函数尾注释）。
    if (user == 0)
        return 0x6b6;
    if (user->getCurCharacR() == 0)
        return 0x6b9;

    arad::SigAradEmblemCompound* sig = (arad::SigAradEmblemCompound*)body;   // ORIG 读 0x10(%ebp)=body
    if (sig == 0)
    {
        LogManager::logFormat(1, "localjapan/Arad_InterDispatcher.cpp", __PRETTY_FUNCTION__,
                              0x6be, "EMBLEM COMPOUND INTER_DSP(kEmblemCompound) null.");
        return 0x6bf;
    }

    // P48 + ORIG 寄存器出口：! 语义产生 xor $0x1 翻转；Check/Process 每次各自 Get()。
    if (!Singleton<EmblemCompoundServer>::Get()->CheckCondition(user, sig))
        return 0;
    if (!Singleton<EmblemCompoundServer>::Get()->Process(user, sig))
        return 0;

    // [b28d-solo-Arad_InternalDispatcher_INTER] P227 全表达式临时链：ORIG 0x0819a199..0x0819a1d5 在 cMyTrace
    // ctor 前右起求 varargs（getCurCharacNo→%ebx、m_resultIdx→槽、m_slot1→%edi、
    // m_slot0→%esi）并跨 ctor 保活；具名局部形 4.4 全落栈槽（前测 4 槽）。
    cMyTrace(__PRETTY_FUNCTION__, 0x6cc, 0)(
        "EMBLEM COMPOUND : success [slot1:%d] [slot2:%d] [selectNo:%d] [resultIdx:%d] [charac_no:%d]",
        (unsigned int)sig->m_slot0, (unsigned int)sig->m_slot1,
        (unsigned int)sig->m_resultIdx, (unsigned int)user->getCurCharacNo());
    return 0;
