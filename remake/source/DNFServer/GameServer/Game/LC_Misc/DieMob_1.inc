// ---------------------------------------------------------------------------
// ORIG 0x0827a81c（报告 Secu_HackLogCheck/DieMob.md）：
// msg->m_fieldF(+0x0f short) != 0xffff 时：SetMonsterInfo(msg)；m_flag223/224/225
// 非 0 者分别以 Check223/224/225 结果刷新。
// [b28c-27] N18a：ORIG 8429xx `call Check223; mov 0x8(%ebp),%edx; mov %al,0x38(%edx)`
// 为 call 返回 char 单字节直存；4.4 恒经 `mov %eax,%edx` 截断中转（+2 条/处）。
// 删 cast、字段 unsigned 化、bool 化均实测不可翻（P168a③b 边界），语义等价。
void Secu_HackLogCheck::DieMob(const MSG_MONSTER_DIE* msg)
{
    // [P1T4 #284-303] ORIG 0x0827a81c 首支 `cmp $0xffff,%ax; je 0x0827a895`
    // （je 直指共享 leave），第三支 body（0x0827a892 `mov %al,0x3a(%edx)`，3 字节）
    // 直落 0x0827a895 epilogue、**无** `jmp <leave>`+`nop` 中继岛 ⇒ 原码是**包裹形**
    // `if (msg->m_fieldF != -1) { 全体 }`。旧早退形 `if (f == -1) return;` 在 4.4 下
    // 第三支尾恒多 `jmp +0x1; nop`（实测 42 条 vs ORIG 41 条）。
    // 探针 /tmp/dm_probe.cpp：d1（早退平铺）=42 条含岛；d3（包裹形）=41 条逐条命中
    // ORIG（`je` 直指 leave、body 直落）。语义等价：m_fieldF==-1 时函数直接返回。
    if (msg->m_fieldF != -1)
    {
        SetMonsterInfo(msg);

        // b28d-36 判词（档① callee 侧旋钮，待 Main 代落 CUser.h）：ORIG
        // 0x0827a84x 为 `call Check223; mov 0x8(%ebp),%edx; mov %al,0x38(%edx)`
        // （LHS-first 无中转），要求 callee 返回类型与 uchar 字段**同型**。
        // 探针 /tmp/d36_probe_dm.cpp：t2（返回 unsigned char）逐条命中，t1/t3
        // （返回 char，含调用点 cast 形）恒多 `mov %eax,%edx` 中转（P211 档③）。
        // Check223 尾部 ORIG `movzbl -0x11(%ebp),%eax` 亦证返回 uchar。
        // ⇒ 正解 = CUser.h Check223/224/225 返回 char→unsigned char。
        // [P1T4 #284-303 复核] 已落实：CUser.h:1834-1836 现为 `unsigned char`，
        // 本件 Check223/224/225 消费点无中转（本函数 fdiff blocks=0）。
        if (m_flag223 != 0)
            m_flag223 = Check223();
        if (m_flag224 != 0)
            m_flag224 = Check224();
        // [修订·P1T4] 旧判「P223 边界：epilogue-threading 尾岛 4.4 不可消」已推翻：
        // 真因是首支早退形；改包裹形后第三支 `je 0x0827a895` 与 ORIG 同形，尾岛自然
        // 消失（旧三形试验①否定早返②共享尾标签③空 else 的负结论作废）。
        if (m_flag225 != 0)
            m_flag225 = Check225();
    }
}