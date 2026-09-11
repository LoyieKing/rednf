// _ZN10CSimpleLog9_WriteLogEPKc @ 0x0854fbfc
bool CSimpleLog::_WriteLog(char const* msg)
{
    if (m_fp == 0) {
        return 0;
    }
    DateTime dt;
    if (dt.DiffDay(m_dt)) {
        CloseFile();
        if (!CreateLogFile(dt)) {
            return 0;
        }
    }
    // [b28d-solo-CSimpleLog] _ZN10CSimpleLog9_WriteLogEPKc @0x0854fbfc
    // 判词：N 封顶（N18a + P75/P183 镜像：4.1 地址值跨分支保活 vs 4.4 恒重物化），语义等价。
    //   ORIG 4.1 形（memcpy 段 10 条，&m_dt 留 %eax 跨 je）：
    //     mov this,%eax; add $0x4,%eax; lea &dt,%edx; cmp %edx,%eax; je;
    //     movl $7,0x8(%esp); lea &dt,%edx; mov %edx,0x4(%esp); mov %eax,(%esp); call memcpy
    //   4.4.6/4.4.7 恒 12 条：比较段 `lea 0x4(%eax),%edx`（无 add 形、cmp 操作数序随分配反向）
    //     + memcpy 段重发 `mov this,%eax; lea 0x4(%eax),%edx`（+2 条 / +6 字节）。
    //   穷举证伪（/tmp/b28d_solo/probe{,2,3}.cpp，18 源形 + 4.4.6 后端）全部产同一 12 条形：
    //     A &m_dt!=&dt / B 反向 / C (void*)cast / D 双 cast / E 空 else / F while+break /
    //     G this->m_dt / H 正条件包裹 / I 三目 / J (char*)this+4 / K/L 混合 / M 三目语句 /
    //     N && 短路 / O 裸 register 指针 / P 裸 register 反向 / Q 具名指针 / R 裸 register 引用；
    //     Q 占槽（dt 槽平移 -0x14，帧劣化）、O/R 改用 %ebx 跨 call 并多 push/pop ebx（+2 条，反劣）。
    //   P248 三档类型旋钮不适用（纯地址比较，无 callee 返回类型/字段类型参与）；
    //   裸 register 第三条件（ORIG 目标寄存器为 callee-saved）不成立（ORIG 用 %eax）⇒ 不留 register。
    //   语义闭合证据：p167 调用点差集 7/7 CLEAR（C1/DiffDay/CloseFile/CreateLogFile/memcpy/fprintf/fflush）；
    //   自赋值防护与拷贝长度 7 与 ORIG 一致。
    // [N6S2NegB 2026-09-11 复核] 真残差定位（n1_v: DIFF orig=57 our=59 gap=-2）：唯一差段是
    //   4.4 后端在 memcpy 段**重发** `mov 0x8(%ebp),%eax; lea 0x4(%eax),%edx`（+2 条），
    //   而 ORIG 0x0854fc80 `mov %eax,(%esp)` 复用比较段已存活的 %eax（=this+4），
    //   且 ORIG 比较段用 `add $0x4,%eax`（4.1 后端形）而非 4.4 的 `lea 0x4(%eax),%edx`。
    //   穷举复验（/tmp/probe_wl/v1..v4：正/反向比较、具名指针、`m_dt = dt` 结构赋值）+
    //   446 后端：全部 12 条形；v3 结构赋值另引入 3 段内联拷贝（33 条）更劣，v4 具名指针
    //   多 -0xc 槽。无源形能让 4.4 跨 je 复用 %eax 寄存器。
    //   结论：N 封顶（编译器后端差异），保持现状 59 条；无源码可消除这 2 条。
    if (&m_dt != &dt) {
        memcpy(&m_dt, &dt, 7);
    }
    fprintf(m_fp, "%s\r\n", msg);
    fflush(m_fp);
    return 1;
}