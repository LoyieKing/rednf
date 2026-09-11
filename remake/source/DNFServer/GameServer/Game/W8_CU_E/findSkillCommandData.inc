// ---------------------------------------------------------------------------
// 1. bool CUser::findSkillCommandData(int skillId, STSkillCommandData& out)
//    @0x08654b94（findSkillCommandData.md ##汇编 + ##反编译 C 逐行转写）
bool CUser::findSkillCommandData(int skillId, STSkillCommandData& out)
{
    // b28d-35: DIFF→N82（帧/槽）。ORIG @0x08654b94 size 0x2d6。
    // [M3S1 2026-09-11 本波总读数] 209 → **199 = ORIG 199（gap 0，ORIG-only/OURS-only 皆空）**。
    //   四旋钮：①两处循环上界 `hit->second.m_count`→**out**.m_count（P0，ORIG 8654c72 实证）；
    //   ②`m_key0` 裸下标 → union `m_keyBytes[i]` 数组视图 + 重读 out（P291/P0，消 4 步 lea）；
    //   ③未定制分支 `size_t n` → **int n** 且条件 `n > 5 || n <= 0`（P196/P290，ORIG jg/setl 有符号）；
    //   ④`int job` 具名局部删 → `getSkillList(get_charac_job())` 右值直传（P0，ORIG %ebx 保活无槽）。
    //   残余 ndiff=102 全为**槽位映射**（我方 cmds=-0x28/迭代器=-0x38/-0x34 vs ORIG -0x24/-0x54/-0x50）：
    //   迭代器/类对象槽受 P331 约束不可按 P339 提头；**帧同为 `sub $0x74`**（逐值相同）
    //   ⇒ 判 N82/N93 族（方向已无 gap，槽位映射不可控）。
    // [旧判词作废] axisB_points.tsv 的 0x8654d47/0x8654d4d 真分叉（ORIG `jg` 有符号 vs
    //   我方 `ja` 无符号）**本波已由 `int n` + `n > 5 || n <= 0` 修掉**（旧记录「改符号性
    //   209→211 劣化」是在 `unsigned char key` 具名局部仍在时测得；删 key 局部后该冲突消失）。
    // 命令表 getter（CUserCharacInfo +0x1254 区容器）：std::map<int,STSkillCommandData>*
    std::map<int, STSkillCommandData>* cmds =
        reinterpret_cast<std::map<int, STSkillCommandData>*>(
            const_cast<char*>(getSkillCommand()));
    if (cmds == 0)
        return false;

    std::map<int, STSkillCommandData>::iterator hit = cmds->find(skillId);
    if (hit != cmds->end())
    {
        out.m_command = hit->second.m_command;
        out.m_count   = hit->second.m_count;
        // [M3S1 本波实测·P0] ORIG 8654c72 上界为 `mov 0x10(%ebp),%eax;
        //   movzbl 0x2(%eax),%eax; movsbl %al,%eax; cmp -0x20(%ebp),%eax; setg`
        //   ⇒ 读 **out**.m_count（第二形参），不是 hit->second.m_count
        //   （后者要多一次 ptEv 调用 + lea，我方原多 1 条）。
        for (int i = 0; i < (signed char)out.m_count; ++i)
        {
            // [旧判词作废] 曾记「m_keyBytes 视图 209→212 / 删 key 局部 209→214」而保留
            //   裸下标；本波复核：当时劣化来自**同处仍读 hit->second.m_count**（多一次
            //   ptEv+lea）与 `size_t` 无符号序的叠加，非视图本身。
            // [M3S1 本波实测·P291/P0] ORIG 8654c37..8654c69 逐条为
            //   `movzbl 0x7(%eax,%edx,1)`（单条 disp+scaled 读 hit->second.m_keyBytes[i]）
            //   → `mov %dl,0x3(%eax,%ebx,1)`（单条写 out.m_keyBytes[i]）
            //   → 重读 `movzbl 0x3(%edx,%eax,1)` 与 7 比较，全程无 key 槽
            // ⇒ 用 union 数组视图 + 重读目标表达式；裸下标 `(&x.m_key0)[i]` 4.4 会发 lea 链。
            out.m_keyBytes[i] = hit->second.m_keyBytes[i];
            if (out.m_keyBytes[i] == 7)
                out.m_keyBytes[i] = 8;
        }
        return true;
    }

    // 未定制：取当前职业技能列表（ORIG G_CDataManager()+0x10 → CSkillList 实例指针）
    // [M3S1 本波实测·P0] ORIG 8654c96 `call get_charac_job; mov %eax,%ebx; call
    //   G_CDataManager; … mov %ebx,0x4(%esp)` —— job 值无槽往返（%ebx 保活）⇒ 原码
    //   是**表达式右值直传**（4.4 对 `receiver->f(arg)` 先求 arg 再求 receiver，与
    //   ORIG 同序）；具名 `int job` 局部多出 `mov %eax,-0xN` + `mov -0xN,%edx`（+1 条）。
    __gnu_cxx::hash_map<int, CSkill*>* list =
        G_CDataManager()->m_skillList->getSkillList(get_charac_job());
    if (list == 0)
        return false;

    __gnu_cxx::hash_map<int, CSkill*>::iterator sit = list->find(skillId);
    // [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N82，gap=-1/blocks=34）→ 试=**守卫反置**：
    //   ORIG 08654d02 `call operator!=`（`_ZNK…_Hashtable_iteratorI…EEneERKSD_`，p167 直证；
    //   我方原写 `sit == list->end()` 产 `…EEeqERKSD_`）+ ORIG 的 0x2113 trace 块在
    //   **函数尾**（08654e2a +0x296，`je` 目标）而非比较点内联 ⇒ 源码是
    //   `if (sit != list->end()) { 主体; return true; } <trace 0x2113>; return false;`
    //   （4.4 按源码块序发射）。反置后：**DIFF→NEAR，gap −1→+0（199=199）**、
    //   diff 行 214→149、p167 该件 CLEAR（0 MISSING/0 EXTRA）。
    //   残余 149 行**全为槽位映射**（sit/end 临时：ORIG -0x5c/-0x4c vs 我方 -0x34/-0x2c；
    //   n：-0x20 vs -0x1c；帧 `sub $0x74` 两侧全等、cmds 槽 -0x24 两侧全等）
    //   ⇒ 4.1/4.4 临时槽分配器差异（N82），源码不可控 ⇒ 维持封顶。
    if (sit != list->end())
    {
        // [M3S1 本波实测·P0] ORIG 8654d1a/8654d26 为**两次** ptEv：`mov 0x4(%eax),%eax;
        //   mov %eax,-0x18` （skill）与 `mov (%eax),%eax; mov %eax,-0x14`（key）⇒ 原码有
        //   具名 key（int）；我方原用 `sit->first` 表达式在三处使用 ⇒ 多两次 ptEv（-3 条）。
        CSkill* skill = sit->second;          // node.first=键（与 skillId 相同）、second=CSkill*
        int key = sit->first;
        // [M3S1 本波实测·P196/P290] ORIG 8654d43..8654d4d 为 `cmpl $0x5,-0x10(%ebp);
        //   jg 8654d4f` + `cmpl $0x0,-0x10(%ebp); jg 8654da3`（**有符号**），循环底
        //   8654e19 `cmp -0x10(%ebp),%eax; setl`（有符号）⇒ n 是 int，条件写
        //   `n > 5 || n <= 0`（`!(n<6)` 在 size_t 下产 `ja`，且 `n >= 6` 产 jge 而非 jg）。
        int n = (int)skill->m_keyCommandList.size();
        if (n > 5 || n <= 0)
        {
            if (n != 0)
            {
                cMyTrace trace(__PRETTY_FUNCTION__,
                               0x2102, 5);
                trace("SKILL_COMMAND_LOG : ABNOMAL COMMAND CNT!! (%d) (%d)",
                      key, (int)n);
            }
            return false;
        }

        out.m_command = (unsigned short)key;      // ORIG 8654da3 从 -0x14 槽读
        out.m_count   = (unsigned char)n;
        // [轴 B 实测·本函数不适用] ORIG 0x08654e19 虽为 `cmp -0x10(%ebp),%eax; setl`
        // 有符号序（P196 前置通过），但本函数改 `int i; i < (int)n` 实测
        // **209→211 劣化**（`unsigned char key` 具名局部的 `movzbl` 链与 int 循环
        // 的寄存器分配冲突）⇒ 回退 `size_t`；残差归 N82/N93（槽向 + getSkillCommand
        // 具名缓存不可去），已按 P196/S1「一次旋钮、无改善即回滚」留痕。
        for (int i = 0; i < n; ++i)
        {
            // [M3S1 本波实测·P291/P0] ORIG 8654dc2..8654e21 同族：单条 scaled 读
            //   `mov (%eax),%eax`（vector<int>::operator[] 结果直传，无 v 槽）→ 单条写
            //   out.m_keyBytes[i] → 重读 out.m_keyBytes[i] 与 7 比较 ⇒ 删 `int v` 具名局部，
            //   表达式右值直传 + 数组视图。
            out.m_keyBytes[i] = (unsigned char)skill->m_keyCommandList[i].m_ids[0];
            if (out.m_keyBytes[i] == 7)
                out.m_keyBytes[i] = 8;
        }
        return true;
    }
    cMyTrace trace(__PRETTY_FUNCTION__,
                   0x2113, 5);
    trace("SKILL_COMMAND_LOG : KEY INDEX DUPLICATE!!");
    return false;
}