int Inter_LoadQuest::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)
        return 0x1a13;
    // ORIG 0x084c5079：pPkt 在 state 岛后才物化 -0x10（技巧13 声明位置）。
    Inter_LoadQuestPktView* pPkt = reinterpret_cast<Inter_LoadQuestPktView*>(data);
    if (pPkt->m_4 == 1)
        return 0x1a1a;
    if (user->getCurCharacR() == 0)
        return 0x1a1d;

    // ORIG 0x084c50bd：questR 具名槽 -0xc（+0xc3 先物化 pPkt 实参、+0xc7 槽装 this）。
    UserQuest* questR = user->getCurCharacQuestR();
    questR->set_quest(
        reinterpret_cast<const SIG_LOAD_QUEST*>(pPkt));

    if (GlobalData::s_timeGate_->isOpen())
    {
        // b28c-l23 P0 清绑：ORIG 0x084c50f0/0x084c511a 两段 `mov %eax,%ebx` 跨 call
        // 缓存（4.1 直落 callee-saved）+ 两次独立 getQuestIndex 重读（P48 无 CSE）。
        // 绑定形与 P0（-O0 局部恒占栈槽）相悖已删；4.4 自然形把值落槽（N96/N18a
        // 反向判词，P168a④），语义不变。[b28c-l23 修正] 保留**裸 register**：
        // P0 禁的是 `register T x asm("reg")` 寄存器绑定，而 4.4 对带 register 的
        // 标量局部不给栈槽、值留 callee-saved 跨 call——正是 ORIG 形（lane9/lane17
        // 同实测：删关键字反而槽化退化）。
        register int questIdx = GlobalData::s_timeGate_->getQuestIndex();
        if (user->getCurCharacQuestR()->IsDoingQuest(questIdx))
        {
            register int questIdx2 = GlobalData::s_timeGate_->getQuestIndex();
            user->getCurCharacQuestW()->giveup_quest(questIdx2);
        }
    }
    user->EnableCharacInfo((CACHE_SECTION)3);
    return 0;
