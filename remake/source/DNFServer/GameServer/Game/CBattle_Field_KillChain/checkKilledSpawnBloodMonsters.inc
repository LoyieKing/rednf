// ============================================================================
// checkKilledSpawnBloodMonsters —— ORIG T 0x08304f8e。
// 血轮小轮收口判定：lastSmall<smallRound 时（未到最后一小轮）仅当
// monsterCount==0 且当前地图非 boss 房图才提前 onFinishBloodRound(false)；
// 否则遍历当前图怪物中属于本 smallRound 者（m_bloodRoundOrder>=smallRound），
// 该 smallRound 相对上一 smallRound 发生轮次或难度变化才推进结算：
// ultimate（dm+0x89f==2）路径走 incUltimateRoundClearPoint/
// forceValuesToChangeRound/setUltimateDifficulty + 0x26 计时器（末轮 1s，
// 否则 10s + sendUltimateRandomDifficulty）；普通路径 BloodSpawnTimerManager(true,false)。
// ============================================================================
void CBattle_Field::checkKilledSpawnBloodMonsters(int monsterCount)
{
    const short roundOrder =
        static_cast<short>(m_bloodRound.getBloodRoundOrder());        // -0x26
    const short smallRound =
        static_cast<short>(m_bloodRound.getBloodSmallRoundOrder());   // -0x24
    const short lastSmall =
        static_cast<short>(m_bloodRound.getBloodLastSmallRoundOrder());  // -0x22

    // [b28d-20] ORIG 0x08304fd5 movzwl -0x22; cmp -0x24,%ax; jge —— 有符号 short 比较
    // （unsigned cast 会产双 movzwl + jae）。
    if (lastSmall < smallRound)
    {
        if (monsterCount == 0 && m_pMap->get_map_type() != 1)
        {
            m_bloodReward.onFinishBloodRound(false, roundOrder, m_party,
                                             m_pDungeon);
        }
        return;
    }

    // [b28d-20] ORIG 0x0830504b `movl $0x0,-0x20(%ebp)` 为写后未读的死局部（P171 照抄；
    // 补此 4 字节使全帧槽号与帧尺寸 0x3a0 对齐）。
    int deadSlot = 0;
    (void)deadSlot;
    map_monster probe;   // ORIG 局部构造（-0x37c），仅对齐原帧布局
    (void)probe;

    // [b28d-20] ORIG 0x08305060/0x0830535b：begin/end 各自内联 GetCurrentMapInfo()（无
    // monsterMap 引用缓存槽）；循环为标准 for（incr 在 cond 前），结算体在循环内，
    // changedRound==0 走 continue（je <++it>），结算完 break 落 +0x428 析构 probe。
    for (std::map<int, map_monster>::iterator it =
             GetCurrentMapInfo()->m_monsterMap.begin();
         it != GetCurrentMapInfo()->m_monsterMap.end(); )
    {
        // [b28d-20] 真语义 bug 修正：ORIG 0x08305090 读 pair+0x33e = map_monster+0x33a
        // = m_bloodSmallRoundOrder（原写 m_bloodRoundOrder(+0x33e) 会读成 pair+0x342）。
        if (it->second.m_bloodSmallRoundOrder < smallRound)
            break;

        const int curRound =
            get_blood_round_of_small_round(smallRound);               // -0x1c
        const int prevRound =
            get_blood_round_of_small_round(
                static_cast<short>(static_cast<unsigned short>(smallRound)
                                   - 1));                             // -0x18
        const int curDiff =
            get_blood_round_difficylty_of_small_round(smallRound);    // -0x14
        const int prevDiff =
            get_blood_round_difficylty_of_small_round(
                static_cast<short>(static_cast<unsigned short>(smallRound)
                                   - 1));                             // -0x10
        if (curRound < 0 || prevRound < 0)
        {
            LogManager::logFormat(
                1, "battle_field.cpp", __PRETTY_FUNCTION__, 0xd59,
                "BLOOD_LOG : small_round_order_ (%d) : ABNOMAL ROUND!!\n",
                static_cast<int>(smallRound));
            return;
        }

        // [b28d-20] ORIG 0x0830514b movb $0x0/-0x9 与 0x08305169 cmpb $0x0,-0x9 直比：
        // 局部为 char（bool 会产 movzbl+xor$1+test 三段，P248 档②）。
        char changedRound = 0;
        if (prevRound != curRound)
            changedRound = 1;
        else if (curDiff != prevDiff)
            changedRound = 1;
        // [b28d-20] ORIG 0x08305350 的 ++it 是循环体自然结尾（changedRound==0 走
        // `je +0x3c2` 直落 incr 块后落 cond）；写成 for 第三段会被 4.4 旋进体内。
        if (changedRound != 0)
        {
            // [b28d-20] ORIG 0x08305185 `mov (%ebx),%ebx; add $0xddc,%ebx`：第二处结算的
            // this 经 m_party->m_battleField(+0xb24).m_bloodReward(+0x2b8) 折叠为
            // m_party+0xddc（与首处提前返回分支的 this+0x2b8 写法不同，运行时同一对象）。
            m_party->m_battleField.m_bloodReward.onFinishBloodRound(
                false, roundOrder, m_party, m_pDungeon);

        // [b28d-20] ORIG 0x083051b5 `movzbl 0x89f(%eax),%eax; cmp $0x2,%al`：真实成员直访问
        // （reinterpret_cast 视图会多一条 add $0x89f，P235）。
        if (m_pDungeon->m_bloodDungeonType == 2)
        {
            // [b28d-20] ORIG 0x083051e4/0x08305213：实参内联（无 diff/advanced 具名局部）。
            m_bloodReward.incUltimateRoundClearPoint(getNextRoundDifficulty(
                static_cast<short>(static_cast<unsigned short>(roundOrder) - 1)));

            if (!forceValuesToChangeRound(roundOrder, smallRound, 1))
                return;

            setUltimateDifficulty(0);
            // [b28d-20] ORIG 0x08305245 setne + je<1s 块>：条件取反并把 10s 块放 then
            // （写 `==` 会产 sete 且分支互换）。
            if (static_cast<int>(roundOrder) != getBloodMaxRound() - 1)
            {
                G_TimerQueue()->InsertTimer(
                    static_cast<TimerEntry::OBJ_TYPE>(1), m_party->GetPartyIndex(),
                    static_cast<TIMER_MESSAGE>(0x26), 10,
                    m_party->gen_timer_key(static_cast<TIMER_MESSAGE>(0x26)), 0x26);
                sendUltimateRandomDifficulty(0, roundOrder, 10);
            }
            else
            {
                G_TimerQueue()->InsertTimer(
                    static_cast<TimerEntry::OBJ_TYPE>(1), m_party->GetPartyIndex(),
                    static_cast<TIMER_MESSAGE>(0x26), 1,
                    m_party->gen_timer_key(static_cast<TIMER_MESSAGE>(0x26)), 0x26);
            }
        }
        else
        {
            BloodSpawnTimerManager(true, false);
        }
            break;
        }
        ++it;
    }
}