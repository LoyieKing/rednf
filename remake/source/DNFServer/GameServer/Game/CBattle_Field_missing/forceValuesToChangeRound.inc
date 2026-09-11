// ---------------------------------------------------------------------------
// CBattle_Field::forceValuesToChangeRound(short round, short smallRound, int diff)
//   —— ORIG T 0x08305f50：末轮强制推进（置全阶段清除、移除当前轮旧怪、写新小轮序）。
// ---------------------------------------------------------------------------
bool CBattle_Field::forceValuesToChangeRound(short round, short smallRound, int diff)
{
    // [round5 收口实测] ndiff=24（134/134）：①槽位整体浅 4（it/begin-sret/find-sret，
    // N82/N93 块级槽族）；②循环首条件极性 `setg/jne` vs ORIG `setle/je`（4.1 的 16 位
    // bool 物化 vs 4.4 直跳，P292/P197④ 族；while 前置形实测 12→56 反劣）⇒ 封 N。
    // p167：16=16 CALLSITE CLEAR。
    // [b28d-solo-CBattle_Field_missing] ORIG 0x08305f50：
    // ①`int nextDiff = diff` 具名槽 -0x18（P0 槽往返）仅供首次调用；
    // ②short result(-0x12) 预清 0；③GetCurrentMapInfo 每用点重调（begin/end/
    //   erase 后重 begin），无判空（P140/P157）；④循环内 it->second 逐字段重读。
    // ⑤残余 N82/N93：本仓 -0x24/-0x28 槽基 vs ORIG -0x20/-0x24，且 4.4 把
    //   continue 处的循环条件内联（`end()` 每轮在尾块求值）vs ORIG 回跳 cond 块
    //   （for(;;)+break 已试，仍差 3 条）；槽基/循环布局跨版本族封顶。
    int nextDiff = diff;
    const short nextOrder = (short)getNextSamllRoundOrder(round, nextDiff);
    short result = 0;
    if (nextOrder == 0)
    {
        LogManager::logFormat(
            1, "battle_field.cpp",
            "bool CBattle_Field::forceValuesToChangeRound(short, short, int)", 0xe7e,
            "ULTIMATE_LOG : blood_round_order (%d) forceValuesToChangeRound ERROR!!\n",
            (int)round);
        return 0;
    }
    if (nextOrder > smallRound)
    {
        for (int i = 0; i < nextOrder; ++i)
            m_bloodPhaseClear[i] = true;
        // [G11 试形2 实测劣化（NEAR→DIFF，多 push %ebx）⇒ 已回滚：mapInfo 具名局部必须保留]
        MapInfo* mapInfo = GetCurrentMapInfo();
        std::map<int, map_monster>::iterator it =
            GetCurrentMapInfo()->m_monsterMap.begin();
        // [round5 实测·第三形] while 前置条件（`while (field <= round)`，无 end()
        // 比较版）复测仍反劣：ndiff 24→36（4.4 恒把条件旋转到尾部并产 `jmp <cond>`），
        // 已回滚为 `for(;;){ if (field > round) break; ... }`（最优 24）。
        for (;;)
        {
            // [判据·多余门禁] ORIG 0x0830605f 循环体首条即 `operator->()` 解引用
            // （无 end() 比较/调用：ORIG 全函数循环内 call 序列仅 operator->/erase/
            // begin/GetCurrentMapInfo），退出仅靠下方两个 break；旧码的
            // `if (it == ...end()) break;` 是多余门禁（P171 过度防御）。
            // [判据·P196 符号性] ORIG 0x0830606a-0x0830607a 为 16 位有符号比较
            // （`movzwl 0x34a; cmp -0x2c(%ebp),%ax; setle`）；旧码 `(int)(unsigned
            // short)` 无符号扩展 + 32 位比较（movzwl+movswl+setg），负值语义分叉。
            // [L31 round2 实测] while 条件前置形（`while (field <= round)`）实测
            // 反劣 12→56 行差（4.4 对 while 前测恒旋转 + bool 归一化段错位），已回滚。
            // [L31 round2 判 N · P292/P197④ 跨版本 bool 归一化] ORIG 0x08306071-0x0830607a
            // 为正向物化 `cmp -0x2c(%ebp),%ax; setle %al; test %al,%al; je 退出`；
            // 我方 `field > round` 恒折反向 `setg/jne`（语义等价、极性/助记符差）。
            // 实测两形均不可翻：①否定形 `if (!(field <= round))` → 仍 setg/jne（差异 12 行不变）；
            // ②while 条件前置 `while (field <= round)` → 反劣 12→56 行。⇒ 记 N（与 L4 的
            // P292/P168a③ 同族封顶）。
            // ORIG 0x08306071 的正向物化 `setle/je` 在 4.4 恒折反向 `setg/jne`
            // （P292/P197④ 同族）⇒ 极性差封 N。
            // [G11 试形1 · P353/P354 正极性] ORIG 0x0830606a-0x0830607a 为正向物化
            // `cmp -0x2c(%ebp),%ax; setle %al; test %al,%al; je`（条件文本 = `<=`
            // 方向）。旧形 `if (field > round) break;` 恒折反向 `setg/jne`。
            // ⇒ 改写为正向包裹 if/else（else 即原 break）。
            if (it->second.m_bloodRoundOrder2 <= round)
            {
                if (it->second.m_bloodRoundOrder2 == round &&
                    (int)(signed char)it->second.m_bloodSpawned == diff)
                    break;
                // [判据·查找键/调用序] ORIG 0x083060c2-0x083060fc 为
                // `operator->()` → `lea 0x8(%eax)`（=&it->second.m_instId，+0x04）→
                // `map::find(const int&)`（sret -0x28）→ `map::erase(iterator)`
                // ⇒ 原码是 erase(find(it->second.m_instId))，非直接 erase(it)。
                mapInfo->m_monsterMap.erase(
                    mapInfo->m_monsterMap.find(it->second.m_instId));
                it = GetCurrentMapInfo()->m_monsterMap.begin();
                result = 0;
            }
            else
            {
                break;
            }
        }
    }
    (void)result;
    m_bloodRound.setBloodSmallRoundOrder(nextOrder);
    return 1;
}