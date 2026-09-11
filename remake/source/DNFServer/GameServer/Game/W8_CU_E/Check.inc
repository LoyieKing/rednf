// ---------------------------------------------------------------------------
// 10. bool CUser::st_monstorkill::Check(unsigned int nowTick, unsigned short nx,
//         unsigned short ny, unsigned short xLim, unsigned short yLim,
//         unsigned int windowLimit)
//     @0x0823049c size 0x1cd（objdump 直转）
// [P8S1 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 DIFF 系 objdump 无 -r 的假残差
bool CUser::st_monstorkill::Check(unsigned int nowTick, unsigned short nx,
                                  unsigned short ny, unsigned short xLim,
                                  unsigned short yLim, unsigned int windowLimit)
{
    // ORIG 块布局（82304cc `ja 823060e` / 82305f3 `jge 8230607` / 8230605 `jl 823065e`）：
    // 超窗 reset 块落在**函数尾部**（823060e..823065a，紧接尾 `mov $0x0,%eax`），
    // 且 `return true` 块（8230607 `mov $0x1,%eax; jmp epilogue`）夹在主体与 reset
    // 块之间 ⇒ 原码用 **goto 尾标签**（P283/P223 形），非「早返块内联 + 尾部 return」。
    if ((unsigned int)(nowTick - m_lastTick) > windowLimit)   // ORIG 82304cc: cmp/ja 无符号超窗
        goto L_reset;

    ++m_count;                                            // ORIG 82304dd: (+0xc)+1

    if ((int)(short)m_minX - (int)(short)nx > 0)          // 82304e9: movswl/cwtl 32位有符号差
        m_minX = nx;
    if ((int)(short)m_minY - (int)(short)ny > 0)          // 8230521
        m_minY = ny;
    if ((int)(short)m_maxX - (int)(short)nx < 0)          // 823053d: 旧值<新值才更新（保 max）
        m_maxX = nx;
    if ((int)(short)m_maxY - (int)(short)ny < 0)          // 823055f
        m_maxY = ny;

    m_deltaX = (short)((unsigned short)m_maxX - (unsigned short)m_minX);  // 8230577: 16位差直存 +0x10
    m_deltaY = (short)((unsigned short)m_maxY - (unsigned short)m_minY);  // 8230594 16位差直存 +0x12

    m_deltaX = (short)std::abs((int)(short)m_deltaX);     // 82305b9: cwtl/sar31/xor/sub 绝对化回写
    m_deltaY = (short)std::abs((int)(short)m_deltaY);     // 82305c9

    // ORIG 82305f3 `jge 8230607`（true 出口被 A 真与 B 假双到达）+ 8230605 `jl 823065e`
    // （false 出口在 reset 块**之后**，即 `jl` 直跳末尾 mov $0）⇒ 源形是
    // `if (A || B) return true; else goto L_false;`（短路 `||`；B 为 yLim 侧的 `>=`）。
    // [M3S1 实测] 拆成两个 `if (x) goto` 派 4.4.7 在标签处插 nop+jmp（162）；合并 `||` 后
    //   `jge L_true`/`jl L_false` 与 ORIG 逐条同构（f14 探针 /tmp/m3s1/）。
    if (((int)(short)m_deltaX >= (int)xLim)               // 82305e6..f3: jge → true 块
        || ((int)(short)m_deltaY >= (int)yLim))           // 82305ff..05: jl → false 块
    {
        return true;
    }
    else
    {
        goto L_false;
    }
L_reset:
    // 超窗重置检测框（823060e..823065a，fall-through 到尾 mov $0x0）
    m_deltaX = 0;
    m_deltaY = 0;
    m_count = 0;
    m_lastTick = nowTick;
    m_minX = nx;
    m_minY = ny;
    m_maxX = nx;
    m_maxY = ny;
L_false:
    return false;                                         // 823065e: mov $0x0,%eax
}