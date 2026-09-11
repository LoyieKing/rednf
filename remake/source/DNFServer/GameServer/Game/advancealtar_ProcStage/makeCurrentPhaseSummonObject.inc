// ----------------------------------------------------------------------------
// ORIG T 0x0812eba8（makeCurrentPhaseSummonObject.md 全文）：
//   timeLine 空 → 0；全部 phase 完成 → 0；取 phases[m_fieldc] 并后移下标；
//   alive=getAliveMonsterCount()；phaseEnd=m_field10+phase.m_field0；
//   按 assigns 的 [minAlive,maxAlive]（-1=不限）选 ActionDefine：
//   无命中 → 用不限档；仍无 → 0。define 空则跳过生成但保留尾部流程。
//   对 define->m_actions：spawnMs=m_field10+firstDelayMs，未达 phaseEnd 且
//   （flag==0 或 flag 计数 < flagValue）时 new _SummonObjectMs 填充并插入，
//   首个插入记 lastInsertIndex；否则 spawnMs+=nextDelayMs。
//   尾部 completePushSummonObject(lastInsertIndex)、m_field10+=时长、debugLog、返回 1。
// ----------------------------------------------------------------------------
bool ProcStage::makeCurrentPhaseSummonObject()
{
    if (m_timeLine == 0)
    {
        return false;
    }
    if (isEndAllPhase())
    {
        return false;
    }

    // b28d-solo-ProcStage：P167/P96——ORIG 0x812ebd9 `_ZNKSt6vectorI…Phase…EixEj`
    //   （const 版 operator[]）+ 全套 `__normal_iteratorIPKN…` ⇒ m_timeLine 为
    //   const TimeLine*（头已 const 化），phase 取 const 引用。
    // [L4d 2026-09-11 负结论] ORIG 0x0812ec2a `mov -0x3c(%ebp),%eax; mov (%eax)…` 的
    //   -0x3c 槽在 ORIG 里是 **m_timeLine 缓存**（0x0812ebca 落槽、0x0812ebf9 复用），
    //   我方 4.4 把该成员留在寄存器、把 operator[] 结果落 -0x3c ⇒ 整组槽浅 4。
    //   实测「补 `const TimeLine* tl = m_timeLine;` 具名槽」= 403→405 条 + NDIFF 552→586
    //   （劣），回滚；4.4 不为该成员发槽（同 P0 族）。
    const Phase& phase = m_timeLine->m_phases[m_fieldc];
    ++m_fieldc;
    const int alive = m_summonObjectMs.getAliveMonsterCount();
    // ORIG 0x0812ec27/0x0812ec2f 两局部：-0x30=phaseEnd（先取 m_field0 再 add
    //   m_field10）、-0x2c=phaseLength（phaseEnd 的副本，供尾部 m_field10+=）。
    int phaseEnd = phase.m_field0;
    const int phaseLength = phaseEnd;
    phaseEnd += m_field10;

    // b28d-solo-ProcStage：P167/P220——ORIG 0x812ec5c/0x812ec71 循环前两次 end()
    //   初始化两个 const_iterator 哨兵（-0x7c=rangedIt、-0x80=unlimitedIt），
    //   命中分支直接 `mov -0x78,%eax; mov %eax,-0x80/-0x7c`（迭代器整体赋值）；
    //   两处 `rangedIt == end()` 用 eq 正极性（差集 eq ×2）。
    std::vector<_ActionAssign>::const_iterator rangedIt = phase.m_assigns.end();
    std::vector<_ActionAssign>::const_iterator unlimitedIt = phase.m_assigns.end();
    // [L4d 2026-09-11] P331 声明序=槽序：ORIG 0x0812ec54 先 begin()→ -0x78（it），
    //   随后两次 end() → -0x7c/-0x80（rangedIt/unlimitedIt）；我方原序把 it 放 for-init
    //   ⇒ 槽序倒置（it 落 -0x98、哨兵占 -0x78/-0x7c）且多 4 条中转。改先声明 it。
    std::vector<_ActionAssign>::const_iterator it = phase.m_assigns.begin();
    for (; it != phase.m_assigns.end(); it++)
    {
        if (it->m_minAliveCount == -1 && it->m_maxAliveCount == -1)
        {
            unlimitedIt = it;            // 不限档
        }
        else if (alive >= it->m_minAliveCount && it->m_maxAliveCount >= alive)
        {
            rangedIt = it;               // 存活数区间档（循环不覆盖已命中者）
        }
    }
    if (rangedIt == phase.m_assigns.end())
    {
        rangedIt = unlimitedIt;
    }
    if (rangedIt == phase.m_assigns.end())
    {
        return false;
    }

    std::map<int, int> spawnCounts;
    int firstIndex = -1;
    // b28d-solo-ProcStage：P167——ORIG 的 m_actions 迭代器为 const 族
    //   （ZNK begin/end + ne(const,const)），无转换构造 ⇒ define 须为 const 指针。
    const ActionDefine* define = rangedIt->m_define;
    if (define != 0)
    {
        for (std::vector<_Action>::const_iterator it = define->m_actions.begin();
             it != define->m_actions.end(); it++)   // P28：ORIG 增量段 ppEi
        {
            // ORIG 0x812f054 段：`mov m_field10→-0x18; …; add %eax,-0x18`（两步具名局部）
            // ⇒ 拆两句（P0/P186：合并成一句被 4.4 折 lea）。
            int spawnMs = m_field10;
            // [L4d 2026-09-11] 语义轴①+槽复原：ORIG 0x812ee2c `mov 0x4(%eax),%eax;
            //   mov %eax,-0x14` 把 m_pSummon 缓存进具名指针槽 -0x14，0x812ee32
            //   `cmpl $0,-0x14; je <for 增量 0x812f034>` ⇒ 原码为「pSummon 判空正向
            //   包裹整段」。我方旧形**无判空**且每用点重取 it->m_pSummon（find/
            //   make_pair/memcpy 三处），既缺 ORIG 的空指针守卫又少 1 个槽 ⇒ 补。
            _SummonObject* pSummon = it->m_pSummon;
            if (pSummon != 0)
            {
            spawnMs += it->m_firstDelayMs;
            while (spawnMs < phaseEnd)
            {
                // widthscan：ORIG 全函数无 movb/cmpb（clr_b/cmp_b orig=0）⇒ create 是 int
                //（4.4 写 bool 会产 movb $1 + cmpb；ORIG 为 cmpl）。
                int create = 1;
                if (it->m_flag != 0)
                {
                    // [L4d 2026-09-11] P0/P183 去具名 key 槽：ORIG 0x812ee72
                    //   `mov -0x14(%ebp),%eax; lea 0xc(%eax),%edx` 直传
                    //   &m_pSummon->m_field0c（map::find 收 const int&），两次用点
                    //   各自重取 ⇒ 原码为表达式内联，无 key 局部。
                    std::map<int, int>::iterator found =
                        spawnCounts.find(pSummon->m_field0c);
                    if (found == spawnCounts.end())
                    {
                        spawnCounts.insert(std::make_pair(pSummon->m_field0c, 1));
                    }
                    else if (found->second < it->m_flagValue)
                    {
                        ++found->second;
                    }
                    else
                    {
                        create = 0;       // 该对象本轮配额用尽
                    }
                }
                if (create)
                {
                    _SummonObjectMs* so = new _SummonObjectMs;
                    so->m_ms = spawnMs;
                    // [L4d 2026-09-11] ORIG 0x812ef9f 段只读一次 m_field14：
                    //   `movzwl 0x14(%eax),%eax` → `mov %ax,0x4(%edx)`（old 存 m_seq）
                    //   → `lea 0x1(%eax),%edx` → `mov %dx,0x14(%eax)`（自增后写回）
                    //   ⇒ 原码是后置自增 `so->m_seq = m_field14++;`（旧形两句各读一次）。
                    so->m_seq = m_field14++;
                    so->m_actionDefineId = define->m_id;
                    // ORIG 0x812f020 段 `movb $0x0,0xc(%eax)`：new 后显式清零 m_deadFlag
                    //（P171 照抄原码显式写；ctor memset 已清但 ORIG 逐条发射）。
                    so->m_deadFlag = 0;
                    std::memcpy(&so->m_summon, pSummon, 0x24);
                    m_summonObjectMs.insertSummonObject(*so);
                    spawnMs += it->m_nextDelayMs;
                    if (firstIndex == -1)
                    {
                        firstIndex = m_summonObjectMs.getLastInsertDataIndex();
                    }
                }
                else
                {
                    spawnMs += it->m_nextDelayMs;
                }
            }
            }
        }
    }
    m_summonObjectMs.completePushSummonObject(firstIndex);
    m_field10 += phaseLength;
    m_summonObjectMs.debugLog();
    return true;
}