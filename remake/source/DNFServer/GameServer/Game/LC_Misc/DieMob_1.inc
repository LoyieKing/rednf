void Secu_HackLogCheck::DieMob(const MSG_MONSTER_DIE* msg)
{
    if (msg->m_fieldF == -1)
        return;

    SetMonsterInfo(msg);

    // b28d-36 判词（档① callee 侧旋钮，待 Main 代落 CUser.h）：ORIG
    // 0x0827a84x 为 `call Check223; mov 0x8(%ebp),%edx; mov %al,0x38(%edx)`
    // （LHS-first 无中转），要求 callee 返回类型与 uchar 字段**同型**。
    // 探针 /tmp/d36_probe_dm.cpp：t2（返回 unsigned char）逐条命中，t1/t3
    // （返回 char，含调用点 cast 形）恒多 `mov %eax,%edx` 中转（P211 档③）。
    // Check223 尾部 ORIG `movzbl -0x11(%ebp),%eax` 亦证返回 uchar。
    // ⇒ 正解 = CUser.h:1732-1734 Check223/224/225 返回 char→unsigned char。
    if (m_flag223 != 0)
        m_flag223 = Check223();
    if (m_flag224 != 0)
        m_flag224 = Check224();
    // b28d-solo-LC_Misc 判词 [N(P223/P63 epilogue-threading)]：ORIG 0x0827a882
    // 第三支 `je 0x0827a895`（leave）与早退出口同址，body 直落 epilogue；4.4 对
    // 平铺 `if(flag!=0){body}` 恒多 `jmp <leave>; nop` 中继岛（fresh 实测 42 条 vs
    // ORIG 41 条，p167 调用点差集 0/0 CLEAR）。三形实测均不消：①否定早返
    // `if (flag == 0) return;` → 两个岛（43 条）；②共享尾标签 `goto end; … end:;`
    // → 仍 jmp+nop；③显式 return/空 else 形反劣。P223 边界：4.4 自身
    // epilogue-threading 尾岛不可造（ORIG 无岛、我方多出）⇒ 语义等价封顶。
    if (m_flag225 != 0)
        m_flag225 = Check225();
