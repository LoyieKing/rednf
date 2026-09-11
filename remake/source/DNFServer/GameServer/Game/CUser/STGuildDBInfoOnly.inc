// STGuildDBInfoOnly ctor（ORIG W 0x08693f56 sz 0x16f；无报告，objdump 转写：
// 逐字段清零 → 循环构造 16×STGuildSkill（+0x45 步长 5）→ 尾部标量清零 →
// memset 子成员 → +0x1c (x|1)&~2 → memset skills 覆盖 → memset masterName）。
// 注意 ORIG 在循环构造 STGuildSkill（-1/0xff）后又 memset(+0x45,0,0x50) 覆盖清零，
// 净效果 skills 全 0（CUser.h 冻结 m_skills 为 char[0x50]，无构造语义，直接清零）。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08693f56 | _ZN17STGuildDBInfoOnlyC1Ev
// 2026-09-11 CuA 探针终判（/tmp/P5/p_bits b.cpp g1..g6）：`b |= 1`、`b = b|1`、
//   具名读-改-写、`b &= ~2` 等 6 形在 4.4 下**全部**产 5 条
//   `mov %eax,%edx; movzbl 0x1c(%eax),%eax; or $1,%eax; mov 0x8(%ebp),%eax; mov %al,0x1c(%edx)`，
//   而 ORIG 0x08694069 是 4 条的 `movzbl 0x1c(%eax),%edx; or $1,%edx; mov %dl,0x1c(%eax)`
//   （this 保 %eax、值走 %edx，且后续重载 this）⇒ 4.4 不产该分配，判 N（勿再改写法）。
// b28d-GrpD 按 ORIG 0x08693f56 直证改写（两处）：
//  ① ORIG 生成序 = [0x17..0x44 逐字段 movl/movb/movw] → [16× call STGuildSkill::C1Ev
//     循环 @+0x62] → [0x95..0xb9 逐字段 @+0x7f] → [函数体 memset…]。循环夹在标量
//     赋值**中间**，只可能是**成员初始化列表**（gcc 按声明序：m_skillLearnCnt@0x44
//     → m_skills@0x45 隐式构造 → m_field95@0x95）；我方旧「体内赋值」写法会把
//     m_skills 隐式构造顶到函数最前（实测我方 -0x20 帧、循环在首条）。
//     逐条指令核对：+0x0b movl 0x17 / +0x11 movb 0x1b / +0x18 movl 0x1e / +0x1f movw 0x22
//     / +0x26 movl 0x24 / +0x2d movb 0x28 / +0x34 movl 0x29 / +0x3b movb 0x2d /
//     +0x42 movw 0x42 / +0x49 movb 0x44 —— 与下列列表顺序一一对应。
STGuildDBInfoOnly::STGuildDBInfoOnly()
    : m_masterNo(0)                 // +0x17
    , m_fundLevel(0)                // +0x1b
    , m_field1E(0)                  // +0x1e
    , m_memberCount(0)              // +0x22
    , m_guildPoint(0)               // +0x24
    , m_guildRank(0)                // +0x28
    , m_fundExp(0)                  // +0x29
    , m_subMasterCnt(0)             // +0x2d
    , m_guildSkillPoint(0)          // +0x42
    , m_skillLearnCnt(0)            // +0x44
    , m_field95(0)                  // +0x95
    , m_powerSecedeTime(0)          // +0x96
    , m_powerWarPoint(0)            // +0x9a
    , m_agitExist(0)                // +0x9e
    , m_powerJoinCount(0)           // +0x9f
    , m_fundMoney(0)                // +0xa0
    , m_fieldB9(0)                  // +0xb9
{
    memset(m_subMasterData, 0, 0x14);   // +0x2e
    // ② ORIG +0x47/+0x53：+0x1c 为 byte 位操作（`mov 0x8(%ebp),%eax;
    //    movzbl 0x1c(%eax),%edx; or $0x1,%edx; mov %dl,0x1c(%eax)` / `…and $0xfffffffd…`），
    //    只动低字节、不触碰 +0x1d。
    //    [b28d-G5] 旧形 `*(unsigned char*)&m_ability`（指针 cast 后 deref）在 4.4 下
    //    **地址不折进位移**：产 `add $0x1c,%eax` + `add $0x1c,%edx` 双地址与
    //    `(%eax)`/`(%edx)` 间接写读（7 条/站 vs ORIG 4 条/站）⇒ 正是 gap +6 的唯一来源。
    //    解 = 具名视图（P311/P246 组件·union 具型视图）：把 `this` 视作 +0x1c 处为
    //    byte 别名的结构，成员访问即折进位移 `0x1c(%eax)`（无地址临时、无槽）。
    struct GuildAbilityByteView
    {
        char m_pad[0x1c];
        union
        {
            unsigned short m_ability;   // 与 STGuildDBInfoOnly::m_ability 同址（+0x1c）
            unsigned char m_field1C;    // 低字节（ORIG 仅动此字节）
        };
    };
    // 2026-09-11（Lane2，P311 附：复用 memset 返回值）：ORIG 0x08694061 `call memset`
    //   之后 0x08694069 直接 `movzbl 0x1c(%eax),%edx` —— 复用 memset 返回值
    //   （= m_guildName = this）寻址，**无** `mov 0x8(%ebp),%edx` 重载；我方分两句写
    //   会多发 1 条重载。把 memset 内联进同一表达式即复现（探针
    //   /tmp/lane2/probe_memsetret.cpp：f_a ⇒ `movzbl 0x1c(%edx),%edx`，
    //   f_b ⇒ `movzbl 0x1c(%eax),%eax` = ORIG 形）。
    // [CUserLane 2026-09-11 T3 实测无效] P211 档③ 同型化（RHS `(unsigned char)1`）产物
    //   与 `|= 1` 逐条相同（88/90、ndiff 2 不变）⇒ 装载序锁在 memset 返回值复用形上，
    //   维持 N 封顶（第 7 形失败）。
    ((GuildAbilityByteView*)memset(m_guildName, 0, 0x17))->m_field1C |= 1;
    ((GuildAbilityByteView*)this)->m_field1C &= ~2;
    memset(m_skills, 0, 0x50);          // +0x45
    memset(m_masterName, 0, 0x15);      // +0xa4
}