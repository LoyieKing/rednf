// ORIG 0x083020b6：概率生成单只 APC（mobId 固定 0x40，roleType 5）。
// [b28d-solo-BattleMonsterDepsC2] 已修：uid 递增改 `uid = uid + 1`（ORIG `lea 0x1(%eax)` 复用
// 寄存器；`mob.m_instId + 1` 会重读 mob+0x4）；m_level 条件加 (signed char)。
// [SweepBattleMonster 2026-09-09] 已修（140 vs 146）：①`int mobId=0x40` 具名局部 + `mobId++`
// 后置（ORIG 0x083020d2 store / 0x08302296 `addl $0x1,-0x10`）；②P0 删 `map_monster tmp` 直接
// `Add_Mob(mob)`（帧 0x6b0→0x370=ORIG）；③m_field11 字节视图双 movb；④P231 register 复用 uid。
// 残差 N：rate()!=0 的 `setne` 与 rate()>rand 的 `setg` 物化（P226 语句表达式实测 +5 条反劣）、
// dim>0 的 if/else 尾部三 jmp 岛、uid 落 %ebx（N7）。
void CBattle_Field::add_monster_APC_AI_by_rate(map_monster& mob, int& uid, int x, int y)
{
    if (m_pDungeon == NULL)
        goto apc_end;   // [round5-C05] P223 尾标签串：ORIG 三守卫直跳 epilogue
                        // （仅 1 个对齐 nop 岛）；实验「全 return;」实测尾部
                        // 中继 jmp ×3（ndiff 6→8）⇒ 保留单尾标签，判 N。
    // P0：ORIG 0x083020e5 `movl $0x40,-0x10(%ebp)`（具名局部，非直填立即数）
    int mobId;
    mobId = 0x40;
    if ((m_endX == x && m_endY == y) ||
        (m_startX == x && m_startY == y))                                  // +0xc/+0x10、+0x4/+0x8
    {
        goto apc_end;
    }
    // [b23S3C] P48 双取：ORIG getAICharacterAppearRate() 每判各调一次
    // （非空检查 1 次、rand 比较 1 次存 %ebx 跨 get_rand_int call）；
    // EXTRA map_monster C1ERKS/D1 = 4.4 Add_Mob 大对象 by-value caller
    // 复制（P67 跨版本 ABI 豁免）。
    // ORIG 首条件 `test; setne %al; test %al,%al; je`、次条件 `cmp %eax,%ebx; setg %al;
    // test; je`。
    // [b28f-L27·P225 推广] 拆**嵌套 if**（合并短路链被 4.4 折成直跳 `je/jle`，恒不产
    //   setcc）⇒ 两处 setne/setg 物化逐条复现，142→141。
    //   余 2 条 = ①`mob.m_mobId = mobId` 的 RHS-first+`mov %eax,%edx` 中转（P211 档③
    //   要求 map_monster::m_mobId 与 RHS 异型，属 MonsterIndependentDrop.h 建模，
    //   跨 TU 勿动）；②尾部我方多 3 个 `jmp` 岛（P223 早返岛数，ORIG 1 个）⇒ 按 N。
    if (m_pDungeon->getAICharacterAppearRate() != 0)
    {
      if (m_pDungeon->getAICharacterAppearRate() > get_rand_int(100))
      {
        CAICharacter* ai =
            G_CDataManager()->m_aiCharacterList->getByLevel(
                static_cast<unsigned int>(m_pDungeon->get_standard_level()));
        if (ai == NULL)
            goto apc_end;

        std::memset(&mob, 0, sizeof(map_monster));  // ORIG 显式 memset 0x34c
        // [b28d-Lane27] ORIG 0x083021ac 取旧值写 mob+0、自增在**函数尾** 0x08302296
        // （Add_Mob 之后 `addl $0x1,-0x10(%ebp)`）⇒ 拆两句并把 `++mobId` 后置。
        mob.m_mobId = mobId;
        // [S6 2026-09-11] 残差 N：ORIG 0x08302181 `call getIdx; mov %eax,%edx;
        // mov 0xc(%ebp),%eax; mov %edx,0xc(%eax)`（3 条）vs 4.4 `mov 0xc(%ebp),%edx;
        // mov %eax,0xc(%edx)`（2 条，少 1 条中转）。显式 `static_cast<int>` 旋钮实测
        // 无效（ndiff 6 不变，已回滚）⇒ 4.1 分配差异，判 N7。
        mob.m_mobIndex = ai->getIdx();
        // P231：ORIG 读 uid 入 %eax 后跨 store 复用（`lea 0x1(%eax),%edx`）⇒ 源侧写成
        // 单条后置自增（一个求值点），4.4 只读一次 uid；两句 `= uid; uid = uid+1;`
        // 因引用别名保守判定会重读 uid（+1 条，已实测反劣）。
        mob.m_instId = uid++;
        mob.m_roleType = 5;
        // [b23S3C] P47：ORIG 真臂 fall-through（dim>0 读 +0x238），getLevel
        // 在 je 臂；m_field11 short=0 产单 movw（ORIG 0x11/0x12 双 movb，
        // 字段拆分属 Monster 域批次）。
        if (static_cast<signed char>(m_pDungeon->get_dimension_possible()) > 0)
            mob.m_level = static_cast<char>(m_standardDimensionLevel);
        else
            mob.m_level = static_cast<char>(ai->getLevel());
        mob.m_dropCnt = 100;
        mob.m_field11b.m_field11b0 = 0;
        mob.m_field11b.m_field11b1 = 0;

        GetCurrentMapInfo()->Add_Mob(mob);
        ++mobId;                                    // ORIG 0x08302296 尾置自增
      }
    }
apc_end:
    ;
}