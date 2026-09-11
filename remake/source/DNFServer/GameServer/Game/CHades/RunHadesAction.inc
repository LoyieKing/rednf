// ORIG 0x084b95bc（RunHadesAction.md）：Hades 行动分发。
//  a1==0x40 跳过历史日志；a1&2 → 标记 hack + 交易惩罚 + DB 上报(2)；
//  a1&1 且非 hack 计时中 → 标记 hack + 开计时 + DB 上报(1)；
//  a1&4 → DisConnSig(0x39, true, 0)。
void CHades::RunHadesAction(unsigned int a1, unsigned int a2, unsigned int a3,
                            unsigned int a4, char* a5)
{
    // b28d-solo-CHades verdict=AE [P214①] ORIG 0x084b95bc orig/ours=98/98
    // （check_tu_routed 口径 AE；.o 级 diff 仅未解析 call/jcc 重定位）。
    // 汇聚条件直写 `if ((a1&1)!=0 && !isHackUserTimer())` 即复现 ORIG
    // `je 岛; call; xor $1; test; je 同岛; mov $1; jmp; 岛: mov $0; test %al; je`
    // 及 `sub $0x28` 保守帧；旧「register bool punish + if(punish)」多
    // `mov %eax,%ebx` + push %ebx 且 setne 落 %bl（P78 假象，非不可翻）。

    if (a1 != 0x40)
        m_user->m_historyLog.HadesHistoryLog(a1, a2, a3, a4);

    if ((a1 & 2) != 0)
    {
        m_user->setHackUserFlag();
        m_user->SetTradePunishType(4);
        DB_HadesPunish::makeRequest(m_user->get_acc_id(), 2, a2, a3, a4, a5);
    }

    if ((a1 & 1) != 0 && !m_user->isHackUserTimer())
    {
        m_user->setHackUserFlag();
        m_user->setHackUserTimer();
        DB_HadesPunish::makeRequest(m_user->get_acc_id(), 1, a2, a3, a4, a5);
    }

    if ((a1 & 4) != 0)
        m_user->DisConnSig((DISCONN_SIG)0x39, true, 0);
}