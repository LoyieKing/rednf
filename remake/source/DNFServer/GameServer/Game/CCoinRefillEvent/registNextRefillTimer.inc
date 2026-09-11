void CCoinRefillEvent::registNextRefillTimer() const
{
    // [b28d-59 判词] （ORIG 0x0832c1fe）：93/93 条、P167 调用点 9/9 CLEAR。
    // [L6S2b 形修正 · P0/P183/P231] ORIG 该函数有两处「值无槽往返、驻 callee-saved
    //   %ebx 跨 call 保活」：① `mov -0x20(%ebp),%edx; mov -0x1c(%ebp),%eax;
    //   mov %edx,%ebx; sub %eax,%ebx; call G_TimerQueue; … mov %ebx,0x10(%esp)`
    //   （0x0832c2c9..2e2，delay 实参）；② `call ctime; mov %eax,%ebx; … call
    //   cMyTrace::C1; mov %ebx,0x8(%esp)`（0x0832c311/336，ctime 结果）。
    //   我方形为槽（`mov %eax,-0x10/-0xc(%ebp)` + 回读）且无 `push %ebx`（帧 0xa8 vs 0x94）。
    //   修法：①删具名 `int delay`、表达式内联回实参位；②`char*` 加 `register`
    //   存储类（裸 register 属 P231/P307 命中形：值跨 callee call 保活 + 4.4 不自
    //   升格 + ORIG 目标为 callee-saved，非 asm 绑定故 P0 合规）。
    //   探针 /tmp/L6S2_probe/pr_tq.cpp（g_named 槽/无 push vs g_inline ebx+push）与
    //   pr_ts.cpp（h_plain 槽 vs h_reg ebx+push，h_inline 会改 ctime 调用序故不可用）
    //   均与 ORIG 逐形一致。
    // [M4S2 · P339 全头部声明 + 声明序按 ORIG 槽深→浅] ORIG 0x0832c1fe 槽图
    //   cur=-0x78 / next=-0x4c / nextSec=-0x20 / curSec=-0x1c / trace=-0x18，
    //   即三件 4B/16B 的槽**自低向高按声明序**紧密排布（next 尾 -0x20 → nextSec
    //   → curSec → trace）。旧译把 curSec/nextSec 写成带初值的语句级声明、trace
    //   在内层块 ⇒ 我方槽序反为 trace=-0x20 / nextSec=-0x10 / curSec=-0xc
    //   （NEAR 残差唯一来源）。改为头部纯声明 + 语句级赋值后逐槽对齐。
    tm cur;
    tm next;
    time_t nextSec;
    int curSec;
    curSec = GlobalData::s_systemTime_.getCurSec();
    localtime_r(reinterpret_cast<time_t*>(&curSec), &cur);
    next = cur;
    next.tm_hour = getNextEventTime(cur);
    if (next.tm_hour < cur.tm_hour)
    {
        next.tm_mday = next.tm_mday + 1;
    }
    next.tm_min = 0;
    next.tm_sec = 0;
    nextSec = mktime(&next);
    if (nextSec - static_cast<time_t>(curSec) >= 0)
    {
        G_TimerQueue()->InsertTimer(static_cast<TimerEntry::OBJ_TYPE>(2), 0,
                                    static_cast<TIMER_MESSAGE>(0x65),
                                    static_cast<int>(nextSec) - curSec, 0, 0);
        register char* timeStr = ctime(&nextSec);
        cMyTrace trace(__PRETTY_FUNCTION__, 0x88, 0);
        trace("Next coin refill time is %s", timeStr);
    }
    else
    {
        // [M4S2 · P223 尾岛] ORIG 0x0832c2bd..0x0832c350 形为
        //   `js +0x151`（落 nop）→ 体内三条语句 → `jmp +0x152` → `+0x151: nop`
        //   → epilogue，即 then 块尾有跳过 else 岛的 jmp、失败落点另有空标签岛。
        //   旧译裸 if（无 else）产 `js <epilogue>` 直连，缺这 2 条（gap +2 来源）。
        //   探针 /tmp/m4s2_probe/pr_tail.cpp：f6(`if (c>=0){g();h();} else {return;}`)
        //   产 `js L_else; call; call; jmp L_end; L_else: nop; L_end:`（= ORIG 逐条），
        //   f1..f5（裸 if/空 else/空语句）均不产 jmp+nop。语义不变（void 尾 return）。
        return;
    }
    // [M4S2 R2 复核 · 尾岛已修；残差 = N95/N126 槽映射族]
    //   gap 已 +2→0（原缺 `jmp +0x152`/`nop +0x151` 两条，见上）。剩余唯一差异是
    //   三件局部槽的**排列**：ORIG 0x0832c208/0x0832c2ac/0x0832c32b 槽序（低→高）
    //   = nextSec(-0x20) < curSec(-0x1c) < trace(-0x18)；我方 = trace(-0x20) <
    //   nextSec(-0x10) < curSec(-0xc)（本形）。两形均实测：
    //   ① 语句级带初值声明（`int curSec = ...;` / `time_t nextSec = ...;`）⇒
    //      curSec(-0xc) < nextSec(-0x10) < trace(-0x20)；
    //   ② P339 全头部纯声明 + 语句级赋值（本形）⇒ nextSec(-0xc) < curSec(-0x10)
    //      < trace(-0x20)：4B 槽序已对齐，但 16B 块内非 POD 对象 trace 恒落
    //      `next` 之下的第一个空位 -0x20，不再上移。
    //   ⇒ 4.4 对块内非 POD 对象的槽分配时机早于外层标量，ORIG 的 trace 位于标量
    //      之上非源码可及 ⇒ 保留本形（DIFF→NEAR，指令序列除槽号外逐条一致）。
}