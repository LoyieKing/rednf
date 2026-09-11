int CDeathTowerRanking::getVirtualRank(unsigned int towerIdx,
                                       unsigned int stage,
                                       unsigned int playTime)
{
    // [b28d-solo-CDeathTower] 旧判「全帧槽平移 N82/P112 封顶」部分作废——
    // [b28d-PoolTower 按 Main P326/P50 复判指令] 三旋钮归正：
    //   ①迭代器组：原码为函数头两具名 const_iterator（ORIG 0x08468eab/eb9 两次
    //     `const_iterator::C1Ev` 默认构造）+ 全程 **nxt/ref 复用赋值**（begin/end/
    //     lower_bound 结果各经匿名转换临时两段 copy；0x08468f3d 区 `mov -0x7c,%eax;
    //     mov %eax,-0x84`）——旧源 first/last/lowerFirst 三个**发明局部**已删。
    //     p167 C1Ev MISSING×2→0（54=54 CLEAR）。
    //   ②哨兵：ORIG 无 bool inMap——`cmpl $0,-0x24(%ebp); jne 尾`（0x08469274）
    //     即 keySpan(-0x24，函数头具名，prologue movl $0) 兼「已插值」标志；
    //     bool inMap 局部（movb/movzbl/xor 三段）删除。
    //   ③位宽：valDiff/abs 是 **32 位** branchless abs（0x084691b0/0x08469203
    //     `mov,sar,xor,sub` + `mov %eax,-0x20`），非 64 位 sbb 展开；-0x20=absDiff、
    //     -0x28=stageSpan、-0x1c=result 均函数头具名标量。另 P183 去 RankMap&
    //     地址引用缓存（ORIG 每使用点重发 imul 链）。
    // 读数（fresh .o）：orig 610 / our 580→**614**；剩余残差 = 2×3 条 abs 输入侧
    // valDiff 槽往返（ORIG 寄存器直传，4.4 恒 spill = N18a 形，P248 三档旋钮
    // 不适用——非类型可翻）+ 全帧槽号平移（帧 0x164+1 push vs ORIG 0x144+3 push，
    // 4.4 outgoing 16B 对齐预留 N115 族）⇒ 平移部分维持 N95/N115/N82 封顶。
    // [b28d-solo-CDeathTower2 复核] fresh 读数 orig 610 / our 618；试形：absDiff
    // 改 unsigned + branchless `(v ^ (v>>31)) - (v>>31)`（P248③，欲复现 ORIG
    // 0x084691a5 的 mov/sar/xor/sub 与零扩展 fildll）实测 630 劣化 ⇒ 已回退。
    // 残余 = 2×3 条 valDiff 槽往返（N18a）+ x87 fildl↔fildll/fmuls↔flds+fmulp
    // （N85 邻族）+ 帧平移 ⇒ N 封顶。
    // 候选新纪录（stage, playTime）。ORIG 0x08468de0：本类无 towerIdx 越界检查。
    // 调用方（rankTable 槽位）保证 towerIdx 在 [0,4]，与 ORIG 保持一致不加防御。
    // [J10 2026-09-11 复核] orig 610 / our 612（gap −2）；p167 54/54 CLEAR（迭代器
    // C1↔C2、stBestRecord_t C1↔C2 同址别名）。ORIG 0x084691b3 `mov -0x20(%ebp),%eax;
    // mov $0x0,%edx; mov %edx,-0x12c; fildll`（absDiff 32→64 零扩展）vs 4.4.6 `fildl`
    // + valDiff 装载槽往返（N18a/N85 邻族）；帧 0x164+1push vs ORIG 0x144+3push（N95/N115）。
    // 试形（历史）：absDiff unsigned + branchless `(v^(v>>31))-(v>>31)` 实测 630 劣化已回滚
    // ⇒ 维持现形（P248 S1 一次性旋钮已用尽）。

    stBestRecord_t newRec;
    newRec.m_stage = stage;
    newRec.m_playTime = playTime;

    // 1) 表级最优（+0x7058 m_tableBest）严格优于新纪录 → -1。
    if (m_tableBest > newRec)
        return -1;
    // 2) 新纪录 == 表级最优 → 返回表级纪录存故名次（+0x70ec = m_tableBest.m_field94）。
    if (newRec == m_tableBest)
        return m_tableBest.m_field94;
    // 3) 新纪录严格优于该塔当前榜首（+0x98 = 槽 1）→ 名次 1。
    if (newRec > m_towers[towerIdx][1])
        return 1;

    // ---- 层内 rank map（键=通关时间，值=名次）插值 ----
    // [b28d-PoolTower P183] ORIG 每使用点重发完整 imul 地址链（0x08468ec5/
    // 08468fdd/084692a3…），无 map 地址缓存槽 ⇒ 去 RankMap& 具名引用。

    // [b28d-PoolTower P326/P50 复判] ORIG prologue 0x08468e90..0x8ebe 标量组：
    // -0x28/-0x24/-0x20 三条 movl $0 + -0x1c=0xffffffff(result) ⇒ 函数头四
    // 具名标量；**无独立 bool inMap**——「是否已层内插值」的哨兵是 keySpan
    // （-0x24）本身：插值段 `max(...)` 写 -0x24（0x0846917f），0x08469274
    // `cmpl $0x0,-0x24; jne 尾`；valDiff/abs 为 **32 位** branchless abs
    // （0x08469203 区 `mov,sar,xor,sub`）后 `mov %eax,-0x20` 存 absDiff。
    unsigned int stageSpan = 0;        // ORIG -0x28（跨层段 max 结果落此）
    unsigned int keySpan = 0;          // ORIG -0x24（兼「已插值」哨兵）
    // [H10 CDeathTowerTU] 类型轴：ORIG 0x084691b3..0x084691c7 读 -0x20 后
    // `mov $0x0,%edx; mov %edx,-0x12c; fildll`（零扩展 32→64）⇒ absDiff 为
    // unsigned（有符号会 cltd/sar）；4.4 int 形发 fildl。
    unsigned int absDiff = 0;          // ORIG -0x20（32 位 abs 结果）
    unsigned int result = 0xffffffff;  // ORIG -0x1c
    RankMap::const_iterator nxt;       // ORIG -0x80（prologue 首个 C1Ev）
    RankMap::const_iterator ref;       // ORIG -0x84（末次返回的基准迭代器）
    // [b28d-D13] ORIG 0x08468eab/0x08468eb9 两次 C1Ev（默认构造）⇒ nxt 先声明
    // 后赋值（P50 反例复活），禁初始化式（会折成拷贝构造，丢 C1Ev）。

    if (m_rankMap[towerIdx][stage].size() >= 2)
    {
        // playTime 须落在 [首键, 末键] 内；否则跳至“层邻插值”分支。
        // [b28d-PoolTower P326/P50 复判] ORIG 0x08468f1f..0x08468f4b 无 first/last
        // 独立槽：begin/end 结果经匿名转换临时**赋值**入函数头 nxt/ref 两槽
        // （`mov -0x7c,%eax; mov %eax,-0x84` 两段形）⇒ 原码即两具名迭代器复用。
        ref = m_rankMap[towerIdx][stage].begin();
        nxt = m_rankMap[towerIdx][stage].end();
        --nxt;
        if (playTime >= ref->first && nxt->first >= playTime)
        {
            // lower_bound(playTime)；若命中键 > playTime 且非 begin，退一位使
            // ref 恒为键 < playTime 的最近元素（= 插值下界）。
            ref = m_rankMap[towerIdx][stage].lower_bound(playTime);
            if (playTime < ref->first && ref != m_rankMap[towerIdx][stage].begin())
                --ref;
            nxt = ref;
            ++nxt;

            if (ref->first == playTime)
                return (int)ref->second;
            if (nxt->first == playTime)
                return (int)nxt->second;

            // 两相邻键线性插值：
            //   keySpan = nxt.key - ref.key（≥1）
            //   slope = |nxt.value - ref.value| / keySpan
            //   result = max(1, round((playTime - ref.key) * slope))
            keySpan = nxt->first - ref->first;
            // [b28d-D13] p167 差集：ORIG 调 std::max<unsigned>×4 ⇒ 4 处 max 源形。
            keySpan = std::max(keySpan, 1u);
            // [H10 CDeathTowerTU 试形 C] ORIG 0x084691a1..0x084691b0：差值在 %ecx
            // 寄存器（`mov %ebx,%ecx; sub %eax,%ecx`）**无具名槽**，随即可 `mov
            // %ecx,%eax; mov %eax,%edx; sar $0x1f,%edx; xor %edx,%eax; sub %edx,%eax`
            // 算术 abs ⇒ 源形是 abs(expr) 内联（非具名 valDiff + ?:）。
            absDiff = (unsigned int)__builtin_abs((int)nxt->second - (int)ref->second);
            float slope = (float)((long double)absDiff / (long double)keySpan);
            // [b28d-D13] 删发明 helper roundHalfEven（p167 EXTRA×2）；ORIG
            // 0x0846921c 形 = fildll+fmuls+fistpll（P176）⇒ 直接 (long long) 转换。
            // [H10 CDeathTowerTU 试过·已回滚] scaled 乘序交换（`slope * X`）实测
            // our 620、并多出 fstpt/fldt 的 long double 往返 ⇒ 回退 `X * slope`。
            long long scaled =
                (long long)((long double)(playTime - ref->first) * slope);
            unsigned int r = (unsigned int)scaled;
            result = std::max(r, 1u);
        }
    }

    if (keySpan == 0)   // ORIG 0x08469274 `cmpl $0,-0x24; jne 尾`：哨兵 = keySpan
    {
        unsigned int lowerStage = 0;   // ORIG -0x14
        unsigned int upperStage = 0;   // ORIG -0x10

        // 当前层 map 仅 1 元素时先判其末元素。
        if (!m_rankMap[towerIdx][stage].empty())
        {
            RankMap::const_iterator last = m_rankMap[towerIdx][stage].end();
            --last;
            if (last->first == playTime)
                return (int)last->second;
            if (last->first < playTime)
                upperStage = stage;
            else
                lowerStage = stage;
        }

        // 向下 / 向上找最近的、rank map 非空的相邻层（下界 ≥2，上界 <200）。
        if (lowerStage == 0)
        {
            lowerStage = stage;
            // [判据①] ORIG 0x0846939f..0x084693c4：do-while 尾测 = `subl $1; cmpl $1;
            // jbe 假岛; call empty; test; je 假岛; mov$1/mov$0; test; jne 回边` ⇒
            // 原码是 while(cond) 而非 break（break 形生 xor$1 + 反极性回边，见旧读数）。
            do
            {
                lowerStage = lowerStage - 1;
            } while (lowerStage >= 2 &&
                     m_rankMap[towerIdx][lowerStage].empty());
        }
        if (upperStage == 0)
        {
            upperStage = stage;
            // [判据①] ORIG 0x0846941f..0x08469447：`addl $1; cmpl $0xc7; ja 假岛;
            // call empty; test; je 假岛; …; jne 回边` ⇒ 同一 while(cond) 形。
            do
            {
                upperStage = upperStage + 1;
            } while (upperStage < kRankStageCount &&
                     m_rankMap[towerIdx][upperStage].empty());
        }

        // 下界层无纪录（或已下探至 <2 仍空）或上界层无纪录（或已上探至 200 仍空）→ -1。
        // [b28d-G11 语义轴/P171] 删除发明护栏 `upperStage>=kRankStageCount`：
        // Ghidra C 0x0846948a 段与 ORIG 反汇编均只有两次 `empty()` 检查
        // （`call empty; test %al; je -1` ×2，无第三条 cmp/jae 链）⇒ ORIG
        // 对上界==200 会无条件读 map[200]（越界照抄），不得加防御。
        if (m_rankMap[towerIdx][lowerStage].empty() ||
            m_rankMap[towerIdx][upperStage].empty())
            return -1;

        // 跨层插值：ref = 上界层末元素（最大键），基准 = 下界层首元素；
        //   stageSpan = upperStage - lowerStage（≥1）
        //   slope = |lower.first.value - upper.last.value| / stageSpan
        //   result = max(1, round((upperStage - stage) * slope))
        // [b28d-PoolTower P326/P50 复判] ORIG 0x084693d6..0x0846942f：begin 结果
        // 两段赋入 -0x80（nxt 复用槽），end 结果赋 -0x84（ref）后 -- ⇒ 无独立
        // lowerFirst 变量。
        nxt = m_rankMap[towerIdx][lowerStage].begin();
        ref = m_rankMap[towerIdx][upperStage].end();
        --ref;

        stageSpan = upperStage - lowerStage;
        stageSpan = std::max(stageSpan, 1u);
        // [H10 CDeathTowerTU 试形 C] 同族：ORIG 0x084695d0 区差值亦在寄存器（无槽）
        // + 算术 abs（0x084695dc sar/xor/sub）⇒ 同 abs(expr) 内联形。
        absDiff = (unsigned int)__builtin_abs((int)nxt->second - (int)ref->second);
        float slope = (float)((long double)absDiff / (long double)stageSpan);
        long long scaled =
            (long long)((long double)(upperStage - stage) * slope);
        unsigned int r = (unsigned int)scaled;
        result = std::max(r, 1u);
    }

    return (int)(result + ref->second);
}