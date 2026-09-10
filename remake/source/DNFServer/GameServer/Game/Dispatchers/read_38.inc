int Dispatcher_SecurityLog::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x0825fabc：int 中转 → 0xd；循环 5 个 int → 0x11/0x15/0x19/0x1d/0x21；失败 0x99a/0x9a2
    // [b28d-solo-S206] P0：ORIG 0x0825fabc 无 msg 指针局部（`lea -0x10(%ebp),%eax`
    //   直传 &first，`mov 0x10(%ebp),%eax` 直接用形参槽）+ first 无预清零
    //   （无 movl $0）⇒ 删指针局部、去初始化式。
    int first;
    if (!packet.get_int(first))
        return 0x99a;
    // [b28d-solo-S206] P211 档③ + disp 折叠：ORIG `mov 0x10(%ebp),%eax;
    //   mov -0x10(%ebp),%edx; mov %edx,0xd(%eax)`（LHS msg 先载、值直存 disp）；
    //   裸 cast 解引用恒产 lea+中转 ⇒ 改具名视图成员。
    struct __attribute__((packed)) SecurityLogView
    {
        char m_head[0xd];
        int m_first;              // +0x0d
    };
    reinterpret_cast<SecurityLogView*>((char*)&msg)->m_first = first;
    for (int i = 0; i < 5; ++i)
    {
        // [FL1 2026-09-10 负结论] ORIG 0x0825fafa 加法序 =
        //   `add $0x4,%edx; shl $0x2,%edx; add %edx,%eax; add $0x1,%eax`
        //   （+1 加在「基址+缩放」之后）。三形实测：
        //   ① `msg + (i+4)*4 + 1`                    → 41/41 NEAR（最优，现形）
        //   ② `(char*)&((uint*)&msg)[i+4] + 1`       → 同 ①（+1 仍折索引侧）
        //   ③ `(uint)((uint*)&msg)[i+4] + 1`（P319 整型加法）→ 42 条且
        //      TU ae−1/diff+1（劣化，已回滚）
        // ⇒ 4.4 恒把尾 +1 折进索引侧，属 P205 封顶（旧注释 probe_add2 同结论）。
        if (!packet.get_int(*reinterpret_cast<unsigned int*>(
                (char*)&msg + (i + 4) * 4 + 1)))
            return 0x9a2;
    }
    return 0;
