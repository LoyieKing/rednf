// [b28d-solo-LinkClosureE 旧判词，已被 D08 推翻] 原 DIFF 299/314：残差 = 7 个
//   epilogue 中继岛（每守卫 1 组 `jmp+nop`，共 14 条），当时 `return`/独立 `goto` 两形
//   均不消 ⇒ 曾判 N121 封顶。**[D08 2026-09-10] 该 14 条已按 C05 范式③ 全消**
//   （our_insns 314→300，见下方判词）⇒ N121 判词作废。其余逐条全等：七守卫
//   `movzwl/mov + test + jne` 直达尾、结构体拷贝（对齐剥离 + rep movsl + 尾 2/1 字节，
//   替代 memcpy）、三 trace 槽 -0x58/-0x48/-0x38、12 个 vararg 全表达式。
// [D08 2026-09-10] 消岛后 gap 精确分解（仅余 +1）：基表源地址计算 ——
//   ORIG 0x08145f5c 段 `mov 0x14(%eax),%ecx; mov job,%eax;
//   imul $0x7dc,%eax,%edx; lea (%ecx,%edx,1),%edx` vs 我方 `mov 0x14(%eax),%eax;
//   mov %eax,%edx; imul $0x7dc,%eax,%eax; add %eax,%edx`（4.4 多一条 eax→edx 中转）。
//   探针 /tmp/addr_probe.cpp g0–g6（char* 算术 / 类型化下标 / uint 算术+回 cast /
//   交换操作数 / u 后缀 七形）读数全同 g0；唯一省掉中转的 g2（具名基址指针）引入
//   `mov %eax,-0xc(%ebp); mov -0xc(%ebp),%eax` 槽往返 ⇒ 净 −1 条反劣。判 N18a。
//   [C05 范式③ 命中·D08 实测] 七守卫由「7 条独立 `if(…) goto L_done;`」改为**单条
//   `||` 短路 `if (c0||c1||…||c6) goto L_done;`**：4.4 对短路 or 逐条
//   `movzwl/test/jne` **直达同一尾标签**，7 组 `jmp+nop` 中继岛全消 ——
//   our_insns 314→**300**（ORIG 299）、gap +15→**+1**。反例留痕：`do{…}while(0)`+9 处
//   `break` 形实测仍 314（break 出口同样物化为岛）；7 条独立 goto 形亦 314。
//   余 +1 条 = 上述基表源地址 `imul/lea` 装载形（N18a，探针 g0–g6 七形不可造）。
//   **适用边界（Main 汇总 A06/A28 反例）**：本法仅在「ORIG 有**相邻多守卫·同目标**、
//   且我方多岛」时成立 —— `_onChangeGrowType` 按同法合并两条 goto **反劣 +4**（199/207），
//   `CWorldDrop::generateRandomItem` / `CPrivateStore::IsAreaNearEntranceDungeon` 类岛根因
//   是 4.4 epilogue-threading / rel8 跳板（拉长后造跳板 +1 jmp）⇒ 源码不可控，命中即按
//   A28 封顶。判据：先数 ORIG 各 `je/jne` 的**落点是否为同一址**；同址才试合并。
//   **试前闸门（Main/C05 量化）**：仅当「我方岛数 − ORIG 岛数 ≥ 2」才试 —— 本件 7−0=7
//   远过闸门（正例）；差 1 岛的多为 4.4 rel8 跳板/epilogue-threading。同 TU 反例：
//   `add_monster_list_bossroom_condition` 三守卫同目标合并**零收益**（176/ORIG 173）；
//   正例参照 `CBattle_Field::choose_random_bossmap_from_dummymaps` 尾部岛 5→3 ⇒ DIFF→AE 236=236。
//   **三手协议（Main/C05③ 定稿）**：① 守卫为**纯条件短路（无 call）** ⇒ 本手 `||` 合并
//   （本件即此，命中）；② 守卫**含 call/setcc 操作数** ⇒ 改**正向嵌套 `if`**
//   （F3c 实证：`AvatarRechargeServer::CalcDurabilityDetail` ours 96→90=ORIG、DIFF→AE；
//   同件 `||` 合并净零）；③ 两手不成 ⇒ A28 封顶。另：「同目标 jcc 多」≠「我方多岛」
//   —— 计数须落在 `jmp+nop` 岛而非 bool 物化形（F3c `importActionDefine` 2:2 判例）。
void InitializeCharacStat::initCharacStat(_Additioal_info& info,
                                          unsigned int characNo, int job,
                                          int level, int grow1, int grow2)
{
    // ORIG 0x08145e70..0x08145ed3：七条独立守卫各自 jne 直跳函数尾
    // [D08 2026-09-10] C05 范式③试验：七守卫合并为单条 `||` 短路 goto
    // （4.4 对短路 or 逐条 test/jne 直达同一尾标签，企消 7 组 jmp+nop 岛）。
    if (info.m_field08 != 0 || info.m_field0c != 0 || info.m_field0a != 0
        || info.m_field0e != 0 || info.m_field00 != 0 || info.m_field04 != 0
        || info.m_field42 != 0)
        goto L_done;

    info.clear();
    if (job < 0 || job > 10 || level > 0x46 || level < 1)
    {
        // [N1D 2026-09-11 试形·负结论] trace1 匿名临时形（`cMyTrace(...)(...)`）
        //   实测槽仍为 -0x48（4.4 对具名/临时同区分配）⇒ 零收益，已回滚。
        //   ORIG 槽序 -0x58(trace1)/-0x48(trace2)/-0x38(trace3) 与我方 -0x48/-0x58/-0x38
        //   的 trace1↔trace2 交换属 4.1↔4.4 栈槽分配策略差（不可控）。
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x1f, 5);
        trace("[ARAD,STAT,RESET] Invalid character job. charac %u, job %d, level %d",
              characNo, job, level);
        goto L_done;
    }

    {
        // ORIG 0x08145f4c..0x08145fb9：结构体拷贝（对齐剥离 + rep movsl + 尾字节）
        // [M2S2 2026-09-11 复测] N18a 第 5 形实测（新钥匙「类型化下标取址」
        //   `&m_characList->m_characs[job]`，替代下方 char* 手工算术）：实地编译
        //   300 条不变（gap 仍 +1），形变为 `imul→%eax; lea (%edx,%eax,1),%eax;
        //   mov %eax,%edx`（中转仍在，且 imul 目标转为 %eax）⇒ 零收益，已回滚。
        //   与 D08 探针 /tmp/addr_probe.cpp g1 读数一致，N18a 维持。
        // [N1D 2026-09-11 复测·n1_v 口径] 本件三段残差全部试毕，均判后端封顶：
        //   ① trace 槽序：ORIG trace1(-0x58)/trace2(-0x48)/trace3(-0x38) vs
        //      我方 trace1(-0x48)/trace2(-0x58)/trace3(-0x38)。本波试形：
        //      trace1 加嵌套块（`{ { cMyTrace ... } }`）与 trace1 改匿名临时
        //      （`cMyTrace(...)(...)`）两形实测槽**均不变**（仍 -0x48）⇒
        //      4.1↔4.4 栈槽分配策略差，源码不可控。
        //   ② 基表源地址：ORIG 0x08145f5c `mov 0x14(%eax),%ecx; mov 0x10(%ebp),%eax;
        //      imul $0x7dc,%eax,%edx; lea (%ecx,%edx,1),%edx`（4 条）vs 我方
        //      `mov 0x14(%eax),%eax; mov %eax,%edx; mov 0x10(%ebp),%eax;
        //      imul $0x7dc,%eax,%eax; add %eax,%edx`（5 条，多 1 条中转）。
        //      本波试形：交换操作数（`job*0x7dc + (char*)list`）实测全同 ⇒ 回滚。
        //      N18a（D08 探针 g0–g6 七形亦不可造）。
        _Additioal_info base = *reinterpret_cast<const _Additioal_info*>(
            reinterpret_cast<const char*>(G_CDataManager()->m_characList) +
            job * 0x7dc);

        const void* stat1 =
            G_CDataManager()->m_characList->m_characs[job].get_growtype_stat(0, 0);
        const void* stat2 =
            G_CDataManager()->m_characList->m_characs[job].get_growtype_stat(grow1, 0);
        const void* stat3 =
            G_CDataManager()->m_characList->m_characs[job].get_growtype_stat(grow1, grow2);
        if (stat1 == 0 || stat2 == 0 || stat3 == 0)
        {
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0x2d, 5);
            trace("[ARAD,STAT,RESET] Invalid character grow type. charac %u, first %d, second %d",
                  characNo, grow1, grow2);
            goto L_done;
        }

        info = base;
        for (int i = 1; i < level; ++i)
        {
            if (i <= 0xe)
                info.add(*reinterpret_cast<const _Additioal_info*>(stat1));
            else if (i <= 0x31)
                info.add(*reinterpret_cast<const _Additioal_info*>(stat2));
            else
                info.add(*reinterpret_cast<const _Additioal_info*>(stat3));
        }

        // ORIG 0x0814616e..0x081461e8：12 个 vararg 全为表达式（右起求值，前 9 个 spill、
        // 末 3 个落 %edi/%esi/%ebx）⇒ 不设具名局部。
        cMyTrace(__PRETTY_FUNCTION__, 0x41, 0)(
            "[ARAD][RESET STAT] Init Character : %d, stat : hp %d, mp %d, pyatt %d, pydef %d, mgatt %d, mgdef %d, speed %d, element %d, element %d, element %d, element %d)",
            characNo, (int)info.m_field00, info.m_field04, (int)info.m_field08,
            (int)info.m_field0a, (int)info.m_field0c, (int)info.m_field0e,
            info.m_field42, (int)(short)info.m_field10[0],
            (int)(short)info.m_field10[1], (int)(short)info.m_field10[2],
            (int)(short)info.m_field10[3]);
    }
L_done:;
}