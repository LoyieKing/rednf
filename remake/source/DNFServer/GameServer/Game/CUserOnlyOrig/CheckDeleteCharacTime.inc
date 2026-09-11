// [b28d-solo-CUserOnlyOrig AE] ORIG 0x0864a830：dead gap 局部 4→8 字节
// （unsigned int→unsigned long long）复现 4.1「tt 槽基 -0x20 + 上方 8B 空洞」
// （帧 0x34 不变：4.4 带 call 帧恒 16k+4 量化，locals+outgoing 44→48 仍落 0x34）
// ⇒ AE。P232/P31 扩展：dead 局部宽度按 ORIG 空洞宽度定。
bool CUser::CheckDeleteCharacTime(int slot)
{
    // ORIG 0x0864a830（报告 CheckDeleteCharacTime.md 全文转写）。
    // CommonTime 6 字节写入视图（+0 年+?/+1 月/+2 日/+3 时/+4 分，SetCurTime
    // 0x82a4224..0x82a427f 逐条 movb 实证）。ORIG 槽基 -0x20（8B）⇒ char[8] 缓冲；
    // 技巧31：前置 0x10 dead 缓冲复现 4.1 帧 0x34（outgoing 与 locals 间空洞）
    //   空洞，删一个 gap 把 tt 推回 -0x20（P31）。[b28 复验] P190 double pad
    //   反把 tt 推至 -0x28（本件槽基差 4、帧已等 ⇒ 非对齐进位场景），回退；
    //   [b28d-solo 定稿] 残差真因 = dead gap 局部宽度：4.4 给 4B gap 占 -0x14
    //   使 tt 落 -0x1c；改 8B（unsigned long long）后 gap 占 -0x18..-0x11、
    //   tt 回 -0x20，帧仍 0x34 ⇒ AE（P232/P31）。
    unsigned long long frameGapA;
    (void)frameGapA;
    char tt[8];                           // 后声明=远槽 → 基址 -0x20（ORIG）
    reinterpret_cast<CommonTime*>(tt)->SetCurTime();       // ORIG 0x0864a837 lea -0x20; call
    // ORIG 0x0864a85a/0x0864a87e/0x0864a8a2：三段比较各调一次 operator[]，
    // 逐字节 `movzbl 0x39+k(%eax),%edx; movzbl -0x20+k(%ebp),%eax; cmp %al,%dl`；
    // 4.1 && 物化 bool 岛（+0x7e mov$1 / +0x85 mov$0 / test;je）⇒ 具名 bool
    // 技巧22：register bool asm("al") 免 -0x11 槽，物化后直 `test %al,%al; je`
    // P0/N96 去绑定：ORIG 物化岛尾 `test %al,%al; je` 直测无槽往返 ⇒ 原码无具名
    // bool（P0），删预清；4.4 槽回写 vs ORIG 顶寄存器直测残差记跨版本 N。
    // [b28 复验] P225 while-break 形在本件反劣（ndiff 3→50：多出口 body 被
    // 4.4 loop-exit 重排整块后移，P225 适用边界=单 fall body）——回退具名形。
    // [b28d-solo-CUserOnlyOrig P226] ORIG 物化岛尾 `test %al,%al; je` 无槽往返
    // ⇒ 原码无具名 bool；改用语句表达式包住整个条件（P226 条件向），消 -0x11 槽
    // 与 cmpb 回读。
    if (({ (unsigned char)m_characList[slot].m_delReqTime[0] == (unsigned char)tt[0]
            && (unsigned char)m_characList[slot].m_delReqTime[1] == (unsigned char)tt[1]
            && (unsigned char)m_characList[slot].m_delReqTime[2] == (unsigned char)tt[2]; }))
    {
        // ORIG 0x0864a8da-0x0864a8eb：ci 小时 *60 结果跨 call 存 %ebx（shl 链
        // mov %edx,%ebx; sub %eax,%ebx）⇒ register asm("ebx") 钉 callee-saved
        int req = (int)m_characList[slot].m_delReqTime[3] * 60   // ORIG call +0x92
                  + (int)m_characList[slot].m_delReqTime[4];     // ORIG -0x10 (+0xd0 二次 call)
        int now = (int)tt[3] * 60 + (int)tt[4];             // ORIG -0xc（本地槽直读）
        // ORIG 0x0864a930：`mov -0x10,%eax; mov -0xc,%edx; mov %edx,%ecx;
        // sub %eax,%ecx; mov %ecx,%eax; cmp $0x59f,%eax; jg 岛(mov$1)`
        // 技巧23：goto 钉块序——ORIG `jg 岛(mov$1)`/`mov$0` fall-through，
        // RET1 岛与外层 return true 共享（0x0864a94a）
        if (now - req > 0x59f)
            goto RET1;
        return false;
    }
    RET1:
    return true;
}