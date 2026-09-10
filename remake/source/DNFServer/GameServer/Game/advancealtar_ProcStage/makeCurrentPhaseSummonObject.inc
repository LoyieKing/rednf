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
    for (std::vector<_ActionAssign>::const_iterator it = phase.m_assigns.begin();
         it != phase.m_assigns.end(); it++)
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
            spawnMs += it->m_firstDelayMs;
            while (spawnMs < phaseEnd)
            {
                // widthscan：ORIG 全函数无 movb/cmpb（clr_b/cmp_b orig=0）⇒ create 是 int
                //（4.4 写 bool 会产 movb $1 + cmpb；ORIG 为 cmpl）。
                int create = 1;
                if (it->m_flag != 0)
                {
                    const int key = it->m_pSummon->m_field0c;
                    std::map<int, int>::iterator found = spawnCounts.find(key);
                    if (found == spawnCounts.end())
                    {
                        spawnCounts.insert(std::make_pair(key, 1));
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
                    so->m_seq = m_field14;
                    m_field14 = static_cast<short>(m_field14 + 1);
                    so->m_actionDefineId = define->m_id;
                    // ORIG 0x812f020 段 `movb $0x0,0xc(%eax)`：new 后显式清零 m_deadFlag
                    //（P171 照抄原码显式写；ctor memset 已清但 ORIG 逐条发射）。
                    so->m_deadFlag = 0;
                    std::memcpy(&so->m_summon, it->m_pSummon, 0x24);
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
    m_summonObjectMs.completePushSummonObject(firstIndex);
    m_field10 += phaseLength;
    m_summonObjectMs.debugLog();
    return true;
