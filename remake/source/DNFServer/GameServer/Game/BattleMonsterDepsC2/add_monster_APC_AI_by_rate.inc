void CBattle_Field::add_monster_APC_AI_by_rate(map_monster& mob, int& uid, int x, int y)
{
    if (m_pDungeon == NULL)
        goto apc_end;   // [round5-C05] P223 尾标签串：ORIG 三守卫直跳 epilogue
                        // （仅 1 个对齐 nop 岛），平铺 `return;` 令 4.4 各发
                        // `jmp epilogue;nop` 中继岛 ×4 ⇒ 改单尾标签。
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
        mob.m_mobIndex = ai->getIdx();
        // P231：ORIG 读 uid 入 %eax 后跨 store 复用（`lea 0x1(%eax),%edx`）⇒ 裸 register
        // 提示（非 asm 绑定，P0 合规）。
        register int curUid = uid;
        mob.m_instId = curUid;
        uid = curUid + 1;
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
