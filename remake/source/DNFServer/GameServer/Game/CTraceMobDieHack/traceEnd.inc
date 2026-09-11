void CTraceMobDieHack::traceEnd(CUser* killer, CUser** users)
{
    if (!m_bTracing)
    {
        return;
    }
    long curSec = GlobalData::s_systemTime_.getCurSec();
    m_bTracing = false;
    if (killer == 0 || killer->GetParty() == 0 ||
        ((CParty*)killer->GetParty())->m_battleField.m_pDungeon == 0)
    {
        return;
    }
    int dungeonIndex = ((CParty*)killer->GetParty())->m_battleField.m_pDungeon->get_index();
    int specialDungeons[4];
    specialDungeons[0] = 1;
    specialDungeons[1] = 2;
    specialDungeons[2] = 0x29;
    specialDungeons[3] = 0x21;
    // [G23-P310 试形] 旧注释「穷举 30 形 × 三后端全部生成 cmpl mem」未覆盖 P310 形：
    // 局部 int + 显式 (unsigned int)i<=K 比较 ⇒ 复现 ORIG 两段
    // `mov -0x10(%ebp),%eax; cmp $0x3,%eax`（4.1 装载形）。
    for (int i = 0; (unsigned int)i <= 3u; ++i)
    {
        // [N6S5ZeroA] ORIG 0x08599b4b(+0xc3) 条件是 `mov -0x10(%ebp),%eax;
        //   cmp $0x3,%eax; setbe %al`，我方为 `cmpl $0x3,-0x10(%ebp); setbe`
        //   （gap=+1，ORIG 多一条装载）。穷举 30 形 × 三后端（4.4.7/4.4.6/4.4.4 +
        //   4.1 族 c5）全部生成 `cmpl $3,mem`；唯一生成 `mov;cmp $3,%eax` 的
        //   volatile 形与 ORIG 的 `addl $1,-0x10(%ebp)` 自增形互斥
        //   （volatile 自增为 load/add/store 三条）⇒ 源码不可控，判 N 封顶
        //   （编译器 lowering 差异；体/符号性/边界 setbe(unsigned) 均与 ORIG 一致）。
        if (specialDungeons[i] == dungeonIndex)
        {
            return;
        }
    }
    for (int j = 0; j <= 3; ++j)
    {
        if (users[j] == 0 || users[j]->getCurCharacR() == 0 ||
            users[j]->GetParty() == 0)
        {
            continue;
        }
        if (users[j]->get_charac_level() >
            ((CParty*)users[j]->GetParty())->m_battleField.m_pDungeon->get_standard_level() + 10)
        {
            continue;
        }
        m_records[j].m_user = users[j];
        m_records[j].m_damage += (int)(curSec - m_traceBeginTime);
        // [N6S5ZeroA] ORIG 0x08599c5b(+0x1d3) 块首为 `mov -0xc(%ebp),%edx;
        //   mov 0x8(%ebp),%ecx; mov %edx,%eax`（edx 版，this→ecx）；`m_count += 1`
        //   曾生成 `mov j,%edx; mov j,%ecx; mov this,%ebx; mov %ecx,%eax`（多一条
        //   P231 纯寄存器复本 + this→ebx）。改后置/前置自增形为 `++` 后，
        //   我方指令形与 ORIG 逐条一致（gap -1→+1，余仅 i 循环那一条）。
        ++m_records[j].m_count;
    }
}