// ---- ORIG 0x84a55a8 getData(char*) const -----------------------------------
// [L6S4 round5b] 判词更新：ORIG 183 vs 我方 **184**（gap −1；改前 215）。
// 本轮修法（两处，均为 P291/P183 族）：
//   ① 条目写侧：cast 指针算术（`((Pair*)(sig+(idx+2)*0x10))->m_e1`）⇒ 4.4 把 sig
//      折进基址、每字段 6 条重算；改权威成员数组下标 `sig->m_jobScore[idx]`（同址）
//      ⇒ 复现 ORIG「索引寄存器 + 基址寄存器 + disp」形（仅常量拆分差：4.4
//      `lea 0x3/disp 0` vs ORIG `lea 0x2/disp 0x10`）。
//   ② 模式统计段：逐字段写（+27 条）改成员整体赋值
//      `*(COUNT*)&sig->m_modeStats[idx][0] = *(COUNT*)&m_modeStats[idx][0]`。
// 残余 1 条 + 错位归因（均 4.4 lowering）：
//   · N7（ORIG 统计段以 esi 缓存 this：`mov 0x8(%ebp),%esi` ⇒ prologue 多 push esi、
//     epilogue 多 pop esi，我方用 ebx/重载；帧 0x30 vs 0x34）；
//   · N4/P30（`(idx+2)*0x10` 的常量拆分习惯 + ORIG 写侧 `0x4(%eax,%ecx,1)` 索引/基址
//     分离 vs 4.4 合并为单指针：整体赋值段 25 vs 23 条）；
//   · 尾部 `sig->m_dailyPlayCount = m_dailyPlayCount` 4.4 多 1 条 `mov %eax,%edx` 中转。
// 试形记录（本轮）：+0x10 起 100 元素具名数组视图 ⇒ 4.4 产 `lea 0x2; lea 0x1` 两段，
// 劣化到 191 ⇒ 回滚。结构复原（ORIG 084a55a8 逐段实证）：
// ① idx 单槽复用（三循环共用 -0xc；ORIG 头部 movl$0 与 for-init movl$0 成对）；
// ② 迭代器为 for 外初始化式（begin() 直构 sret，无 C1Ev）；
// ③ 条目基址 (idx+2)*0x10 每点重发（P183 禁指针缓存，旧版 entry 具名局部多一槽）；
// ④ it-> 三次 ptEv（084a6764），第三段 `it->second` 整体结构赋值（一次 ptEv）。
// [L6S5 P211 试形·回滚] 尾部 m_dailyPlayCount 异型 store：改
//   `*(unsigned int*)&sig->m_dailyPlayCount = m_dailyPlayCount;`（同型别名）产
//   `add $0x67c,%eax; mov 0x8(%ebp),%edx; mov 0x50(%edx),%edx; mov %edx,(%eax)`（4 条），
//   与 ORIG 084a57c4 的 4 条（`mov 0x8(%ebp),%eax; mov 0x50(%eax),%edx;
//   mov -0x10(%ebp),%eax; mov %edx,0x67c(%eax)`）**形态不同**且总数不变（183/184）
//   ⇒ 回滚（本条属 N7/N4，源码不可控）。
void CFairPvPScore::getData(char* pData) const
{
    SIG_FAIR_PVP_SCORE* sig = reinterpret_cast<SIG_FAIR_PVP_SCORE*>(pData);
    int idx = 0;
    std::map<JobKey, FAIR_PVP_SCORE_MODE_COUNT>::const_iterator it =
        m_jobScore_.begin();
    for (idx = 0; it != m_jobScore_.end() && idx <= 99; ++it, ++idx)
    {
        // [L6S4 P291/P183] ORIG 084a55e1 段为「索引寄存器 + 基址寄存器 + disp」：
        //   `mov -0x10(%ebp),%eax; lea 0x2(%ebx),%ecx; shl $0x4,%ecx;
        //    mov %dl,0x10(%ecx,%eax,1)`（值=%dl）/ 0x11 / 0x14..0x1c。
        // 旧 cast 指针算术形把 sig 折进基址 ⇒ 每字段 6 条重算（215 条）。
        // 权威成员数组下标形（m_jobScore@+0x30）降到 184 条（ORIG 183）：
        // 唯一残差 = 常量拆分习惯差（4.4 `lea 0x3/disp 0` vs ORIG `lea 0x2/disp 0x10`，
        // 4.1 把 +0x30 拆成 2*0x10+0x10）；「+0x10 起 100 元素」视图试形反而劣化
        // 到 191（4.4 产 `lea 0x2; lea 0x1` 两段）⇒ 回滚。
        sig->m_jobScore[idx].m_job = (unsigned char)(unsigned int)it->first.m_job;
        sig->m_jobScore[idx].m_sub = (unsigned char)(unsigned int)it->first.m_sub;
        *(FAIR_PVP_SCORE_MODE_COUNT*)&sig->m_jobScore[idx].m_cnt0 = it->second;
        ++sig->m_jobScoreCount;                          // 字节自增（ORIG movzbl/lea/movb）
    }

    std::list<unsigned char>::const_iterator it2 = m_recentResults_.begin();
    // ORIG 084a56f0/084a56fa：先 `addl $1,-0xc` 再 operator++（increment 序与 map 循环相反）。
    for (idx = 0; it2 != m_recentResults_.end() && idx <= 9; ++idx, ++it2)
        sig->m_recentResults[idx] = *it2;

    for (idx = 0; idx <= 1; ++idx)
    {
        // [L6S4] ORIG 084a5674 段为整体结构赋值（源/目标基址各算一次、3 读 3 写：
        //   `mov 0x8(%edx),%ebx; mov %ebx,0x4(%eax,%ecx,1)` …）⇒ 逐字段写会
        // 每字段重算两侧基址（+27 条）。与 loadData 区块 3 同一复现形。
        // [L6S4] 方向必须为「成员 → 线格式」（sig 为目标）——上一编辑曾误交换，
        // 已回滚（该形若反向即真语义 bug，勿再试）。
        *(FAIR_PVP_SCORE_MODE_COUNT*)&sig->m_modeStats[idx][0] =
            *(FAIR_PVP_SCORE_MODE_COUNT*)&m_modeStats[idx][0];
    }
    sig->m_field1c = m_field40;
    sig->m_field20 = m_field44;
    sig->m_winStreak = m_winStreak;
    sig->m_bestWinStreak = m_bestWinStreak;
    // [L6S5 试形·回滚] P211 类型轴：`*(unsigned int*)&sig->m_dailyPlayCount = ...`
    //   产 4 条（`add $0x67c,%eax; ...; mov %edx,(%eax)`），与 ORIG 084a57c4
    //   的 4 条（`mov 0x8(%ebp),%eax; mov 0x50(%eax),%edx; mov -0x10(%ebp),%eax;
    //   mov %edx,0x67c(%eax)`）**形态不同**且总数不变（183/184）⇒ 回滚。
    sig->m_dailyPlayCount = m_dailyPlayCount;
    sig->m_lastPlayTime = m_field54;   // unsigned→int 异型：ORIG 084a57d3 两段中转
    sig->m_giveItem = m_giveItem;
}